#!/usr/bin/env python3

# External command, intended to be called with add_dist_script() in meson.build

#                        argv[1]         argv[2]        argv[3:]
# extra-dist-cmd.py <root_source_dir> <root_build_dir> <no_dist>...

# Meson does not preserve timestamps on distributed files. Neither does this script.

import os
import sys
import shutil
import subprocess
import re

root_source_dir = sys.argv[1]
root_build_dir = sys.argv[2]
dist_root = os.getenv('MESON_DIST_ROOT')

# Make a ChangeLog file for distribution.
cmd = [
  'git',
  '--git-dir=' + os.path.join(root_source_dir, '.git'),
  '--work-tree=' + root_source_dir,
  'log',
  '--no-merges',
  '--date=short',
  '--max-count=200',
  '--pretty=tformat:%cd  %an  <%ae>%n%n  %s%n%w(0,0,2)%+b',
]
with open(os.path.join(dist_root, 'ChangeLog'), mode='w') as logfile:
  result = subprocess.run(cmd, stdout=logfile)
  if result.returncode:
    sys.exit(result.returncode)

# Distribute some built files in addition to the files in the local git clone.
os.chdir(root_build_dir)
dist_docs_tutorial = os.path.join(dist_root, 'docs', 'tutorial')
dist_docs_tutorial_C = os.path.join(dist_docs_tutorial, 'C')
docs_tutorial_index_docbook = os.path.join('docs', 'tutorial', 'index.docbook')

# English index.docbook and html files.
# shutil.copy() does not copy timestamps.
shutil.copy(docs_tutorial_index_docbook, dist_docs_tutorial_C)
shutil.copytree(os.path.join('docs', 'tutorial', 'html'),
                os.path.join(dist_docs_tutorial, 'html'),
                copy_function=shutil.copy)

# Read the distributed LINGUAS file, containing a list of available translations.
linguas = os.path.join(dist_docs_tutorial, 'LINGUAS')
langs = []
if os.path.isfile(linguas):
  with open(linguas) as linguas_file_obj:
    buffer = linguas_file_obj.read().splitlines()
  comment_pattern = re.compile(r'\s*(?:#|$)') # comment or blank line
  for line in buffer:
    if not comment_pattern.match(line):
      langs += line.split()
else:
  print('=== Warning: File', linguas, 'not found.')

# .gmo files with translations and translated index.docbook files.
for lang in langs:
  for file in [lang + '.gmo', 'index.docbook']:
    shutil.copy(os.path.join('docs', 'tutorial', lang, file),
                os.path.join(dist_docs_tutorial, lang))

# If there is an updated PDF file, include it in the tarball.
pdf_file = os.path.join('docs', 'tutorial', 'programming-with-gtkmm.pdf')
if os.path.isfile(pdf_file) and \
   os.stat(pdf_file).st_mtime > os.stat(docs_tutorial_index_docbook).st_mtime:
  shutil.copy(pdf_file, dist_docs_tutorial_C)
else:
  print('--- Info: No updated PDF file found.')

# Don't distribute .gitignore files.
for dirpath, dirnames, filenames in os.walk(dist_root):
  if '.gitignore' in filenames:
    os.remove(os.path.join(dirpath, '.gitignore'))

# Remove an empty MESON_DIST_ROOT/build directory.
dist_build_dir = os.path.join(dist_root, 'build')
if os.path.isdir(dist_build_dir):
  try:
    os.rmdir(dist_build_dir)
  except OSError:
    # Ignore the error, if not empty.
    pass

# Remove specified files and directories from the MESON_DIST_ROOT directory.
for rel_path in sys.argv[3:]:
  abs_path = os.path.join(dist_root, rel_path)
  if os.path.isfile(abs_path):
    os.remove(abs_path)
  elif os.path.isdir(abs_path):
    shutil.rmtree(abs_path, ignore_errors=True)
  else:
    # Ignore non-existent files and directories.
    print('--- Info:', abs_path, 'not found, not removed.')

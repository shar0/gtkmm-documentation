#!/bin/sh -e

# External command, intended to be called with add_dist_script() in meson.build

# extra-dist-cmd.sh <root_source_dir> <root_build_dir>

# Meson does not preserve timestamps on distributed files. Neither does this script.

# Make a ChangeLog file for distribution.
git --git-dir="$1/.git" --work-tree="$1" log --no-merges --date=short --max-count=200 \
    --pretty='tformat:%cd  %an  <%ae>%n%n  %s%n%w(0,0,2)%+b' > "$MESON_DIST_ROOT/ChangeLog"

# Distribute some built files in addition to the files in the local git clone.
cd "$2"
dist_docs_tutorial="$MESON_DIST_ROOT/docs/tutorial"

# English index.docbook and html files.
cp "docs/tutorial/index.docbook" "$dist_docs_tutorial/C/"
# -R == --recursive (Posix does not support long options.)
cp -R "docs/tutorial/html/" "$dist_docs_tutorial/"

# Read the distributed LINGUAS file, containing a list of available translations.
linguas="$dist_docs_tutorial/LINGUAS"
langs=
if [ -f "$linguas" ]; then
  langs="$(sed '/^ *#/d' "$linguas")"
else
  echo "=== Warning: File $linguas not found."
fi

# .gmo files with translations and translated index.docbook files.
for lang in $langs; do
  for file in "$lang.gmo" "index.docbook"; do
    cp "docs/tutorial/$lang/$file" "$dist_docs_tutorial/$lang/"
  done
done

# If there is an updated PDF file, include it in the tarball.
pdf_file="docs/tutorial/programming-with-gtkmm.pdf"
if [ -f "$pdf_file" -a "$pdf_file" -nt "docs/tutorial/index.docbook" ]; then
  cp "$pdf_file" "$dist_docs_tutorial/C/"
else
  echo "--- Info: No updated PDF file found."
fi

# Remove all .gitignore files and an empty $MESON_DIST_ROOT/build directory.
find "$MESON_DIST_ROOT" -name ".gitignore" -exec rm '{}' \;
if [ -d "$MESON_DIST_ROOT/build" ]; then
  # Ignore the error, if not empty.
  rmdir "$MESON_DIST_ROOT/build" 2>/dev/null || true
fi

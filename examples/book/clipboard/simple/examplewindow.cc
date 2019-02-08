/* gtkmm example Copyright (C) 2002 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "examplewindow.h"
#include <iostream>

ExampleWindow::ExampleWindow()
: m_VBox(Gtk::Orientation::VERTICAL),
  m_Label("Select cells in the grid, click Copy, then open a second "
        "instance of this example to try pasting the copied data."),
  m_ButtonA1("A1"), m_ButtonA2("A2"), m_ButtonB1("B1"), m_ButtonB2("B2"),
  m_Button_Copy("_Copy", /* mnemonic= */ true), m_Button_Paste("_Paste", true)
{
  set_title("Gtk::Clipboard example");

  m_VBox.set_margin(12);
  add(m_VBox);

  m_VBox.add(m_Label);

  //Fill Grid:
  m_VBox.add(m_Grid);
  m_Grid.set_expand(true);
  m_Grid.set_row_homogeneous(true);
  m_Grid.set_column_homogeneous(true);
  m_Grid.attach(m_ButtonA1, 0, 0);
  m_Grid.attach(m_ButtonA2, 1, 0);
  m_Grid.attach(m_ButtonB1, 0, 1);
  m_Grid.attach(m_ButtonB2, 1, 1);

  //Add ButtonBox to bottom:
  m_VBox.add(m_ButtonBox);
  m_VBox.set_spacing(6);

  //Fill ButtonBox:
  m_ButtonBox.add(m_Button_Copy);
  m_Button_Copy.set_hexpand(true);
  m_Button_Copy.set_halign(Gtk::Align::END);
  m_Button_Copy.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_copy) );
  m_ButtonBox.add(m_Button_Paste);
  m_Button_Paste.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_paste) );
}

ExampleWindow::~ExampleWindow()
{
}

void ExampleWindow::on_button_copy()
{
  //Build a string representation of the stuff to be copied:
  //Ideally you would use XML, with an XML parser here:
  Glib::ustring strData;
  strData += m_ButtonA1.get_active() ? "1" : "0";
  strData += m_ButtonA2.get_active() ? "1" : "0";
  strData += m_ButtonB1.get_active() ? "1" : "0";
  strData += m_ButtonB2.get_active() ? "1" : "0";

  get_clipboard()->set_text(strData);
}

void ExampleWindow::on_button_paste()
{
  //Tell the clipboard to call our method when it is ready:
  get_clipboard()->read_text_async(sigc::mem_fun(*this,
              &ExampleWindow::on_clipboard_text_received));
}

void ExampleWindow::on_clipboard_text_received(Glib::RefPtr<Gio::AsyncResult>& result)
{
  Glib::ustring text;
  try
  {
    text = get_clipboard()->read_text_finish(result);
  }
  catch (const Glib::Error& err)
  {
    // Print an error about why pasting failed.
    // Usually you probably want to ignore such failures,
    // but for demonstration purposes, we show the error.
    std::cout << "Pasting failed: " << err.what() << std::endl;
  }

  //See comment in on_button_copy() about this silly clipboard format.
  if(text.size() >= 4)
  {
    m_ButtonA1.set_active( text[0] == '1' );
    m_ButtonA2.set_active( text[1] == '1' );
    m_ButtonB1.set_active( text[2] == '1' );
    m_ButtonB2.set_active( text[3] == '1' );
  }
}

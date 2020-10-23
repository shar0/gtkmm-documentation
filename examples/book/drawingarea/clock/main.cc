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

#include "clock.h"
#include <gtkmm/application.h>
#include <gtkmm/window.h>

class ExampleWindow : public Gtk::Window
{
public:
  ExampleWindow();

protected:
  Clock m_clock;
};

ExampleWindow::ExampleWindow()
{
  set_title("Cairomm Clock");
  set_child(m_clock);
}

int main(int argc, char** argv)
{
  auto app = Gtk::Application::create("org.gtkmm.example");

  return app->make_window_and_run<ExampleWindow>(argc, argv);
}

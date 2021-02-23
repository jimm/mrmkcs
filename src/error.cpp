#include <iostream>
#include "error.h"
// #include "curses/gui.h"

void error_message(const char * const msg) {
  std::cerr << msg << '\n';
  // GUI *gui = gui_instance();
  // if (gui != nullptr) {
  //   gui->show_message(msg);
  //   gui->clear_message_after(10);
  // }
}

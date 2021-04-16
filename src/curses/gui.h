#ifndef GUI_H
#define GUI_H

#include "../mrmkcs.h"
#include "list_window.h"

using namespace std;

class InfoWindow;
class Window;

class GUI {
public:
  GUI(MrMKCS *mrm);
  ~GUI();

  void run();

  void show_message(string);
  void clear_message();
  void clear_message_after(int);
  int clear_message_seconds() { return clear_msg_secs; }
  int clear_message_id() { return clear_msg_id; }

private:
  MrMKCS *mrm;
  WindowLayout layout;
  Window *message;
  InfoWindow *info;

  int clear_msg_secs;
  int clear_msg_id;

  void event_loop();
  void config_curses();
  void create_windows();
  void resize_windows();
  void free_windows();
  void refresh_all();
  void set_window_data();
  void close_screen();
  void help();
  void load();
  void load(string path);
  void save();
  void save(string path);
  void reload();
};

GUI *gui_instance();

#endif /* GUI_H */

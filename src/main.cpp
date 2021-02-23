#include <iostream>
#include <getopt.h>
#include <libgen.h>
#include <portmidi.h>
#include "error.h"
#include "mrmkcs.h"
// #include "curses/gui.h"

using std::cerr;

struct opts {
  bool list_devices;
  bool testing;
} opts;

void list_devices(const char *title, const PmDeviceInfo *infos[], int num_devices) {
  printf("%s:\n", title);
  for (int i = 0; i < num_devices; ++i)
    if (infos[i] != nullptr) {
      const char *name = infos[i]->name;
      const char *q = (name[0] == ' ' || name[strlen(name)-1] == ' ') ? "\"" : "";
      printf("  %2d: %s%s%s%s\n", i, q, name, q, infos[i]->opened ? " (open)" : "");
    }
}

void list_all_devices() {
  int num_devices = Pm_CountDevices();
  const PmDeviceInfo *inputs[num_devices], *outputs[num_devices];

  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    inputs[i] = info->input ? info : 0;
    outputs[i] = info->output ? info : 0;
  }

  list_devices("Inputs", inputs, num_devices);
  list_devices("Outputs", outputs, num_devices);
}

void cleanup() {
  PmError err = Pm_Terminate();
  if (err != 0)
    fprintf(stderr, "error terminating PortMidi: %s\n", Pm_GetErrorText(err));
}

void initialize() {
  PmError err = Pm_Initialize();
  if (err != 0) {
    fprintf(stderr, "error initializing PortMidi: %s\n", Pm_GetErrorText(err));
    exit(1);
  }

  // Pm_Initialize(), when it looks for default devices, can set errno to a
  // non-zero value. Reinitialize it here.
  errno = 0;

  atexit(cleanup);
}

void load_mrm(const char *path, bool testing) {
  // Storage storage(path);
  // if (storage.load(testing) == nullptr) // sets MRM instance as a side-effect
  //   exit(1);                           // error already printed
  // if (storage.has_error()) {
  //   cerr << "error: " << storage.error() << '\n';
  //   exit(1);
  // }
}

void initialize_mrm(bool testing) {
  // KeyMaster *mrm = new KeyMaster();
  // mrm->testing = testing;
  // mrm->initialize();
}

void run_curses() {
  // KeyMaster_instance()->start();
  // GUI gui(KeyMaster_instance());
  // gui.run();
  // // Don't save MRM above and use it here. User might have loaded a new one.
  // KeyMaster_instance()->stop();
}

void usage(const char *prog_name) {
  const char * const usage_data[] = {
    "-l or --list-ports",
    "List all attached MIDI ports",
    "-n or --no-midi",
    "No MIDI (ignores bad/unknown MIDI ports)",
    "-h or --help",
    "This help",
  };
  cerr << "usage: " << basename((char *)prog_name) << " [-l] [-n] [-w] [-t] [-v VIEW] [-c] [-h] file\n";
  for (int i = 0; i < sizeof(usage_data) / sizeof(char *); ++i) {
    cerr << '\n';
    cerr << "    " << usage_data[i++] << '\n';
    cerr << "        " << usage_data[i] << '\n';
  }
}

void parse_command_line(int argc, char * const *argv, struct opts *opts) {
  int ch, testing = false;
  char *prog_name = argv[0];
  static struct option longopts[] = {
    {"list", no_argument, 0, 'l'},
    {"no-midi", no_argument, 0, 'n'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  };

  opts->list_devices = opts->testing = false;
  while ((ch = getopt_long(argc, argv, "lnh", longopts, 0)) != -1) {
    switch (ch) {
    case 'l':
      opts->list_devices = true;
      break;
    case 'n':
      opts->testing = true;
      break;
    case 'h': default:
      usage(prog_name);
      exit(ch == '?' || ch == 'h' ? 0 : 1);
    }
  }
}

int main(int argc, char * const *argv) {
  struct opts opts;
  const char *prog_name = argv[0];

  parse_command_line(argc, argv, &opts);
  argc -= optind;
  argv += optind;

  if (opts.list_devices) {
    list_all_devices();
    exit(0);
  }

  initialize();

  if (argc > 0)
    load_mrm(argv[0], opts.testing);
  else
    initialize_mrm(opts.testing);

  run_curses();

  exit(0);
  return 0;
}

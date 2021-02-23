#ifndef MRMKCS_H
#define MRMKCS_H

#include <vector>
#include <portmidi.h>
#include "song.h"
#include "clock.h"

using std::vector;

class Input;
class Output;
class Song;
class SequencePlayer;

class MrMKCS {
public:
  MrMKCS();
  ~MrMKCS();

  // ================ running ================
  void start();
  void stop();
  void midi_in(PmMessage msg);

  SequencePlayer *play_sequence(int n) { return _song->play_sequence(n, nullptr, 0, 1, 0); }

  // ================ accessors ================
  inline vector<Input *> &inputs() { return _inputs; }
  inline vector<Output *> &outputs() { return _outputs; }
  inline Song *song() { return _song; }
  inline void song(Song *song) { _song = song; }
  inline Clock &clock() { return _clock; }
  inline bool is_testing() { return _testing; }

  void set_testing(bool val) { _testing = val; }

  // ================ initialization ================
  void initialize();

  // ================ clock ================
  void start_clock();
  void continue_clock();
  void stop_clock();
  void toggle_clock();
  bool clock_is_running();
  void set_clock_bpm(int bpm);

  // ================ memory_management ================
  void register_for_cleanup(SequencePlayer *sp);

  // ================ testing only ================
  void add_input(Output *output) { _outputs.push_back(output); }
  void add_output(Output *output) { _outputs.push_back(output); }

private:
  vector<Input *> _inputs;
  vector<Output *> _outputs;
  set<SequencePlayer *> _finished_sequence_players;
  mutex _finished_sequence_players_mutex;
  Clock _clock;
  bool _running;
  bool _testing;
  Song *_song;

  void cleanup();
};

MrMKCS *MrMKCS_instance();

#endif /* MRMKCS_H */

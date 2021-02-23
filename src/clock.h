#ifndef CLOCK_H
#define CLOCK_H

#include <set>
#include <pthread.h>
#include <portmidi.h>

class Output;
class SequencePlayer;

class Clock {
public:
  std::vector<Output *> &outputs;
  unsigned long microsecs_per_tick;

  Clock(std::vector<Output *> &outputs);
  ~Clock();

  float bpm() { return _bpm; }
  void set_bpm(float bpm);

  void start();
  void continue_clock();
  void stop();
  void tick();
  bool is_running() { return thread != nullptr; }
  inline long time() { return _ticks_since_start; }

  void add_player(SequencePlayer *seq_player);
  void remove_player(SequencePlayer *seq_player);

protected:
  float _bpm;
  long _ticks_since_start;
  pthread_t thread;
  std::set<SequencePlayer *> players;

  void send(PmMessage msg);
  void start_or_continue(PmMessage msg);
};

#endif /* CLOCK_H */

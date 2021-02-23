#ifndef SEQUENCE_PLAYER_H
#define SEQUENCE_PLAYER_H

#include "clock.h"
#include "sequence.h"
#include "sequence_player.h"

enum SequencePlayerState {
  sps_stopped,
  sps_playing,
  sps_finished
};

class SequencePlayer {
public:
  SequencePlayer(Sequence &seq, SequencePlayer *parent, int transpose, int repeats, bool parent_waiting);
  virtual ~SequencePlayer();    // virtual for testing

  void play();
  void stop();
  virtual void tick(long time); // virtual for testing
  void return_from_wait(SequencePlayer *child);

private:
  Clock &clock;
  Sequence &seq;
  SequencePlayer *parent;       // may be null
  SequencePlayerState state;
  int xpose;
  int curr_event_index;
  Event *curr_event;
  long waiting_until;
  int repeats_remaining;
  bool tell_parent_when_done;
  // index is event number
  vector<int> pending_note_off_event_indexes;
  set<SequencePlayer *>children;

  Event *next_event();
  void play_midi_event();
  void play_sequence();

  void done_playing();

  void cleanup_notes_and_controllers();
  void play_pending_note_offs();
  void play_pending_sustain_offs();
  void play_pending_poly_pressure_offs();
};

#endif /* SEQUENCE_PLAYER_H */

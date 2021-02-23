#include "sequence_player.h"
#include "mrmkcs.h"
#include "vector_utils.h"

/** NOTE: SequencePlayers delete themselves! **/

SequencePlayer::SequencePlayer(Sequence &s, SequencePlayer *c, int transpose, int repeats, bool parent_waiting)
  : clock(MrMKCS_instance()->clock()), seq(s), parent(c), xpose(transpose),
    state(sps_stopped), curr_event_index(-1), waiting_until(0), repeats_remaining(repeats),
    tell_parent_when_done(parent_waiting)
{
}

SequencePlayer::~SequencePlayer() {
  clock.remove_player(this);
}

void SequencePlayer::play() {
  pending_note_off_event_indexes.clear();
  state = sps_playing;

  curr_event_index = -1;
  curr_event = next_event();
  if (curr_event == nullptr) {
    done_playing();
    return;
  }
  waiting_until = clock.time() + curr_event->tick_offset;

  // remove_player gets called from MrMKCS::cleanup().
  clock.add_player(this);
}

void SequencePlayer::stop() {
  state = sps_stopped;
  cleanup_notes_and_controllers();
  for (auto child : children)
    child->stop();
}

void SequencePlayer::tick(long time) {
  if (state != sps_playing)
    return;

  if (time < waiting_until)
    return;

  do {
    if (curr_event->is_midi())
      play_midi_event();
    else
      play_sequence();
    curr_event = next_event();
  } while (curr_event != nullptr && curr_event->tick_offset == 0);
  if (curr_event != nullptr)
    waiting_until = time + curr_event->tick_offset;
}

void SequencePlayer::return_from_wait(SequencePlayer *child) {
  children.erase(child);
  state = sps_playing;
}

Event *SequencePlayer::next_event() {
  ++curr_event_index;
  if (curr_event_index >= seq.events.size()) {
    --repeats_remaining;
    if (repeats_remaining == 0) {
      // Stop and clean up this player but not our children, they may still
      // be running.
      state = sps_stopped;
      cleanup_notes_and_controllers();

      done_playing();
      return nullptr;
    }
  }

  return seq.events[curr_event_index];
}

// Play curr_event as a MIDI event
void SequencePlayer::play_midi_event() {
  int i;
  uint8_t byte;

  switch (curr_event->status & 0xf0) {
  case NOTE_ON:
    i = seq.index_of_note_off_for_note_on(curr_event_index);
    if (i > curr_event_index)
        pending_note_off_event_indexes.push_back(i);
    break;
  case NOTE_OFF:
    erase(pending_note_off_event_indexes, curr_event_index);
    break;
  }

  bool do_xpose = curr_event->is_note();
  if (do_xpose)
    curr_event->data1 += xpose;
  curr_event->send();
  if (do_xpose)
    curr_event->data1 -= xpose;
}

void SequencePlayer::play_sequence() {
  Song *song = MrMKCS_instance()->song();
  SequencePlayer *player = song->play_sequence(
    curr_event->sequence_number(), this, curr_event->transpose(),
    curr_event->repeats(), curr_event->wait);
  children.insert(player);

  // You might think we want to call the player's `tick` method. However, as
  // a side-effect of the player registering itself with the clock, the
  // clock is going to send a tick to it because it's iterating through the
  // vector of registered players and we've just added it so it will be
  // picked up. I'm not sure I like relying on that behavior (the vector
  // being modified while it's being iterated) but we've documented it here
  // at least.
}

void SequencePlayer::done_playing() {
  if (tell_parent_when_done && parent != nullptr)
    parent->return_from_wait(this);
  state = sps_finished;
  MrMKCS_instance()->register_for_cleanup(this);
}

// ================ cleanup ================

void SequencePlayer::cleanup_notes_and_controllers() {
  play_pending_note_offs();
}

void SequencePlayer::play_pending_note_offs() {
  for (auto i : pending_note_off_event_indexes) {
    Event *e = seq.events[i];
    e->data1 += xpose;
    seq.events[i]->send();
    e->data1 -= xpose;
  }
  pending_note_off_event_indexes.clear();
}

#ifndef SONG_H
#define SONG_H

#include <string>
#include "sequence.h"
#include "sequence_player.h"

#define NUM_SEQUENCES 36

class Song {
public:
  Song();

  inline Sequence &sequence(int n) { return sequences[n]; }
  SequencePlayer *play_sequence(int n, SequencePlayer *parent, int transpose, int repeats, uint8_t flags);

private:
  string name;
  uint8_t beats_per_measure;
  float bpm;
  Sequence sequences[NUM_SEQUENCES]; // 0-9, A-Z
};

// Translates character `c` ([0-9A-Za-z]) into a sequence number.
int char_to_seq_number(char c);

#endif /* SONG_H */

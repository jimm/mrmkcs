#include "song.h"
#include "sequence_player.h"

// Translates character `c` ([0-9A-Za-z]) into a sequence number.
int char_to_seq_number(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'Z')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'z')
    return c - 'a' + 10;
  return -1;                    // help!
}

Song::Song()
  : beats_per_measure(4), bpm(120.0)
{
}

SequencePlayer *Song::play_sequence(int n, SequencePlayer *parent, int transpose, int repeats, uint8_t flags) {
  if (n < 0 || n >= NUM_SEQUENCES)
    return nullptr;
  SequencePlayer *player = new SequencePlayer(sequences[n], parent, transpose, repeats, flags);
  player->play();
  return player;
}

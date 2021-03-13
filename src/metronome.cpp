#include "mrmkcs.h"
#include "metronome.h"

Metronome::Metronome() : Metronome(MrMKCS_instance()->clock()) {}

Metronome::Metronome(Clock &c) : clock(c) {}

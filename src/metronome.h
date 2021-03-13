#ifndef METRONOME_H
#define METRONOME_H

class Metronome {
public:
  Metronome();
  Metronome(Clock &clock);

private:
  Clock &clock;
};


#endif /* METRONOME_H */

#include "consts.h"
#include "event.h"

// ================ Event ================

Event::Event()
  : tick_offset(0), output(0), status(0), data1(0), data2(0), wait(false)
{
}

Event::Event(const Event &other)
  : tick_offset(other.tick_offset), output(other.output), status(other.status),
    data1(other.data1), data2(other.data2), wait(other.wait)
{
}

Event::Event(uint16_t _tick_offset, Output *_output,
             uint8_t _status, uint8_t _data1, uint8_t _data2, bool _wait)
  : tick_offset(_tick_offset), output(_output),
    status(_status), data1(_data1), data2(_data2), wait(_wait)
{
}

void Event::send() {
  output->write_midi(status, data1, data2);
}

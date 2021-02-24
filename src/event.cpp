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
             uint8_t _status, uint8_t _data1, uint8_t _data2)
  : tick_offset(_tick_offset), output(_output),
    status(_status), data1(_data1), data2(_data2), wait(false)
{
}

Event::Event(uint16_t _tick_offset, Output *_output, PmMessage msg)
  : tick_offset(_tick_offset), output(_output),
    status(Pm_MessageStatus(msg)), data1(Pm_MessageData1(msg)), data2(Pm_MessageData2(msg)),
    wait(false)
{
}

Event::Event(uint8_t _tick_offset, int sequence_number, int transpose, int repeats, bool _wait)
  : tick_offset(_tick_offset), output(nullptr), status((uint8_t)sequence_number),
    data1((uint8_t)transpose), data2((uint8_t)repeats), wait(_wait)
{
}

void Event::send() {
  output->write(status, data1, data2);
}

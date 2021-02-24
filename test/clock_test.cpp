#include "catch.hpp"
#include "test_helper.h"
#include "../src/consts.h"
#include "../src/clock.h"
#include "../src/sequence_player.h"

#define CATCH_CATEGORY "[clock]"

class TestClockListener : public SequencePlayer {
public:
  TestClockListener(Sequence &s)
    : SequencePlayer(s, nullptr, 0, 1, false), last_tick_seen(0)
    {}
  virtual ~TestClockListener() {}

  void tick(long time) {
    last_tick_seen = time;
  }

  long time() { return last_tick_seen; }

private:
  long last_tick_seen;
};


TEST_CASE("clock bpm math", CATCH_CATEGORY) {
  auto outputs = std::vector<Output *>();
  auto clock = Clock(outputs);

  REQUIRE(clock.bpm() == 120.0);
  REQUIRE(clock.microsecs_per_tick == (long)(5.0e5 / (float)CLOCK_TICKS_PER_QUARTER_NOTE));

  clock.set_bpm(60);
  REQUIRE(clock.bpm() == 60.0);
  REQUIRE(clock.microsecs_per_tick == (long)(1.0e6 / (float)CLOCK_TICKS_PER_QUARTER_NOTE));

  clock.set_bpm(240);
  REQUIRE(clock.bpm() == 240.0);
  REQUIRE(clock.microsecs_per_tick == (long)(2.5e5 / (float)CLOCK_TICKS_PER_QUARTER_NOTE));
}

TEST_CASE("clock tick sends CLOCK to outputs", CATCH_CATEGORY) {
  auto outputs = std::vector<Output *>();
  Output output(pmNoDevice, "out1", "out 1 port");
  auto clock = Clock(outputs);

  outputs.push_back(&output);

  clock.tick();
  REQUIRE(output.num_io_messages == 1);
  REQUIRE(output.io_messages[0] == CLOCK_MESSAGE);

  clock.tick();
  REQUIRE(output.num_io_messages == 2);
  REQUIRE(output.io_messages[1] == CLOCK_MESSAGE);
}

TEST_CASE("clock time correct and sent to players", CATCH_CATEGORY) {
  MrMKCS *mrm = create_test_data();
  auto clock = mrm->clock();
  auto seq = mrm->song()->sequence(0);
  TestClockListener player(seq);

  clock.add_observer(&player);

  REQUIRE(clock.time() == 0);
  REQUIRE(player.time() == 0);

  clock.tick();
  REQUIRE(clock.time() == 1);
  REQUIRE(player.time() == 1);

  for (int i = 0; i < 10; ++i)
    clock.tick();
  REQUIRE(clock.time() == 11);
  REQUIRE(player.time() == 11);

  delete mrm;
}

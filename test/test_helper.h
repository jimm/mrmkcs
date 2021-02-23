#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include "../src/mrmkcs.h"

MrMKCS *create_test_data();
void clock_ticks(int n);
void print_messages(Instrument *inst);

extern PmMessage CLOCK_MESSAGE;

#endif /* TEST_HELPER_H */

/*
 * Original author: Venki
 */
#ifndef _TM1637_H_
#define _TM1637_H_

#include <stdint.h>
#include "bcd.h"

void tm1637_setup(void);
void tm1637_display_bcd_time(struct bcd_time);

#endif /* _TM1637_H_ */

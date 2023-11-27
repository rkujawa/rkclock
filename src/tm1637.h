/*
 * Original author: Venki
 */
#include <stdint.h>

#ifndef _TM1637_H_
#define _TM1637_H_

void tm1637_setup(void);
void tm1637_demo(void);
void tm1637_display_decimal(int v, bool display_separator);
void tm1637_set_brightness(uint8_t brightness);
#endif /* _TM1637_H_ */

/*
 * Original author: Venki
 */

#ifndef _TM1637_H_
#define _TM1637_H_

void tm1637_init(void);
void tm1637_demo(void);
void tm1637_display_decimal(int v, int display_separator);
void tm1637_set_brightness(char brightness);
void tm1637_start(void);
void tm1637_stop(void);
void tm1637_read_result(void);
void tm1637_write_byte(unsigned char b);
void tm1637_delay_usec(unsigned int i);
void tm1637_clk_high(void);
void tm1637_clk_low(void);
void tm1637_data_high(void);
void tm1637_data_low(void);
#endif /* _TM1637_H_ */

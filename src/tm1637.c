/*
 * MIT License
 * Copyright (c) 2016 Roger Dahl
 *
 * tm1637.c
 *
 * Contains modifications by: Vidura Embedded
 * Contains modifications by: rkujawa
 */
#include <stdint.h>

#include <libopencm3/stm32/gpio.h>

#include "tm1637.h"

#define GPIO_PORT GPIOB
#define GPIO_CLK_PIN GPIO5
#define GPIO_DATA_PIN GPIO4

static void tm1637_start(void);
static void tm1637_stop(void);
static void tm1637_read_result(void);
static void tm1637_write_byte(uint8_t b);
static void tm1637_delay(uint32_t i);
inline static void tm1637_clk_high(void);
inline static void tm1637_clk_low(void);
inline static void tm1637_data_high(void);
inline static void tm1637_data_low(void);
static void tm1637_set_brightness(uint8_t brightness);

const char segmentMap[] = {
	0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
	0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
	0x00
};

inline static void
tm1637_clk_high(void)
{
	gpio_set(GPIO_PORT, GPIO_CLK_PIN);
}

inline static void 
tm1637_clk_low(void)
{
	gpio_clear(GPIO_PORT, GPIO_CLK_PIN);
}

inline static void
tm1637_data_high(void)
{
	gpio_set(GPIO_PORT, GPIO_DATA_PIN);
}

inline static void
tm1637_data_low(void)
{
	gpio_clear(GPIO_PORT, GPIO_DATA_PIN);
}

static void
tm1637_read_result(void)
{
	tm1637_clk_low();
	tm1637_delay(5);

	tm1637_clk_high();
	tm1637_delay(2);
	tm1637_clk_low();
}

/*
void 
tm1637_demo(void)
{
	uint8_t i = 0;

	tm1637_setup();
	tm1637_set_brightness(8);

	while(1){
		tm1637_display_decimal(i++, 0);
	}
}
*/

void 
tm1637_setup(void)
{
	gpio_mode_setup(GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_CLK_PIN);
	gpio_mode_setup(GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_DATA_PIN);
	tm1637_set_brightness(8);
}

static void
tm1637_write_segments(uint8_t digit_array[4])
{
	uint8_t i;

	tm1637_start();
	tm1637_write_byte(0x40);
	tm1637_read_result();
	tm1637_stop();

	tm1637_start();
	tm1637_write_byte(0xc0);
	tm1637_read_result();

	for (i = 0; i < 4; ++i) {
		tm1637_write_byte(digit_array[3 - i]);
		tm1637_read_result();
	}

	tm1637_stop();
}

void
tm1637_display_bcd_time(struct bcd_time bt)
{
	uint8_t digit_array[4];

	digit_array[3] = segmentMap[bt.ht];
	digit_array[2] = segmentMap[bt.hu];
	digit_array[1] = segmentMap[bt.mt];
	digit_array[0] = segmentMap[bt.mu];

	/* Turn on the separator. */
	digit_array[2] |= 1 << 7;

	tm1637_write_segments(digit_array);

}
/*
void 
tm1637_display_decimal(int v, bool display_separator)
{
	uint8_t i, digit_array[4];

	for (i = 0; i < 4; ++i) {
		digit_array[i] = segmentMap[v % 10];
		if (i == 2 && display_separator) {
			digit_array[i] |= 1 << 7;
		}
		v /= 10;
	}
	tm1637_write_segments(digit_array);
}
*/
/*
 * Valid brightness values: 0 - 8. 0 = display off.
 */
static void 
tm1637_set_brightness(uint8_t brightness)
{
	/*
	 * Brightness command:
	 * 1000 0XXX = display off
	 * 1000 1BBB = display on, brightness 0-7
	 * X = don't care
	 * B = brightness
	 */
	tm1637_start();
	tm1637_write_byte(0x87 + brightness);
	tm1637_read_result();
	tm1637_stop();
}

static void
tm1637_start(void)
{
	tm1637_clk_high();
	tm1637_data_high();
	tm1637_delay(2);
	tm1637_data_low();
}

static void
tm1637_stop(void)
{
	tm1637_clk_low();
	tm1637_delay(2);
	tm1637_data_low();
	tm1637_delay(2);
	tm1637_clk_high();
	tm1637_delay(2);
	tm1637_data_high();
}


static void
tm1637_write_byte(uint8_t b)
{
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		tm1637_clk_low();
		if (b & 0x01) {
			tm1637_data_high();
		}
		else {
			tm1637_data_low();
		}
		tm1637_delay(3);
		b >>= 1;
		tm1637_clk_high();
		tm1637_delay(3);
	}
}

/* 
 * Our tick timer runs relativly slow, at interval of 1ms, so here we just
 * implement delay with nops.
 */
static void
tm1637_delay(uint32_t i)
{
	uint8_t j;

	for (; i>0; i--) {
		for (j = 0; j < 100; ++j) {
			__asm__("nop");
		}
	}
}


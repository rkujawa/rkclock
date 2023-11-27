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
static void tm1637_delay_usec(uint32_t i);
inline static void tm1637_clk_high(void);
inline static void tm1637_clk_low(void);
inline static void tm1637_data_high(void);
inline static void tm1637_data_low(void);


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
	tm1637_delay_usec(5);

	tm1637_clk_high();
	tm1637_delay_usec(2);
	tm1637_clk_low();
}

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


void 
tm1637_setup(void)
{
	gpio_mode_setup(GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_CLK_PIN);
	gpio_mode_setup(GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_DATA_PIN);
	tm1637_set_brightness(8);
}


void 
tm1637_display_decimal(int v, bool display_separator)
{
	uint8_t digitArr[4];
	for (int i = 0; i < 4; ++i) {
		digitArr[i] = segmentMap[v % 10];
		if (i == 2 && display_separator) {
			digitArr[i] |= 1 << 7;
		}
		v /= 10;
	}

	tm1637_start();
	tm1637_write_byte(0x40);
	tm1637_read_result();
	tm1637_stop();

	tm1637_start();
	tm1637_write_byte(0xc0);
	tm1637_read_result();

	for (int i = 0; i < 4; ++i) {
		tm1637_write_byte(digitArr[3 - i]);
		tm1637_read_result();
	}

	tm1637_stop();
}

// Valid brightness values: 0 - 8.
// 0 = display off.
void 
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
	tm1637_delay_usec(2);
	tm1637_data_low();
}

static void
tm1637_stop(void)
{
	tm1637_clk_low();
	tm1637_delay_usec(2);
	tm1637_data_low();
	tm1637_delay_usec(2);
	tm1637_clk_high();
	tm1637_delay_usec(2);
	tm1637_data_high();
}


static void
tm1637_write_byte(uint8_t b)
{
	for (int i = 0; i < 8; ++i) {
		tm1637_clk_low();
		if (b & 0x01) {
			tm1637_data_high();
		}
		else {
			tm1637_data_low();
		}
		tm1637_delay_usec(3);
		b >>= 1;
		tm1637_clk_high();
		tm1637_delay_usec(3);
	}
}

static void
tm1637_delay_usec(uint32_t i)
{
	for (; i>0; i--) {
		for (int j = 0; j < 500; ++j) {
			__asm__("nop");
		}
	}
}


#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <libopencm3/cm3/systick.h>

#include "tm1637.h"
#include "key.h"
#include "rtc.h"
/* #include "usart.h" */

#ifdef STM32L0
#define MSI_DEFAULT_FREQ ((uint32_t)2097000) /* in Hz */
#endif 

static uint32_t ticks = 0;
static bool display_needs_update = true;

static void
sys_tick_setup(void)
{
	/* MSI Hz counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	/* MSI / 2097 = 1000 overflows per second - every 1ms one interrupt */
	systick_set_reload((MSI_DEFAULT_FREQ/1000)-1);

	systick_interrupt_enable();
	systick_counter_enable();
}

void 
sys_tick_handler(void)
{
	/* We call this handler every 1ms so one tick = 1ms. */
	ticks++;

	if ((ticks % 10) == 0)
		key_isr();

	/* 1000 ticks = 1s, 30000 ticks = 30s. */
	if (ticks == 30000) {
		/* Time to update the display with new value. */
		display_needs_update = true;
		ticks = 0;
	}
}

static inline void
rcc_gpio_setup(void)
{
	/* GPIOB is used for display and (optionally) Nucleo on-board LED. */
	rcc_periph_clock_enable(RCC_GPIOB);
	/* GPIOA is used for keys. */
	rcc_periph_clock_enable(RCC_GPIOA);
}

int
main(void)
{
	rcc_gpio_setup();
	key_setup();
	rtc_setup();
	/* usart_setup(); */
	sys_tick_setup();
	tm1637_setup();

	/* Nucleo LED is connected to pin 3 on GPIOB, uncomment if needed. */
	/*
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	gpio_set(GPIOB, GPIO3); 
	*/

	while(true) {

		switch(get_key_short(KEY_MASK)) {
			/* Handle hour key press. */
			case KEY_H:
				rtc_increment_hour();
				display_needs_update = true;
				break;
			/* Handle minute key press. */
			case KEY_M:
				rtc_increment_minute();
				display_needs_update = true;
				break;
		}

		/* Handle display update. */
		if (display_needs_update) {
			/* gpio_toggle(GPIOB, GPIO3); */
			tm1637_display_decimal(rtc_get_hourminute(), true); 
			display_needs_update = false;
		}
	}
}


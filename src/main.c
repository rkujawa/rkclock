#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "tm1637.h"
#include "usart.h"
#include "key.h"
#include "rtc.h"

#ifdef STM32L0
#define MSI_DEFAULT_FREQ ((uint32_t)2097000) /* in Hz */
#endif 

static void clock_hour_increment();
static void clock_minute_increment();

static uint32_t ticks = 0;

void 
sys_tick_handler(void)
{
	ticks++;

	/* We call this handler every 1ms so 1000ms = 1s on/off. */
/*	if (ticks == 1000) {
		gpio_toggle(GPIOB, GPIO3);
		ticks = 0;
	}
*/	

	if ((ticks % 10) == 0)
		key_isr();
}

static void
clock_hour_increment(void)
{
	gpio_toggle(GPIOB, GPIO3);
}

static void
clock_minute_increment(void)
{
	gpio_toggle(GPIOB, GPIO3);
}

int 
main(void) 
{
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	gpio_set(GPIOB, GPIO3);

	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO6);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO7);

	rtc_setup();

	usart_setup();
/*
	nvic_enable_irq(NVIC_RTC_IRQ);
	nvic_set_priority(NVIC_RTC_IRQ, 2);
*/

	/* MSI Hz counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	/* MSI / 2097 = 1000 overflows per second - every 1ms one interrupt */
	systick_set_reload((MSI_DEFAULT_FREQ/1000)-1);

	systick_interrupt_enable();
	systick_counter_enable();

	tm1637_setup();
//	tm1637_demo(); 

	while(true) {
/*		for (int i = 0; i < 50000; i++) {
			__asm__("nop");
		}
		usart_test();
*/
		tm1637_display_decimal(rtc_get_hourminute(), true);

		if (get_key_press(KEY_H)) {
			clock_hour_increment();
		}
		if (get_key_press(KEY_M)) {
			clock_minute_increment();
		}
	}
}


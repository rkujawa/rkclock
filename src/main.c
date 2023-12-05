#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/gpio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "tm1637.h"
#include "usart.h"
#include "key.h"

#ifdef STM32L0
#define MSI_DEFAULT_FREQ ((uint32_t)2097000) /* in Hz */
#endif 

static uint32_t ticks = 0;

static void 
rcc_rtc_select_clock(uint32_t clock)
{
	RCC_CSR &= ~(RCC_CSR_RTCSEL_MASK << RCC_CSR_RTCSEL_SHIFT);
	RCC_CSR |= (clock << RCC_CSR_RTCSEL_SHIFT);
}

static int 
rtc_setup(void)
{
	/* turn on power block to enable unlocking */
	rcc_periph_clock_enable(RCC_PWR);
	pwr_disable_backup_domain_write_protect();

	/* reset rtc */
	RCC_CSR |= RCC_CSR_RTCRST;
	RCC_CSR &= ~RCC_CSR_RTCRST;

	/* use the LSE clock */
	rcc_osc_on(RCC_LSE);
	rcc_wait_for_osc_ready(RCC_LSE);

	/* Select the LSE as rtc clock */
	rcc_rtc_select_clock(RCC_CSR_RTCSEL_LSE);

	RCC_CSR |= RCC_CSR_RTCEN;

	rtc_unlock();

	/* enter init mode */
	rtc_set_init_flag();

	rtc_wait_for_init_ready();

	/* set synch prescaler, using defaults for 1Hz out */
	uint32_t sync = 255;
	uint32_t async = 127;
	rtc_set_prescaler(sync, async);

	/* load time and date here if desired, and hour format */

	/* exit init mode */
	rtc_clear_init_flag();

	/* and write protect again */
	rtc_lock();

	/* and finally enable the clock */
	RCC_CSR |= RCC_CSR_RTCEN;

	/* And wait for synchro.. */
	rtc_wait_for_synchro();

//	rtc_interrupt_enable(RTC_SEC);

	return 0;
}

/*
void rtc_isr(void)
{
//	rtc_clear_flag(RTC_SEC);

	gpio_toggle(GPIOC, GPIO12);
	//usart_test();
}
*/

void 
sys_tick_handler(void)
{
	ticks++;

	/* We call this handler every 1ms so 1000ms = 1s on/off. */
	/*if (ticks == 1000) {
		gpio_toggle(GPIOB, GPIO3);
		ticks = 0;
	}
	*/

	if ((ticks % 10) == 0)
		key_isr();
}

int 
main(void) 
{
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	gpio_set(GPIOB, GPIO3);

	rtc_setup();
/*	tm1637_setup();
	tm1637_demo(); */

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

	while(1) {
		for (int i = 0; i < 50000; i++) {
			__asm__("nop");
		}
//		gpio_toggle(GPIOB, GPIO3);
		usart_test();

		if (get_key_press(KEY0)) {
			gpio_toggle(GPIOB, GPIO3);
		}
	}
}


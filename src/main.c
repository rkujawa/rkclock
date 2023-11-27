#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/gpio.h>

#include "tm1637.h"
#include "usart.h"

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
	return 0;
}

int 
main(void) 
{
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	gpio_set(GPIOB, GPIO3);

	rtc_setup();
	tm1637_setup();

	usart_setup();

	while(1) {
		for (int i = 0; i < 50000; i++) {
			__asm__("nop");
		}
		gpio_toggle(GPIOB, GPIO3);
		usart_test();
	}
}


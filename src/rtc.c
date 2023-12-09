#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>

#define RTC_TR_RESERVED_MASK	(0x007F7F7FU)

static void 
rcc_rtc_select_clock(uint32_t clock)
{
	RCC_CSR &= ~(RCC_CSR_RTCSEL_MASK << RCC_CSR_RTCSEL_SHIFT);
	RCC_CSR |= (clock << RCC_CSR_RTCSEL_SHIFT);
}

/* Convert 8-bit BCD to binary value. */
static 
uint8_t from_bcd(uint8_t val)
{
        return (10 * (val >> 4) + (0x0F & val));
}

static uint32_t
rtc_get_time()
{
	uint32_t time = RTC_TR & RTC_TR_RESERVED_MASK;

	while (time != (RTC_TR & RTC_TR_RESERVED_MASK)) {
		time = RTC_TR & RTC_TR_RESERVED_MASK;
	}	
	return time;
}

uint16_t
rtc_get_hourminute()
{
	uint8_t ht, hu, mt, mu;
	uint32_t time;

	time = rtc_get_time();

	ht = from_bcd((time >> RTC_TR_HT_SHIFT) & RTC_TR_HT_MASK);
	hu = from_bcd((time >> RTC_TR_HU_SHIFT) & RTC_TR_HU_MASK);
	mt = from_bcd((time >> RTC_TR_MNT_SHIFT) & RTC_TR_MNT_MASK);
	mu = from_bcd((time >> RTC_TR_MNU_SHIFT) & RTC_TR_MNU_MASK);

	return ht*1000 + hu*100 + mt*10 + mu;
}

void 
rtc_setup(void)
{
	/* turn on power block to enable unlocking */
	rcc_periph_clock_enable(RCC_PWR);
	pwr_disable_backup_domain_write_protect();

	/* Reset the RTC. */
	RCC_CSR |= RCC_CSR_RTCRST;
	RCC_CSR &= ~RCC_CSR_RTCRST;

	/* Turn on the LSE clock. */
	rcc_osc_on(RCC_LSE);
	rcc_wait_for_osc_ready(RCC_LSE);

	/*
	 * Use LSE as RTC clock. Nucleo L011K4 is equipped with 32768 Hz quartz
	 * connected to LSE oscillator input.
	 */
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

	rtc_wait_for_synchro();

//	rtc_interrupt_enable(RTC_SEC);

}

/*
void rtc_isr(void)
{
//	rtc_clear_flag(RTC_SEC);

	gpio_toggle(GPIOC, GPIO12);
	//usart_test();
}
*/



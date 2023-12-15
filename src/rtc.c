#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>

#include "rtc.h"

#define RTC_TR_RESERVED_MASK	(0x007F7F7FU)

static void 
rcc_rtc_select_clock(uint32_t clock)
{
	RCC_CSR &= ~(RCC_CSR_RTCSEL_MASK << RCC_CSR_RTCSEL_SHIFT);
	RCC_CSR |= (clock << RCC_CSR_RTCSEL_SHIFT);
}

/* Convert 8-bit BCD to binary value. */
static uint8_t 
from_bcd(uint8_t val)
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

static uint8_t
rtc_get_minute()
{
	uint8_t mt, mu;
	uint32_t time;

	time = rtc_get_time();

	mt = from_bcd((time >> RTC_TR_MNT_SHIFT) & RTC_TR_MNT_MASK);
	mu = from_bcd((time >> RTC_TR_MNU_SHIFT) & RTC_TR_MNU_MASK);

	return mt*10 + mu;
}

static uint8_t
rtc_get_hour()
{
	uint8_t ht, hu;
	uint32_t time;

	time = rtc_get_time();

	ht = from_bcd((time >> RTC_TR_HT_SHIFT) & RTC_TR_HT_MASK);
	hu = from_bcd((time >> RTC_TR_HU_SHIFT) & RTC_TR_HU_MASK);

	return ht*10 + hu;
}

static inline void
rtc_set_prep()
{
	rtc_unlock();
	rtc_enable_bypass_shadow_register();
	rtc_set_init_flag();
	rtc_wait_for_init_ready();
}

static inline void
rtc_set_finish()
{
	rtc_clear_init_flag();
	rtc_disable_bypass_shadow_register();
	rtc_lock();
}

void
rtc_increment_hour()
{
	uint8_t h;
	h = rtc_get_hour() + 1;

	if (h > 23)
		h = 0;

	rtc_set_prep();

	rtc_time_set_hour(h, true);

	rtc_set_finish();
}

void
rtc_increment_minute()
{
	uint8_t m;
	m = rtc_get_minute() + 1;

	if (m > 59)
		m = 0;

	rtc_set_prep();

	rtc_time_set_minute(m);

	rtc_set_finish();
}

struct bcd_time
rtc_get_bcd_hourminute(void)
{
	uint32_t time;
	struct bcd_time bt;

	time = rtc_get_time();

	bt.ht = (time >> RTC_TR_HT_SHIFT) & RTC_TR_HT_MASK;
	bt.hu = (time >> RTC_TR_HU_SHIFT) & RTC_TR_HU_MASK;
	bt.mt = (time >> RTC_TR_MNT_SHIFT) & RTC_TR_MNT_MASK;
	bt.mu = (time >> RTC_TR_MNU_SHIFT) & RTC_TR_MNU_MASK;

	return bt;
}

uint16_t
rtc_get_hourminute(void)
{
	/*
	uint8_t ht, hu, mt, mu;
	uint32_t time;

	time = rtc_get_time();

	ht = from_bcd((time >> RTC_TR_HT_SHIFT) & RTC_TR_HT_MASK);
	hu = from_bcd((time >> RTC_TR_HU_SHIFT) & RTC_TR_HU_MASK);
	mt = from_bcd((time >> RTC_TR_MNT_SHIFT) & RTC_TR_MNT_MASK);
	mu = from_bcd((time >> RTC_TR_MNU_SHIFT) & RTC_TR_MNU_MASK);

	return ht*1000 + hu*100 + mt*10 + mu;
	*/

	struct bcd_time bt;

	bt = rtc_get_bcd_hourminute();

	return bt.ht*1000 + bt.hu*100 + bt.mt*10 + bt.mu;
}

void 
rtc_setup(void)
{
	/* Turn on power block to enable unlocking. */
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

	rtc_set_init_flag();

	rtc_wait_for_init_ready();

	/* Set synch prescaler, using defaults for 1Hz out. */
	uint32_t sync = 255;
	uint32_t async = 127;
	rtc_set_prescaler(sync, async);

	/* Exit init mode, write protect and enable the clock.. */
	rtc_clear_init_flag();
	rtc_lock();
	RCC_CSR |= RCC_CSR_RTCEN;
	rtc_wait_for_synchro();

	/* If we ever need the interrupt... */
	//rtc_interrupt_enable(RTC_SEC);

}

/*
void rtc_isr(void)
{
	rtc_clear_flag(RTC_SEC);

	gpio_toggle(GPIOC, GPIO12);
	usart_test();
}
*/



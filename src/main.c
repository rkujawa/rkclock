#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#define USART_CONSOLE USART2

static int 
_write(int fd, char *ptr, int len) 
{
	int i = 0;
	/*
	 * Only work for STDOUT, STDIN, and STDERR
	 */
	if (fd > 2) {
		return -1;
	}
	while (*ptr && (i < len)) {
		usart_send_blocking(USART_CONSOLE, *ptr);
		if (*ptr == '\n') {
			usart_send_blocking(USART_CONSOLE, '\r');
		}
		i++;
		ptr++;
	}
	return i;
}

static void 
usart_setup(void)
{
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_GPIOA);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF4, GPIO2);

	/* Setup UART parameters. */
	usart_set_baudrate(USART_CONSOLE, 9600);
	usart_set_databits(USART_CONSOLE, 8);
	usart_set_stopbits(USART_CONSOLE, USART_STOPBITS_1);
	usart_set_mode(USART_CONSOLE, USART_MODE_TX);
	usart_set_parity(USART_CONSOLE, USART_PARITY_NONE);
	usart_set_flow_control(USART_CONSOLE, USART_FLOWCONTROL_NONE);

	usart_enable(USART_CONSOLE);
}

static void 
usart_test(void)
{
	const char *nigg = "test";

	_write(1, (char*) nigg, sizeof(nigg));
}

static void 
rcc_rtc_select_clock(uint32_t clock)
{
	RCC_CSR &= ~(RCC_CSR_RTCSEL_MASK << RCC_CSR_RTCSEL_SHIFT);
	RCC_CSR |= (clock << RCC_CSR_RTCSEL_SHIFT);
}

static int 
setup_rtc(void)
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
	RTC_ISR |= RTC_ISR_INIT;
	while ((RTC_ISR & RTC_ISR_INITF) == 0);

	/* set synch prescaler, using defaults for 1Hz out */
	uint32_t sync = 255;
	uint32_t async = 127;
	rtc_set_prescaler(sync, async);

	/* load time and date here if desired, and hour format */

	/* exit init mode */
	RTC_ISR &= ~(RTC_ISR_INIT);

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

	setup_rtc();

	usart_setup();
	while(1) {
		for (int i = 0; i < 50000; i++) {
			__asm__("nop");
		}
		gpio_toggle(GPIOB, GPIO3);
		usart_test();
	}
}


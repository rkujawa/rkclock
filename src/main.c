#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#define USART_CONSOLE USART2

const char nigg[] = "nigg";

static int _write(int fd, char *ptr, int len) {
	int i = 0;
	/*
	* Write "len" of char from "ptr" to file id "fd"
	* Return number of char written.
	*
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

static void usart_setup(void)
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

static void usart_test(void)
{
	_write(1, (char*) nigg, 3);
}

int main(void) 
{
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	gpio_set(GPIOB, GPIO3);

	usart_setup();
	while(1) {
		/* wait a little bit */
		for (int i = 0; i < 50000; i++) {
			__asm__("nop");
		}
		gpio_toggle(GPIOB, GPIO3);
		usart_test();
	}
}


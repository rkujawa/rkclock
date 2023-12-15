/* Based on https://github.com/szczys/stm32-cm3-helloworld/blob/master/helloworld/debounce-stm32-cm3.c */

#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/gpio.h>

#include <stdint.h>

#include "key.h"

static volatile uint32_t key_press;
static volatile uint32_t key_state;
static volatile uint32_t key_rpt;

/**
 * @brief Read debounced button presses.
 * @param key_mask Keymask corresponding to the pin for the button you with to poll.
 * @return Keymask where any high bits represent a button press.
 */
static uint32_t get_key_press(uint32_t key_mask);

void
key_setup(void)
{
	/* Set GPIO pins to input and enable pull-ups (keys are active low). */
	gpio_mode_setup(KEY_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY_H|KEY_M);
}

static uint32_t 
get_key_press(uint32_t key_mask)
{
	cm_disable_interrupts();  // read and clear atomic !
	key_mask &= key_press;	  // read key(s)
	key_press ^= key_mask;    // clear key(s)
	cm_enable_interrupts();
	return key_mask;
}

uint32_t 
get_key_short(uint32_t key_mask) 
{ 
	cm_disable_interrupts();  // read key state and key press atomic ! 
	return get_key_press(~key_state & key_mask); 
} 

void 
key_isr(void)
{ 
	static uint8_t ct0, ct1;
	uint8_t i;

	i = key_state ^ gpio_port_read(KEY_PORT);    // key changed ? (natural state is high so no need for ~KEY_PIN)
	ct0 = ~( ct0 & i );          // reset or count ct0
	ct1 = ct0 ^ (ct1 & i);       // reset or count ct1
	i &= ct0 & ct1;              // count until roll over ?
	key_state ^= i;              // then toggle debounced state
	key_press |= key_state & i;  // 0->1: key press detect
}

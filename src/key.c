/* Based on https://github.com/szczys/stm32-cm3-helloworld/blob/master/helloworld/debounce-stm32-cm3.c */

#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/gpio.h>

#include <stdint.h>

#include "key.h"

volatile uint32_t key_press;
volatile uint32_t key_state;
volatile uint32_t key_rpt;

/*--------------------------------------------------------------------------
  FUNC: 9/15/18 - Used to read debounced button presses
  PARAMS: A keymask corresponding to the pin for the button you with to poll
  RETURNS: A keymask where any high bits represent a button press
--------------------------------------------------------------------------*/
uint32_t get_key_press( uint32_t key_mask )
{
	cm_disable_interrupts();  // read and clear atomic !
	key_mask &= key_press;	  // read key(s)
	key_press ^= key_mask;    // clear key(s)
	cm_enable_interrupts();
	return key_mask;
}

/*--------------------------------------------------------------------------
  FUNC: 9/15/18 - Used to check for debounced buttons that are held down
  PARAMS: A keymask corresponding to the pin for the button you with to poll
  RETURNS: A keymask where any high bits is a button held long enough for
		its input to be repeated
--------------------------------------------------------------------------*/
/*
uint32_t get_key_rpt( uint32_t key_mask ) 
{ 
	cm_disable_interrupts();  // read and clear atomic ! 
	key_mask &= key_rpt;      // read key(s) 
	key_rpt ^= key_mask;      // clear key(s) 
	cm_enable_interrupts();
	return key_mask; 
} 
*/

/*--------------------------------------------------------------------------
  FUNC: 9/15/18 - Used to read debounced button released after a short press
  PARAMS: A keymask corresponding to the pin for the button you with to poll
  RETURNS: A keymask where any high bits represent a quick press and release
--------------------------------------------------------------------------*/
uint32_t get_key_short( uint32_t key_mask ) 
{ 
	cm_disable_interrupts();  // read key state and key press atomic ! 
	return get_key_press( ~key_state & key_mask ); 
} 

/*--------------------------------------------------------------------------
  FUNC: 9/15/18 - Used to read debounced button held for REPEAT_START amount
	of time.
  PARAMS: A keymask corresponding to the pin for the button you with to poll
  RETURNS: A keymask where any high bits represent a long button press
--------------------------------------------------------------------------*/
/*
uint32_t 
get_key_long(uint32_t key_mask) 
{ 
	return get_key_press(get_key_rpt(key_mask)); 
}
*/
/*--------------------------------------------------------------------------
  FUNC: 9/15/18 - Used to debounce buttons in an interrupt service routine
	that runs once every 10 milliseconds
  PARAMS: None
  RETURNS: None
--------------------------------------------------------------------------*/
void 
key_isr(void)
{ 
	static uint8_t ct0, ct1; //, rpt;  
	uint8_t i;

	i = key_state ^ gpio_port_read(KEY_PORT);    // key changed ? (natural state is high so no need for ~KEY_PIN)
	ct0 = ~( ct0 & i );          // reset or count ct0
	ct1 = ct0 ^ (ct1 & i);       // reset or count ct1
	i &= ct0 & ct1;              // count until roll over ?
	key_state ^= i;              // then toggle debounced state
	key_press |= key_state & i;  // 0->1: key press detect

/*
  if((key_state & REPEAT_MASK) == 0)   // check repeat function 
     rpt = REPEAT_START;      // start delay 
  if(--rpt == 0){ 
    rpt = REPEAT_NEXT;         // repeat delay 
    key_rpt |= key_state & REPEAT_MASK; 
	}
*/
}

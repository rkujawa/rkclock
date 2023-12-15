#ifndef _KEY_H_ 
#define _KEY_H_ 

#define KEY_H             GPIO6
#define KEY_M             GPIO7
#define KEY_PORT          GPIOA
#define KEY_CLOCK         RCC_GPIOA
/* Add keys on the same port to keymask: (KEY0 | KEY1 | KEY2) */
#define KEY_MASK          (KEY_H|KEY_M)

/**
 * @brief Read debounced button released after a short press.
 * @param key_mask A keymask corresponding to the pin for the button you with to poll.
 * @return Keymask where any high bits represent a quick press and release
 */
uint32_t get_key_short(uint32_t key_mask); 

/**
 * @brief Debounce buttons in an interrupt service routine.
 */
void key_isr(void);

void key_setup(void);

#endif

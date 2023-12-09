#ifndef _KEY_H_ 
#define _KEY_H_ 

#define KEY_H             GPIO6
#define KEY_M             GPIO7
#define KEY_PORT          GPIOA
#define KEY_CLOCK         RCC_GPIOA
//Add keys on the same port to keymask: (KEY0 | KEY1 | KEY2)
#define KEY_MASK          (KEY_H|KEY_M)

/*********************** Debounce ****************************
* Info on debounce code:
* https://github.com/szczys/Button-Debounce/blob/master/debounce-test.c 
*/
//Debounce
#define REPEAT_MASK   (KEY_H|KEY_M)   // repeat: key1, key2 
#define REPEAT_START  50       // after 500ms 
#define REPEAT_NEXT   70       // every 700ms
/************************************************************/

uint32_t get_key_press( uint32_t key_mask );
/*uint32_t get_key_rpt( uint32_t key_mask );*/
uint32_t get_key_short( uint32_t key_mask ); 
/*uint32_t get_key_long( uint32_t key_mask );*/
void key_isr( void );

#endif

#ifndef _BCD_H_ 
#define _BCD_H_ 

/**
 * @brief BCD time representation.
 */
struct bcd_time {
	uint8_t ht;	/**< Hour tens. */
	uint8_t hu;	/**< Hour units. */
	uint8_t mt;	/**< Minute tens. */
	uint8_t mu;	/**< Minute units. */
};

/*
 * Convert 8-bit BCD to binary value. 
 */
/*uint8_t from_bcd(uint8_t v);*/
/*
 * Convert 8-bit binary to BCD value.
 */
/*uint8_t to_bcd(uint8_t v);*/
/**
 * @brief Perform BCD addition.
 */
uint8_t bcd_add(uint8_t x, uint8_t y);

#endif

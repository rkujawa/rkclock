#ifndef _RTC_H_ 
#define _RTC_H_ 

/**
 * @brief BCD time representation.
 */
struct bcd_time {
	uint8_t ht;	/**< Hour tens. */
	uint8_t hu;	/**< Hour units. */
	uint8_t mt;	/**< Minute tens. */
	uint8_t mu;	/**< Minute units. */
};

void rtc_setup(void);
/**
 * @brief Get a single 16-bit value representing current hour and minute i.e. 16:32 = 1632.
 */
uint16_t rtc_get_hourminute();
/**
 * @brief Increment time in RTC by one hour.
 */
void rtc_increment_hour();
/**
 * @brief Increment time in RTC byt one minute.
 */
void rtc_increment_minute();

#endif

#ifndef _RTC_H_ 
#define _RTC_H_ 

#include "bcd.h"

/**
 * @brief Configure the RTC hardware for operation.
 */
void rtc_setup(void);
/**
 * @brief Get current hour and minute encoded as structure of BCD nibbles.
 */
struct bcd_time rtc_get_hourminute_bcd(void);
/**
 * @brief Increment time in RTC by one hour.
 */
void rtc_increment_hour();
/**
 * @brief Increment time in RTC byt one minute.
 */
void rtc_increment_minute();

#endif

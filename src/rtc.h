#ifndef _RTC_H_ 
#define _RTC_H_ 

void rtc_setup(void);
uint16_t rtc_get_hourminute();

void rtc_increment_hour();
void rtc_increment_minute();

#endif

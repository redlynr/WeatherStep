#ifndef __TIMEBOX_WEATHER_
#define __TIMEBOX_WEATHER_

void update_weather();
void update_weather_values(int temp_val, int max_val, int min_val, int weather_val);
//void store_weather_values(int temp_val, int max_val, int min_val, int weather_val, char forecast_val[], char stocks_val[]);   // KAH 2/26/2016
void store_weather_values(int temp_val, int max_val, int min_val, int weather_val);   // KAH 2/26/2016
void toggle_weather();
bool is_weather_enabled();

void store_forecast_value(char forecast_val[]);
void store_stocks_value(char stocks_val[]);
void store_rss_value(char rss_val[]);
void store_rss_value2(char rss_val[]);
void store_rss_value3(char rss_val[]);
#endif

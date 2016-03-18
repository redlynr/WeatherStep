#include <pebble.h>
#include "keys.h"
#include "text.h"

static bool weather_enabled;
static bool use_celsius; // KAH 2/26/2016
static int weather_pins;
static char weather_key_buffer[20]; // KAH 2/26/2016
static bool health_enabled;

static char* weather_conditions[] = {
    "\U0000F07B", // 'unknown': 0,
    "\U0000F00D", // 'clear': 1,
    "\U0000F00D", // 'sunny': 2,
    "\U0000F002", // 'partlycloudy': 3,
    "\U0000F002", // 'mostlycloudy': 4,
    "\U0000F00C", // 'mostlysunny': 5,
    "\U0000F002", // 'partlysunny': 6,
    "\U0000F013", // 'cloudy': 7,
    "\U0000F019", // 'rain': 8,
    "\U0000F01B", // 'snow': 9,
    "\U0000F01D", // 'tstorms': 10,
    "\U0000F0b5", // 'sleat': 11,
    "\U0000F00A", // 'flurries': 12,
    "\U0000F0b6", // 'hazy': 13,
    "\U0000F01D", // 'chancetstorms': 14,
    "\U0000F01B", // 'chancesnow': 15,
    "\U0000F0b5", // 'chancesleat': 16,
    "\U0000F008", // 'chancerain': 17,
    "\U0000F01B", // 'chanceflurries': 18,
    "\U0000F07B", // 'nt_unknown': 19,
    "\U0000F02E", // 'nt_clear': 20,
    "\U0000F02E", // 'nt_sunny': 21,
    "\U0000F086", // 'nt_partlycloudy': 22,
    "\U0000F086", // 'nt_mostlycloudy': 23,
    "\U0000F081", // 'nt_mostlysunny': 24,
    "\U0000F086", // 'nt_partlysunny': 25,
    "\U0000F013", // 'nt_cloudy': 26,
    "\U0000F019", // 'nt_rain': 27,
    "\U0000F01B", // 'nt_snow': 28,
    "\U0000F01D", // 'nt_tstorms': 29,
    "\U0000F0b5", // 'nt_sleat': 30,
    "\U0000F038", // 'nt_flurries': 31,
    "\U0000F04A", // 'nt_hazy': 32,
    "\U0000F01D", // 'nt_chancetstorms': 33,
    "\U0000F038", // 'nt_chancesnow': 34,
    "\U0000F0B3", // 'nt_chancesleat': 35,
    "\U0000F036", // 'nt_chancerain': 36,
    "\U0000F038", // 'nt_chanceflurries': 37,
    "\U0000F003", // 'fog': 38,
    "\U0000F04A", // 'nt_fog': 39,
};


/*  KAH 2/26/2016 
void update_weather(void) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    char weather_key_buffer[20];
    if (persist_exists(KEY_WEATHERKEY)) {
        persist_read_string(KEY_WEATHERKEY, weather_key_buffer, sizeof(weather_key_buffer));
    } else {
        weather_key_buffer[0] = '\0';
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting weather with key (%s) %d", weather_key_buffer, (int)time(NULL));
    dict_write_uint8(iter, KEY_USECELSIUS, 
        persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS) ? persist_read_int(KEY_USECELSIUS) : 0);
    dict_write_cstring(iter, KEY_WEATHERKEY, weather_key_buffer);
    app_message_outbox_send();
}
*/


// KAH 2/26/2016
void update_weather(void) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

  //  APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting weather with key (%s) %d", weather_key_buffer, (int)time(NULL));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting weather. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL)); 
    dict_write_uint8(iter, KEY_USECELSIUS, use_celsius);
    dict_write_uint8(iter, KEY_WEATHERPINS, weather_pins);
APP_LOG(APP_LOG_LEVEL_DEBUG, "weather.c Weather2 Pin %d", weather_pins);   
 //   dict_write_cstring(iter, KEY_WEATHERKEY, weather_key_buffer);
    app_message_outbox_send();
}



void update_weather_values(int temp_val, int max_val, int min_val, int weather_val) {
    //bool useCelsius = persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS); // KAH 2/26/2016
    char temp_pattern[4];
    char temp_text[8];
    char max_text[8];
    char min_text[8];
    char weather_text[4];
    char steps_text[10]; // if health not enabled, put hi/lo in it's spot

    //if (get_loaded_font() == BLOCKO_BIG_FONT) {
    //    strcpy(temp_pattern, useCelsius ? "%dc" : "%df");
    //} else if (get_loaded_font() == BLOCKO_FONT) {
    //    strcpy(temp_pattern, useCelsius ? "%dc" : "%df");
    //} else {
        strcpy(temp_pattern, use_celsius ? "%d°" : "%d°");
    //}
     
    snprintf(temp_text, sizeof(temp_text), temp_pattern, temp_val);
    snprintf(max_text, sizeof(max_text), "%d", max_val);
    snprintf(min_text, sizeof(min_text), "%d", min_val);
  
    health_enabled = persist_read_int(KEY_ENABLEHEALTH);
    if (!health_enabled) {
      snprintf(steps_text,sizeof(steps_text), "%d°/%d°", max_val,min_val);
    }
  
  // replace icons that don't look nice
    if (weather_val == 0) {  // unknown
      weather_val = 7; // cloudy
    }
    if (weather_val == 38) {  // foggy - for some reason, looks distorted with 34 size
      weather_val = 13; // hazy
    }
   

  
    snprintf(weather_text, sizeof(weather_text), "%s", weather_conditions[weather_val]);
    
    //KAH 2/26/2016
    //persist_write_int(KEY_TEMP, temp_val);
    //persist_write_int(KEY_MAX, max_val);
    //persist_write_int(KEY_MIN, min_val);
    //persist_write_int(KEY_WEATHER, weather_val);

    set_temp_cur_layer_text(temp_text);
    set_temp_max_layer_text(max_text);
    set_temp_min_layer_text(min_text);
    set_weather_layer_text(weather_text);
  
    
    set_steps_or_sleep_layer_text(steps_text);
  
    set_max_icon_layer_text("\U0000F058");
    set_min_icon_layer_text("\U0000F044");
}

void toggle_weather(bool from_configs) {
    weather_enabled = persist_exists(KEY_ENABLEWEATHER) && persist_read_int(KEY_ENABLEWEATHER);
    if (weather_enabled) {
      
        // KAH 2/26/2016
        if (persist_exists(KEY_WEATHERKEY)) {
            persist_read_string(KEY_WEATHERKEY, weather_key_buffer, sizeof(weather_key_buffer));
        } else {
            weather_key_buffer[0] = '\0';
        }
        use_celsius = persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS);
        weather_pins = persist_exists(KEY_WEATHERPINS) && persist_read_int(KEY_WEATHERPINS);
APP_LOG(APP_LOG_LEVEL_DEBUG, "weather.c Weather1 Pin %d", weather_pins); 

      
      
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather is enabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        //update_weather_values(0, 0, 0, 0);  // KAH 2/26/2016
        if (from_configs) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating weather from configs. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_weather_values(0, 0, 0, 0);  // KAH 2/26/2016
            update_weather();
          
// KAH 2/26/2016 
        } else if (persist_exists(KEY_TEMP)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating weather from storage. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            int temp = persist_read_int(KEY_TEMP);
            int min = persist_read_int(KEY_MIN);
            int max = persist_read_int(KEY_MAX);
            int weather = persist_read_int(KEY_WEATHER);

            update_weather_values(temp, max, min, weather);
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "No weather data from storage. Requesting... %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_weather_values(0, 0, 0, 0);
            update_weather();
        }
// KAH 2/26/2016      
      
      
      
    } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather disabled, clearing up. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        set_temp_cur_layer_text("");
        set_temp_max_layer_text("");
        set_temp_min_layer_text("");
        set_weather_layer_text("");
        set_max_icon_layer_text("");
        set_min_icon_layer_text("");
    }
}

// KAH 2/26/2016
void store_weather_values(int temp, int max, int min, int weather) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Storing weather data. %d %d%d", use_celsius, (int)time(NULL), (int)time_ms(NULL, NULL));
    persist_write_int(KEY_TEMP, temp);
    persist_write_int(KEY_MAX, max);
    persist_write_int(KEY_MIN, min);
    persist_write_int(KEY_WEATHER, weather);
}



bool is_weather_enabled() {
    return weather_enabled;
}

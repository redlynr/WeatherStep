#include <pebble.h>
#include <ctype.h>
#include <time.h>
#include "keys.h"
#include "locales.h"
#include "health.h"
#include "text.h"
#include "weather.h"

#if (PBL_PLATFORM_APLITE)
//static char stocks_v[300];
static char stocksList_v[50];
//static char rss_v[250];
static char speedMultiplier[5];
static char speedMultiplier_v[5];
static char ticker_buffer[200];
#else
//static char stocks_v[250];
//static char rss_v[250];
static char stocksList_v[100];
static char speedMultiplier[10];
static char speedMultiplier_v[10];
static char ticker_buffer[200];
#endif

static char temp_buffer[200];
static int rss_counter = 1;

//static char speedMultiplier[5];
//static char speedMultiplier_v[5];
//static char ticker_buffer[500];

static Layer *s_battery_layer;
static int s_battery_level, parent_bounds;
static bool charge = 0;

static Window *watchface;

static signed int tz_hour;
static uint8_t tz_minute;
static char tz_name[TZ_LEN];
//animates layer by number of pixels
static PropertyAnimation *s_box_animation;
static int shakeOption;
static int shakeOption1;
static int shakeOption2;
static int shakeOption3;

bool first_animation_is_running = false;
bool second_animation_is_running = false;
bool third_animation_is_running = false;

//char forecast_val[1000];
//char stocks_val[500];




void create_ticker(){
   APP_LOG(APP_LOG_LEVEL_DEBUG, "before creating ticker layer");
    ticker_text = text_layer_create(GRect(width, date_top, 5000, 40));
    text_layer_set_background_color(ticker_text, GColorClear);
    //text_layer_set_overflow_mode(ticker_text,GTextOverflowModeWordWrap);
    text_layer_set_text_alignment(ticker_text, PBL_IF_ROUND_ELSE(GTextAlignmentLeft, GTextAlignmentLeft));
    layer_set_hidden(text_layer_get_layer(ticker_text),true);
    text_layer_set_font(ticker_text,base_font);
    bool enableAdvanced = persist_exists(KEY_ENABLEADVANCED) ? persist_read_int(KEY_ENABLEADVANCED) : false;
    text_layer_set_text_color(ticker_text, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_DATECOLOR)) : base_color);
            
  APP_LOG(APP_LOG_LEVEL_DEBUG, "after creating ticker layer");
  Layer *window_layer = window_get_root_layer(watchface);
  layer_add_child(window_layer, text_layer_get_layer(ticker_text));
}

void destroy_ticker(){
  text_layer_destroy(ticker_text);
}


void anim_stopped_handler(Animation *animation, bool finished, void *context) {
	
   // Schedule the reverse animation, unless the app is exiting
  if (finished) {
    
   APP_LOG(APP_LOG_LEVEL_DEBUG, "animation is finshed");  
   if (first_animation_is_running){ 
    first_animation_is_running = false;
   } else if (second_animation_is_running) {
     second_animation_is_running = false;
   } 
    else if (third_animation_is_running) {
     third_animation_is_running = false;
   }
    
  set_ticker(" ");
  hide_ticker(shakeOption);
  destroy_ticker();  
  property_animation_destroy(s_box_animation);

   APP_LOG(APP_LOG_LEVEL_DEBUG, "after destroying property animation %d free",(int)heap_bytes_free());  
  }
}


float stof(const char* s){
  float rez = 0, fact = 1;
  if (*s == '-'){
    s++;
    fact = -1;
  };
  for (int point_seen = 0; *s; s++){
    if (*s == '.'){
      point_seen = 1; 
      continue;
    };
    int d = *s - '0';
    if (d >= 0 && d <= 9){
      if (point_seen) fact /= 10.0f;
      rez = rez * 10.0f + (float)d;
    };
  };
  return rez * fact;
};

void run_animation(){
  // Play the Animation
  
  float speed;
  float speed_adj;
  int anim_delay;
  
 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Let's run the animation");

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Shake Action %d",  (int)persist_read_int(KEY_SHAKEACTION));
 create_ticker();
 set_ticker("initializeed"); 
 



  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Shake Action %d",  shakeOption);
  
  if (shakeOption > 1) {  
       
    if (persist_exists(KEY_STOCKS)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating stocks from storage. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            persist_read_string(KEY_STOCKS, ticker_buffer, sizeof(ticker_buffer));
            APP_LOG(APP_LOG_LEVEL_DEBUG, "size of ticker_buffer. %d",(int)sizeof(ticker_buffer));
            if(sizeof(ticker_buffer)>0){
              set_ticker(ticker_buffer);
            }
    } else {
    set_ticker("Unfortunately, no stocks :(");
    }
  }
  
  if (shakeOption == 1) { 
      if (persist_exists(KEY_FORECAST)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating forecast from storage. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
          
            persist_read_string(KEY_FORECAST, ticker_buffer, sizeof(ticker_buffer));
            APP_LOG(APP_LOG_LEVEL_DEBUG, "size of ticker_buffer. %d",(int)sizeof(ticker_buffer));
            if(sizeof(ticker_buffer)>0){
              set_ticker(ticker_buffer);
              }
      } else {
          set_ticker("Unfortunately, no forecast :(");
      } 
  }
 
  
  if (shakeOption == 4) { 
    
    if (rss_counter == 1){
    
      if (persist_exists(KEY_RSS)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating rss1 from storage. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
          
            persist_read_string(KEY_RSS, ticker_buffer, sizeof(ticker_buffer));
            APP_LOG(APP_LOG_LEVEL_DEBUG, "rss1 %s",ticker_buffer);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "size of ticker_buffer. %d",(int)sizeof(ticker_buffer));
            if(sizeof(ticker_buffer)>0){
              set_ticker(ticker_buffer);
              }
      } else {
          set_ticker("Unfortunately, no rss :(");
      } 
    } else if (rss_counter == 2){
      if (persist_exists(KEY_RSS2)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating rss2 from storage. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
          
            persist_read_string(KEY_RSS2, ticker_buffer, sizeof(ticker_buffer));
            APP_LOG(APP_LOG_LEVEL_DEBUG, "rss2 %s",ticker_buffer);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "size of ticker_buffer. %d",(int)sizeof(ticker_buffer));
            if(sizeof(ticker_buffer)>0){
              set_ticker(ticker_buffer);
              }
      } else {
          set_ticker("Unfortunately, no 2nd rss :(");
      } 
    } else if (rss_counter == 3){
      if (persist_exists(KEY_RSS3)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating rss3 from storage. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
          
            persist_read_string(KEY_RSS3, ticker_buffer, sizeof(ticker_buffer));
            APP_LOG(APP_LOG_LEVEL_DEBUG, "rss3 %s",ticker_buffer);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "size of ticker_buffer. %d",(int)sizeof(ticker_buffer));
            if(sizeof(ticker_buffer)>0){
              set_ticker(ticker_buffer);
              }
      } else {
          set_ticker("Unfortunately, no 3rd rss :(");
      } 
    }
  
        
        
        
        
  rss_counter = rss_counter + 1;
  if (rss_counter > 3){
    rss_counter = 1;
  }
  
  
  }
  
  if (shakeOption > 0){
    display_ticker(shakeOption);
  }
 
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ticker pixels %d ",ticker_pixels);
  
  GRect start_frame = GRect(width, date_top, ticker_pixels, 40);
  GRect finish_frame = GRect(-ticker_pixels, date_top, 5000, 40);
 
  
  
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Before creating property animation %d free",(int)heap_bytes_free());
    
  speed_adj = 1.0;
  speed = 10000.0;   
  // "normalization"
  speed = speed * (ticker_pixels / 500.0);  
  
  
  if (persist_exists(KEY_SPEEDMULTIPLIER)){
    persist_read_string(KEY_SPEEDMULTIPLIER, speedMultiplier, sizeof(speedMultiplier));
    speed_adj = stof(speedMultiplier);
  }
  
  
  if (speed_adj > 0){
    speed = speed / speed_adj;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "speed  %d ",(int)speed);
  
  /*
  if (shakeOption ==0){
    finish_frame = start_frame;
    anim_delay = 2000;
    speed = 0;
  } else{
    anim_delay = 0;
  }
  */
  anim_delay = 0;
  
  s_box_animation = property_animation_create_layer_frame(text_layer_get_layer(ticker_text), &start_frame, &finish_frame);
  animation_set_handlers((Animation*)s_box_animation, (AnimationHandlers) {
    .stopped = anim_stopped_handler
  }, NULL);
  animation_set_duration((Animation*)s_box_animation, (int)speed );
  animation_set_curve((Animation*)s_box_animation, AnimationCurveLinear);
  animation_set_delay((Animation*)s_box_animation, anim_delay);

  animation_schedule((Animation*)s_box_animation);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "after creating property animation %d free",(int)heap_bytes_free());
  
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Animation has been scheduled");

 
 
}



// KAH 3/21/2016
// when watch is shaken or tapped
static void accel_tap_handler(AccelAxisType axis, int32_t direction) {   
	
if (persist_exists(KEY_SHAKEACTION)){ 
    shakeOption1 = (int)persist_read_int(KEY_SHAKEACTION);
    if (shakeOption1 >= 48){
      shakeOption1 = shakeOption1 - 48;
    }

}  
if (persist_exists(KEY_SHAKEACTION2)){ 
    shakeOption2 = (int)persist_read_int(KEY_SHAKEACTION2);
    if (shakeOption2 >= 48){
      shakeOption2 = shakeOption2 - 48;
    }

}
 
if (persist_exists(KEY_SHAKEACTION3)){ 
    shakeOption3 = (int)persist_read_int(KEY_SHAKEACTION3);
    if (shakeOption3 >= 48){
      shakeOption3 = shakeOption3 - 48;
    }

}
  
  
APP_LOG(APP_LOG_LEVEL_DEBUG, "shakeOption1 %d",shakeOption1); 
APP_LOG(APP_LOG_LEVEL_DEBUG, "shakeOption2 %d",shakeOption2); 
APP_LOG(APP_LOG_LEVEL_DEBUG, "shakeOption3 %d",shakeOption3); 
  

  
  // Play the animation
  
  if (!first_animation_is_running && !second_animation_is_running && !third_animation_is_running ) {
   shakeOption = shakeOption1;
   first_animation_is_running = true;
   run_animation();
  } else if (first_animation_is_running && !second_animation_is_running) {
      //if (shakeOption2 > 0){
        set_ticker(" ");
        hide_ticker(shakeOption);
        destroy_ticker();  
        property_animation_destroy(s_box_animation);
        shakeOption = shakeOption2;
         APP_LOG(APP_LOG_LEVEL_DEBUG, "time to run 2nd anim"); 
        second_animation_is_running = true;
        first_animation_is_running = false;
        run_animation();
      //}
  }else if (second_animation_is_running && !third_animation_is_running) {
     //if (shakeOption3 > 0){
        set_ticker(" ");
        hide_ticker(shakeOption);
        destroy_ticker();  
        property_animation_destroy(s_box_animation);
        shakeOption = shakeOption3;
       APP_LOG(APP_LOG_LEVEL_DEBUG, "time to run 3rd anim"); 
        second_animation_is_running = false;
        third_animation_is_running = true;
        run_animation();  
        second_animation_is_running = false;
     //} 

  } else if(third_animation_is_running) {
    hide_ticker(shakeOption);
    destroy_ticker();  
    property_animation_destroy(s_box_animation);
    third_animation_is_running = false;
  }
  

}

static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    struct tm *gmt_time = gmtime(&temp);
    gmt_time->tm_hour = gmt_time->tm_hour + tz_hour;
    mktime(gmt_time);
    gmt_time->tm_min = gmt_time->tm_min + tz_minute;
    mktime(gmt_time);
    char tz_text[22];
    char hour_text[13];
    char date_text[13];


    // Write the current hours and minutes into a buffer
    strftime(hour_text, sizeof(hour_text), (clock_is_24h_style() ? "%H:%M" : "%I:%M"), tick_time);
  
    // remove leading zero on hour
    if (persist_exists(KEY_NOLEADINGZERO)){
     if (!clock_is_24h_style() && persist_read_int(KEY_NOLEADINGZERO)) {
      if (hour_text[0] == '0') {
        hour_text[0] = ' ';
 //        for (unsigned char i = 1; hour_text[i]; ++i) {
 //          hour_text2[i-1]=hour_text[i];
 //        }
 //        strcpy(hour_text,hour_text2);
      }
     }
    }  
      
    if (tz_name[0] != '#') {
        strftime(tz_text, sizeof(tz_text), (clock_is_24h_style() ? "%H:%M" : "%I:%M%p"), gmt_time);

        if ((gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon == tick_time->tm_mon && gmt_time->tm_mday > tick_time->tm_mday) ||
            (gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon > tick_time->tm_mon) ||
            (gmt_time->tm_year > tick_time->tm_year)
        ) {
                strcat(tz_text, "+1");
        } else if ((gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon == tick_time->tm_mon && gmt_time->tm_mday < tick_time->tm_mday) ||
            (gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon < tick_time->tm_mon) ||
            (gmt_time->tm_year < tick_time->tm_year)
        ) {
            strcat(tz_text, "-1");
        }

        strcat(tz_text, ".");
        strcat(tz_text, tz_name); 

        for (unsigned char i = 0; tz_text[i]; ++i) {
            if (get_loaded_font() == BLOCKO_FONT || get_loaded_font() == BLOCKO_BIG_FONT) {
                tz_text[i] = tolower((unsigned char)tz_text[i]);
            } else {
                tz_text[i] = toupper((unsigned char)tz_text[i]);
            }
        }
        set_alt_time_layer_text(tz_text);
    } else {
        set_alt_time_layer_text("");
    }

    set_hours_layer_text(hour_text);
    get_current_date(tick_time, date_text, sizeof(date_text));
    set_date_layer_text(date_text);
}

void bt_handler(bool connected) {
    if (connected) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Phone is connected.");
        set_bluetooth_layer_text("");
    } else {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Phone is not connected.");
        bool is_sleeping = is_user_sleeping();
        bool bluetooth_disconnect_vibe = persist_exists(KEY_BLUETOOTHDISCONNECT) && persist_read_int(KEY_BLUETOOTHDISCONNECT);

        if (bluetooth_disconnect_vibe && !is_sleeping) {
            vibes_long_pulse();
        }
        set_bluetooth_layer_text("\U0000F294");
    }
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
   
 GRect bounds = layer_get_bounds(layer);
  
  // Find the width of the bar
  int width = (int)(float)(((float)s_battery_level / 100.0F) * parent_bounds);
  
  // Draw the background
  //graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_context_set_fill_color(ctx, GColorFromHEX(persist_read_int(KEY_BATTERYBACKCOLOR)));
  graphics_fill_rect(ctx, bounds, GCornerNone, 0);
  
  // Draw the battery bar  
  
  graphics_context_set_fill_color(ctx, GColorFromHEX(persist_read_int(KEY_BATTERYCOLOR)));
     
  
  if (s_battery_level <= 20 && charge == 0)  {
      graphics_context_set_fill_color(ctx, GColorRed);      
  }  
  
   if (charge == 1)  {
       graphics_context_set_fill_color(ctx, GColorIcterine);    
  }  
   graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), GCornerNone, 0);
 
}


static void battery_handler(BatteryChargeState charge_state) {
    char s_battery_buffer[8];

    if (charge_state.is_charging) {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "(=/=)");
    } else {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), (charge_state.charge_percent <= 20 ? "! %d%%" : "%d%%"), charge_state.charge_percent);
    }

    set_battery_layer_text(s_battery_buffer);
  
  
    s_battery_level = charge_state.charge_percent;  
  
    if (charge_state.is_plugged) {
      charge = 1;
    } else {
      charge = 0;
    }  
    // Update meter
    layer_mark_dirty(s_battery_layer);
  
  
}

static void load_screen(bool from_configs) {
    if (from_configs) {
        unload_face_fonts();
    }
    load_face_fonts();
    set_face_fonts();
    set_colors(watchface);
    load_locale();
    update_time();
    toggle_health(from_configs);
    toggle_weather(from_configs);
    battery_handler(battery_state_service_peek());
    bt_handler(connection_service_peek_pebble_app_connection());
}

/*
static void check_for_updates() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Checking for updates. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, KEY_HASUPDATE, 1); 
    app_message_outbox_send();
}
*/

static void notify_update(int update_available) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Notifying user. (%d) %d%d", update_available, (int)time(NULL), (int)time_ms(NULL, NULL));
    set_update_layer_text(update_available ? "\U0000F102" : "");
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple *error_tuple = dict_find(iterator, KEY_ERROR);
APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox received callback 1");
    if (error_tuple) {
        get_health_data();
       APP_LOG(APP_LOG_LEVEL_DEBUG, "returning after error_tuple");
        return;
    }

    Tuple *update_tuple = dict_find(iterator, KEY_HASUPDATE);
    if (update_tuple) {
        int update_val = update_tuple->value->int8;
        persist_write_int(KEY_HASUPDATE, update_val);
        notify_update(update_val);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "returning after update_tuple");
        return;
    }

 Tuple *rss_tuple = dict_find(iterator, KEY_RSS);
    if (rss_tuple) {
        strncpy(temp_buffer, rss_tuple->value->cstring,200);   
        store_rss_value(temp_buffer);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "rss %s ",temp_buffer);  
        APP_LOG(APP_LOG_LEVEL_DEBUG, "returning after rss_tuple");
        return;
    } 
  
  Tuple *rss_tuple2 = dict_find(iterator, KEY_RSS2);
    if (rss_tuple2) {
        strncpy(temp_buffer, rss_tuple2->value->cstring,200);   
        store_rss_value2(temp_buffer);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "rss %s ",temp_buffer);  
        APP_LOG(APP_LOG_LEVEL_DEBUG, "returning after rss_tuple");
        return;
    } 
  
  Tuple *rss_tuple3 = dict_find(iterator, KEY_RSS3);
    if (rss_tuple3) {
        strncpy(temp_buffer, rss_tuple3->value->cstring,200);   
        store_rss_value3(temp_buffer);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "rss %s ",temp_buffer);  
        APP_LOG(APP_LOG_LEVEL_DEBUG, "returning after rss_tuple");
        return;
    } 
 
  Tuple *stocks_tuple = dict_find(iterator, KEY_STOCKS);
    if (stocks_tuple) {
        strncpy(temp_buffer, stocks_tuple->value->cstring,200);      
        store_stocks_value(temp_buffer);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "stocks %s ",temp_buffer); 
        APP_LOG(APP_LOG_LEVEL_DEBUG, "returning after stocks_tuple");
        return;
    } 
  
          
  
  
  
 APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox received callback 2");   
    
    Tuple *temp_tuple = dict_find(iterator, KEY_TEMP);
    Tuple *max_tuple = dict_find(iterator, KEY_MAX);
    Tuple *min_tuple = dict_find(iterator, KEY_MIN);
    Tuple *weather_tuple = dict_find(iterator, KEY_WEATHER);
    Tuple *forecast_tuple = dict_find(iterator, KEY_FORECAST);
//    Tuple *stocks_tuple = dict_find(iterator, KEY_STOCKS);
//    Tuple *rss_tuple = dict_find(iterator, KEY_RSS);
APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox received callback 3");  
    if (forecast_tuple && temp_tuple && max_tuple && min_tuple && weather_tuple && is_weather_enabled()) {
APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox received callback 4");      
        int temp_val = (int)temp_tuple->value->int32;
        int max_val = (int)max_tuple->value->int32;
        int min_val = (int)min_tuple->value->int32;
        int weather_val = (int)weather_tuple->value->int32;
//        strcpy(forecast_val, forecast_tuple->value->cstring);
//        strcpy(stocks_val, stocks_tuple->value->cstring);
      
        //*forecast_val = forecast_tuple->value->cstring;
        //*stocks_val = stocks_tuple->value->cstring; 
APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox received callback 5");        
        update_weather_values(temp_val, max_val, min_val, weather_val);
      
 APP_LOG(APP_LOG_LEVEL_DEBUG, "before storing weather");     
// APP_LOG(APP_LOG_LEVEL_DEBUG, "stocks_key_buffer %s ",stocks_key_buffer);   
//        store_weather_values(temp_val, max_val, min_val, weather_val, forecast_val, stocks_val); // KAH 2/26/2016
//        store_weather_values(temp_val, max_val, min_val, weather_val, forecast_key_buffer, stocks_key_buffer);
        store_weather_values(temp_val, max_val, min_val, weather_val);

        strncpy(temp_buffer, forecast_tuple->value->cstring,200);
        store_forecast_value(temp_buffer);
       APP_LOG(APP_LOG_LEVEL_DEBUG, "forecast %s ",temp_buffer);
        ////strncpy(temp_buffer, stocks_tuple->value->cstring,200);      
       // store_stocks_value(temp_buffer);
       //APP_LOG(APP_LOG_LEVEL_DEBUG, "stocks %s ",temp_buffer);
        //strncpy(temp_buffer, rss_tuple->value->cstring,200);   
        //store_rss_value(temp_buffer);
       //APP_LOG(APP_LOG_LEVEL_DEBUG, "rss %s ",temp_buffer);   
      
        get_health_data();

        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather data updated. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
first_animation_is_running = false;
second_animation_is_running = false;
third_animation_is_running = false; 
        return;
    }
  
   

    Tuple *enableHealth = dict_find(iterator, KEY_ENABLEHEALTH);
    if (enableHealth) {
        bool health = enableHealth->value->int32;
        persist_write_int(KEY_ENABLEHEALTH, health);
    }

    Tuple *useKm = dict_find(iterator, KEY_USEKM);
    if (useKm) {
        bool km = useKm->value->int8;
        persist_write_int(KEY_USEKM, km);
    }

    Tuple *showSleep = dict_find(iterator, KEY_SHOWSLEEP);
    if (showSleep) {
        bool sleep = showSleep->value->int8;
        persist_write_int(KEY_SHOWSLEEP, sleep);
    }

    Tuple *enableWeather = dict_find(iterator, KEY_ENABLEWEATHER);
    if (enableWeather) {
        bool weather = enableWeather->value->int8;
        persist_write_int(KEY_ENABLEWEATHER, weather);
    }

    Tuple *weatherKey = dict_find(iterator, KEY_WEATHERKEY);
    if (weatherKey) {
        char* key = weatherKey->value->cstring;
        persist_write_string(KEY_WEATHERKEY, key);
    }

    Tuple *useCelsius = dict_find(iterator, KEY_USECELSIUS);
    if (useCelsius) {
        bool celsius = useCelsius->value->int8;
        persist_write_int(KEY_USECELSIUS, celsius);
    }

    Tuple *timezones = dict_find(iterator, KEY_TIMEZONES);
    if (timezones) {
        signed int tz = timezones->value->int8;
        persist_write_int(KEY_TIMEZONES, tz);
        tz_hour = tz;
    }

    Tuple *timezonesMin = dict_find(iterator, KEY_TIMEZONESMINUTES);
    if (timezonesMin) {
        int tz_min = timezonesMin->value->int8;
        persist_write_int(KEY_TIMEZONESMINUTES, tz_min);
        tz_minute = tz_min;
    }

    Tuple *timezonesCode = dict_find(iterator, KEY_TIMEZONESCODE);
    if (timezones) {
        char* tz_code = timezonesCode->value->cstring;
        persist_write_string(KEY_TIMEZONESCODE, tz_code);
        strcpy(tz_name, tz_code);
    }

    Tuple *bgColor = dict_find(iterator, KEY_BGCOLOR);
    if (bgColor) {
        uint32_t bg_c = bgColor->value->int32;
        persist_write_int(KEY_BGCOLOR, bg_c);
    }

    Tuple *hoursColor = dict_find(iterator, KEY_HOURSCOLOR);
    if (hoursColor) {
        uint32_t time_c = hoursColor->value->int32;
        persist_write_int(KEY_HOURSCOLOR, time_c);
    }

    Tuple *enableAdvanced = dict_find(iterator, KEY_ENABLEADVANCED);
    if (enableAdvanced) {
        bool adv = enableAdvanced->value->int8;
        persist_write_int(KEY_ENABLEADVANCED, adv);
    }

    Tuple *dateColor = dict_find(iterator, KEY_DATECOLOR);
    if (dateColor) {
        uint32_t date_c = dateColor->value->int32;
        persist_write_int(KEY_DATECOLOR, date_c);
    }

    Tuple *altHoursColor = dict_find(iterator, KEY_ALTHOURSCOLOR);
    if (altHoursColor) {
        uint32_t alt_c = altHoursColor->value->int32;
        persist_write_int(KEY_ALTHOURSCOLOR, alt_c);
    }

    Tuple *batteryColor = dict_find(iterator, KEY_BATTERYCOLOR);
    if (batteryColor) {
        uint32_t bat_c = batteryColor->value->int32;
        persist_write_int(KEY_BATTERYCOLOR, bat_c);
    }
  

    Tuple *batteryLowColor = dict_find(iterator, KEY_BATTERYLOWCOLOR);
    if (batteryLowColor) {
        uint32_t batl_c = batteryLowColor->value->int32;
        persist_write_int(KEY_BATTERYLOWCOLOR, batl_c);
    }

    Tuple *weatherColor = dict_find(iterator, KEY_WEATHERCOLOR);
    if (weatherColor) {
        uint32_t weather_c = weatherColor->value->int32;
        persist_write_int(KEY_WEATHERCOLOR, weather_c);
    }

    Tuple *tempColor = dict_find(iterator, KEY_TEMPCOLOR);
    if (tempColor) {
        uint32_t temp_c = tempColor->value->int32;
        persist_write_int(KEY_TEMPCOLOR, temp_c);
    }

    Tuple *minColor = dict_find(iterator, KEY_MINCOLOR);
    if (minColor) {
        uint32_t min_c = minColor->value->int32;
        persist_write_int(KEY_MINCOLOR, min_c);
    }

    Tuple *maxColor = dict_find(iterator, KEY_MAXCOLOR);
    if (maxColor) {
        uint32_t max_c = maxColor->value->int32;
        persist_write_int(KEY_MAXCOLOR, max_c);
    }

    Tuple *stepsColor = dict_find(iterator, KEY_STEPSCOLOR);
    if (stepsColor) {
        uint32_t steps_c = stepsColor->value->int32;
        persist_write_int(KEY_STEPSCOLOR, steps_c);
    }

    Tuple *distColor = dict_find(iterator, KEY_DISTCOLOR);
    if (distColor) {
        uint32_t dist_c = distColor->value->int32;
        persist_write_int(KEY_DISTCOLOR, dist_c);
    }

    Tuple *fontType = dict_find(iterator, KEY_FONTTYPE);
    if (fontType) {
        uint8_t font = fontType->value->int8;
        persist_write_int(KEY_FONTTYPE, font);
    }

    Tuple *bluetoothDisconnect = dict_find(iterator, KEY_BLUETOOTHDISCONNECT);
    if (bluetoothDisconnect) {
        uint8_t btd = bluetoothDisconnect->value->int8;
        persist_write_int(KEY_BLUETOOTHDISCONNECT, btd);
    }

    Tuple *bluetoothColor = dict_find(iterator, KEY_BLUETOOTHCOLOR);
    if (bluetoothColor) {
        uint32_t bluetooth_c = bluetoothColor->value->int32;
        persist_write_int(KEY_BLUETOOTHCOLOR, bluetooth_c);
    }

    Tuple *overrideLocation = dict_find(iterator, KEY_OVERRIDELOCATION);
    if (overrideLocation) {
        char* loc = overrideLocation->value->cstring;
        persist_write_string(KEY_OVERRIDELOCATION, loc);
    }

    Tuple *updateAvailable = dict_find(iterator, KEY_UPDATE);
    if (updateAvailable) {
        uint32_t update_a = updateAvailable->value->int32;
        persist_write_int(KEY_UPDATE, update_a);
    }

    Tuple *updateColor = dict_find(iterator, KEY_UPDATECOLOR);
    if (updateColor) {
        uint32_t update_c = updateColor->value->int32;
        persist_write_int(KEY_UPDATECOLOR, update_c);
    }

    Tuple *locale = dict_find(iterator, KEY_LOCALE);
    if (locale) {
        uint8_t locale_v = locale->value->int8;
        persist_write_int(KEY_LOCALE, locale_v);
    }

    Tuple *dateFormat = dict_find(iterator, KEY_DATEFORMAT);
    if (dateFormat) {
        uint8_t dateformat_v = dateFormat->value->int8;
        persist_write_int(KEY_DATEFORMAT, dateformat_v);
 
      
    }
  
   Tuple *leadingZero = dict_find(iterator, KEY_NOLEADINGZERO);
    if (leadingZero) {
        bool zero = leadingZero->value->int8;
        persist_write_int(KEY_NOLEADINGZERO, zero);
    }
  
    Tuple *useBigTemp = dict_find(iterator, KEY_USEBIGTEMP);
    if (useBigTemp) {
        bool bigTemp = useBigTemp->value->int8;
        persist_write_int(KEY_USEBIGTEMP, bigTemp);
    }
  
    Tuple *batteryBackColor = dict_find(iterator, KEY_BATTERYBACKCOLOR);
    if (batteryBackColor) {
        uint32_t batb_c = batteryBackColor->value->int32;
        persist_write_int(KEY_BATTERYBACKCOLOR, batb_c);
    }
  
    Tuple *weatherPins = dict_find(iterator, KEY_WEATHERPINS);
    if (weatherPins) {
        uint8_t weatherPins_v = weatherPins->value->int8;
        persist_write_int(KEY_WEATHERPINS, weatherPins_v);
    }

  /*
    Tuple *stocks = dict_find(iterator, KEY_STOCKS);
    if (stocks) {
        //stocks_v[]= stocks->value->cstring;
        strcpy(stocks_v,stocks->value->cstring);
        persist_write_string(KEY_STOCKS, stocks_v);
    }
 */ 
  
  
    Tuple *shakeAction = dict_find(iterator, KEY_SHAKEACTION);
    if (shakeAction) {
        uint8_t shakeAction_v = shakeAction->value->int8;
        persist_write_int(KEY_SHAKEACTION, shakeAction_v);  
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Shake Action Pin %d", shakeAction_v);   
    }  
  
 
       
      
    Tuple *stocksList = dict_find(iterator, KEY_STOCKSLIST);
    if (stocksList) {
        //stocksList_v[]= stocksList->value->cstring;
        strcpy(stocksList_v,stocksList->value->cstring);
        persist_write_string(KEY_STOCKSLIST, stocksList_v);
    }
  
  Tuple *shakeAction2 = dict_find(iterator, KEY_SHAKEACTION2);
    if (shakeAction2) {
        uint8_t shakeAction2_v = shakeAction2->value->int8;
        persist_write_int(KEY_SHAKEACTION2, shakeAction2_v);  
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Shake Action Pin %d", shakeAction_v);   
    } 
  
 
 APP_LOG(APP_LOG_LEVEL_DEBUG, "before Tuple speedMultiplier");  
   Tuple *speedMultiplier = dict_find(iterator, KEY_SPEEDMULTIPLIER);
    if (speedMultiplier) {
     APP_LOG(APP_LOG_LEVEL_DEBUG, "speedMultiplier = true");
        strcpy(speedMultiplier_v,speedMultiplier->value->cstring);
        persist_write_string(KEY_SPEEDMULTIPLIER, speedMultiplier_v);
    } 
 
  
 Tuple *shakeAction3 = dict_find(iterator, KEY_SHAKEACTION3);
    if (shakeAction) {
        uint8_t shakeAction3_v = shakeAction3->value->int8;
        persist_write_int(KEY_SHAKEACTION3, shakeAction3_v);  
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Shake Action Pin %d", shakeAction_v);   
    } 
  
 Tuple *specifiedRSS = dict_find(iterator, KEY_SPECIFIEDRSS);
    if (specifiedRSS) {
        uint8_t specifiedRSS_v = specifiedRSS->value->int8;
        persist_write_int(KEY_SPECIFIEDRSS, specifiedRSS_v);  
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Shake Action Pin %d", shakeAction_v);   
    }  

  /*
 Tuple *rss = dict_find(iterator, KEY_RSS);
    if (rss) {
        //stocks_v[]= stocks->value->cstring;
        strcpy(rss_v,rss->value->cstring);
        persist_write_string(KEY_RSS, rss_v);
    }  
  
*/
  
first_animation_is_running = false;
second_animation_is_running = false;
third_animation_is_running = false; 
  
 APP_LOG(APP_LOG_LEVEL_DEBUG, "after Tuple speedMultiplier");   
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Configs persisted. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    destroy_text_layers();
    create_text_layers(watchface);
    load_screen(true);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped! Reason code: %d", reason);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! Reason code: %d", reason);
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Outbox send success!");
}

static void watchface_load(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Watchface load start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));


  
      // KAH 3/21/2016
    accel_tap_service_subscribe(&accel_tap_handler);
  
    create_text_layers(window);
  
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    parent_bounds = bounds.size.w -20;

    // Create BATTERY meter Layer
  
    s_battery_layer = layer_create(GRect(PBL_IF_ROUND_ELSE(20,10), PBL_IF_ROUND_ELSE(125, 120), bounds.size.w -PBL_IF_ROUND_ELSE(40,20), 2));
    layer_set_update_proc(s_battery_layer, battery_update_proc);
    layer_add_child(window_get_root_layer(window), s_battery_layer);
    battery_handler(battery_state_service_peek());
  
         
      
      
    if (persist_exists(KEY_TIMEZONESCODE)) {
        persist_read_string(KEY_TIMEZONESCODE, tz_name, sizeof(tz_name));
        tz_hour = persist_exists(KEY_TIMEZONES) ? persist_read_int(KEY_TIMEZONES) : 0;
        tz_minute = persist_exists(KEY_TIMEZONESMINUTES) ? persist_read_int(KEY_TIMEZONESMINUTES) : 0;
    } else {
        tz_name[0] = '#';
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Watchface load end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void watchface_unload(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unload start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    
    unload_face_fonts();

    destroy_text_layers();
    layer_destroy(s_battery_layer);
  // KAH 3/24/2016
    accel_tap_service_unsubscribe();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unload end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}




static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    //bool is_sleeping = is_user_sleeping();

    //show_sleep_data_if_visible();

//    bool update_enabled = persist_exists(KEY_UPDATE) ? persist_read_int(KEY_UPDATE) : true;

//    if (update_enabled && tick_time->tm_hour == 4) { // updates at 4am
//        check_for_updates();
//    }
//    if (!update_enabled) {
//        notify_update(false);
//    }

    //uint8_t tick_interval = is_sleeping ? 60 : 30;
    uint8_t tick_interval = 30;
  
    if((tick_time->tm_min % tick_interval == 0) && is_weather_enabled()) {
        update_weather();

    }
 
   
  
}


static void init(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Init start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    init_sleep_data();

    watchface = window_create();

    window_set_window_handlers(watchface, (WindowHandlers) {
        .load = watchface_load,
        .unload = watchface_unload,
    });

    battery_state_service_subscribe(battery_handler);
  
    // KAH 3/21/2016
    //accel_tap_service_subscribe(&accel_tap_handler);
  
    window_stack_push(watchface, true);

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_open(384, 64);

  
  
    connection_service_subscribe((ConnectionHandlers) {
	.pebble_app_connection_handler = bt_handler
    });

    load_screen(false);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Init end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void deinit(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Deinit start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    // KAH 3/21/2016
    //accel_tap_service_unsubscribe();
    window_destroy(watchface);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Deinit end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

int main(void) {

    APP_LOG(APP_LOG_LEVEL_DEBUG, "App start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    init();
    app_event_loop();
    deinit();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

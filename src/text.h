#ifndef __TIMEBOXED_TEXT_
#define __TIMEBOXED_TEXT_

#include <pebble.h>

void run_animation();

void create_text_layers(Window*);

void destroy_text_layers();

void load_face_fonts();
void unload_face_fonts();
void set_face_fonts();
void set_colors(Window*);

uint8_t get_loaded_font();

void set_hours_layer_text(char*);
void set_date_layer_text(char*);
void set_alt_time_layer_text(char*);
void set_battery_layer_text(char*);
void set_bluetooth_layer_text(char*);
void set_temp_cur_layer_text(char*);
void set_temp_max_layer_text(char*);
void set_temp_min_layer_text(char*);
void set_steps_or_sleep_layer_text(char*);
void set_dist_or_deep_layer_text(char*);
void set_weather_layer_text(char*);
void set_max_icon_layer_text(char*);
void set_min_icon_layer_text(char*);
void set_update_layer_text(char*);

void hide_ticker(int shakeOption);
void display_ticker(int shakeOption);
void set_ticker(char *ticker);

extern int16_t ticker_pixels;


extern int hours_top;
extern int width;
extern int date_left;
extern int date_top;
extern int alt_top;
extern int battery_top;
extern int bt_top;

extern int temp_min_max_top;
extern int temp_icon_min_max_top;
extern int update_top;

extern GFont time_font;
extern GFont medium_font;
extern GFont base_font;
extern GFont steps_font;
extern GFont weather_font;
extern GFont weather_big_font;
extern GFont awesome_font;

extern TextLayer *hours;
extern TextLayer *date;
extern TextLayer *ticker_text;
extern TextLayer *alt_time;
extern TextLayer *battery;
extern TextLayer *bluetooth;
extern TextLayer *temp_cur;
extern TextLayer *temp_max;
extern TextLayer *temp_min;
extern TextLayer *steps_or_sleep;
extern TextLayer *dist_or_deep;
extern TextLayer *weather;
extern TextLayer *max_icon;
extern TextLayer *min_icon;
extern TextLayer *update;

extern GColor base_color;




#endif

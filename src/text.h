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
int16_t ticker_pixels;


int hours_top;
int width;
int date_left;
int date_top;
int alt_top;
int battery_top;
int bt_top;
   // int temp_cur_top;
int temp_min_max_top;
int temp_icon_min_max_top;
int update_top;

GFont time_font;
GFont medium_font;
GFont base_font;
GFont steps_font;
GFont weather_font;
GFont weather_big_font;
GFont awesome_font;

TextLayer *hours;
TextLayer *date;
TextLayer *ticker_text;
TextLayer *alt_time;
TextLayer *battery;
TextLayer *bluetooth;
TextLayer *temp_cur;
TextLayer *temp_max;
TextLayer *temp_min;
TextLayer *steps_or_sleep;
TextLayer *dist_or_deep;
TextLayer *weather;
TextLayer *max_icon;
TextLayer *min_icon;
TextLayer *update;

GColor base_color;


#endif

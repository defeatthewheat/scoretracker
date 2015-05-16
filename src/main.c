/*
 * main.c
 * Creates a Window, TextLayer and shows the number of seconds, minutes 
 * and hours since launch.
 */

#include <pebble.h>

static Window *s_main_window;
static TextLayer *time_layer;

static int s_uptime = 0;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Use a long-lived buffer
  // Get time since launch
 
  time_t temp = time(NULL);
  
  tick_time = localtime(&temp);
  static char buffer[] = "00:00";
  strftime(buffer,sizeof("00:00"), "%H:%M", tick_time);
  
    // Update the TextLayer
  text_layer_set_text(time_layer, buffer);

  // Increment s_uptime
  s_uptime++;
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create output TextLayer
  time_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, fonts_load_system_font(FONT_KEY_BITHAM_42_BOLD));

  layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(time_layer);
}

static void init(void) {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  // Subscribe to TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

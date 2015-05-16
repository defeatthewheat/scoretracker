/*
 * main.c
 * Creates a Window, TextLayer and shows the number of seconds, minutes 
 * and hours since launch.
 */

#include <pebble.h>

static Window *s_main_window;
static TextLayer *teams_layer;
static TextLayer *time_layer;
static TextLayer *battery_disp_layer;
static char sys_battery_buff[16];

static int s_uptime = 0;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Get time
 
  time_t temp = time(NULL);
  
  tick_time = localtime(&temp);
  static char buffer[] = "00:00";
  strftime(buffer,sizeof("00:00"), "%H:%M", tick_time);
	
  // Update the TextLayer
  text_layer_set_text(time_layer, buffer);

  // Increment s_uptime
  s_uptime++;
}

//Handles battery percentages
static void battery_handler(BatteryChargeState new_state) {
  static char sys_battery_buff[32];
  snprintf(sys_battery_buff, sizeof(sys_battery_buff), "%d%%", new_state.charge_percent);
  text_layer_set_text(battery_disp_layer, sys_battery_buff);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create output TextLayer for battery percentage
	battery_disp_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
	text_layer_set_text_alignment(battery_disp_layer, GTextAlignmentRight);
	text_layer_set_font(battery_disp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(battery_disp_layer));
	
	//Check on Battery percentage
	battery_handler(battery_state_service_peek());
	
	// Create output TextLayer for time
  time_layer = text_layer_create(GRect(0, 25, window_bounds.size.w, window_bounds.size.h));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
	
	//output TextLayers for teams
	
	teams_layer = text_layer_create(GRect(0, 80, window_bounds.size.w, window_bounds.size.h));
	text_layer_set_text_alignment(teams_layer, GTextAlignmentCenter);
	text_layer_set_font(teams_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text(teams_layer, "SAN v. CIN");
	
	layer_add_child(window_layer, text_layer_get_layer(teams_layer));
	
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

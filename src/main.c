#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_time_font;

/**
 *
 */
static void update_datetime() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char timeBuffer[] = "0000";
  static char dateBuffer[] = "0000";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(timeBuffer, sizeof("0000"), "%H%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(timeBuffer, sizeof("0000"), "%I%M", tick_time);
  }
  
  // Write the date into the buffer.
  strftime(dateBuffer, sizeof("0000"), "%d%m", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, timeBuffer);
  text_layer_set_text(s_date_layer, dateBuffer);
}

/**
 *
 */
static void main_window_load(Window *window) {
  // Setup the Window layer
  window_set_background_color(window, GColorBlack);
  
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(5, 35, 139, 45));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "0000");
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(5, 82, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "0000");

  // Setup fonts
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ZDYK_42));
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_date_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

  // Make sure the time is displayed from the start
  update_datetime();
}

static void main_window_unload(Window *window) {
  //gbitmap_destroy(s_background_bitmap);
  //bitmap_layer_destroy(s_background_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  fonts_unload_custom_font(s_time_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_datetime();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

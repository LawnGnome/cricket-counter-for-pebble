#include <pebble.h>
#include "cricket.h"

#define PEBBLE_WIDTH 144 - ACTION_BAR_WIDTH
#define HORIZONTAL_MARGIN 4
#define VERTICAL_MARGIN 6
#define INNINGS_KEY 1

struct
{
    Layer *top;
    ActionBarLayer *actionBar;
    TextLayer *ball;
    TextLayer *over;
    TextLayer *wicket;
    StatusBarLayer *statusBar;
} layers;

GBitmap *menuIcons[3];
GBitmap *windowIcon;
Window *window;
Innings innings;

char ballBuffer[10];
char overBuffer[17];
char wicketBuffer[13];

void update_text()
{
    snprintf(ballBuffer, sizeof(ballBuffer), "Ball: %u", innings_get_ball(&innings));
    text_layer_set_text(layers.ball, ballBuffer);

    snprintf(overBuffer, sizeof(overBuffer), "Over: %u", innings_get_over(&innings));
    text_layer_set_text(layers.over, overBuffer);

    snprintf(wicketBuffer, sizeof(wicketBuffer), "Wickets: %u", innings_get_wicket(&innings));
    text_layer_set_text(layers.wicket, wicketBuffer);
}

void handle_down(ClickRecognizerRef recognizer, void *ctx)
{
    (void)recognizer;
    (void)ctx;

    innings_decrement_ball(&innings);
    update_text();
}

void handle_reset(ClickRecognizerRef recognizer, void *ctx)
{
    (void)recognizer;
    (void)ctx;

    innings_init(&innings);
    update_text();
}

void handle_select(ClickRecognizerRef recognizer, void *ctx)
{
    (void)recognizer;
    (void)ctx;

    innings_increment_wicket(&innings);
    update_text();
}

void handle_up(ClickRecognizerRef recognizer, void *ctx)
{
    (void)recognizer;
    (void)ctx;

    innings_increment_ball(&innings);
    update_text();
}

void click_config_provider(void *ctx)
{
    (void)ctx;

    window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, handle_up);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, handle_down);

    window_single_click_subscribe(BUTTON_ID_SELECT, handle_select);
    window_long_click_subscribe(BUTTON_ID_SELECT, 500, handle_reset, NULL);
}

void update_top_callback(Layer *me, GContext *ctx)
{
    (void)me;

    update_text();
}

void handle_init()
{
    windowIcon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WINDOW_ICON);

    window = window_create();
    window_set_background_color(window, GColorWhite);
    window_stack_push(window, true);

    innings_init(&innings);
    persist_read_data(INNINGS_KEY, &innings, sizeof(innings));

    layers.top = layer_create(layer_get_frame(window_get_root_layer(window)));
    layer_set_update_proc(layers.top, update_top_callback);
    layer_add_child(window_get_root_layer(window), layers.top);

    menuIcons[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP);
    menuIcons[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WICKET);
    menuIcons[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOWN);

    layers.actionBar = action_bar_layer_create();
    action_bar_layer_add_to_window(layers.actionBar, window);
    action_bar_layer_set_click_config_provider(layers.actionBar, click_config_provider);
    action_bar_layer_set_icon(layers.actionBar, BUTTON_ID_UP, menuIcons[0]);
    action_bar_layer_set_icon(layers.actionBar, BUTTON_ID_SELECT, menuIcons[1]);
    action_bar_layer_set_icon(layers.actionBar, BUTTON_ID_DOWN, menuIcons[2]);

    layers.ball = text_layer_create(GRect(HORIZONTAL_MARGIN, VERTICAL_MARGIN + STATUS_BAR_LAYER_HEIGHT, PEBBLE_WIDTH - 2 * VERTICAL_MARGIN, 50));
    text_layer_set_font(layers.ball, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_32)));
    text_layer_set_text_alignment(layers.ball, GTextAlignmentCenter);
    text_layer_set_text_color(layers.ball, GColorBlack);
    layer_add_child(layers.top, text_layer_get_layer(layers.ball));

    layers.over = text_layer_create(GRect(HORIZONTAL_MARGIN, 3 * VERTICAL_MARGIN + 60, PEBBLE_WIDTH - 2 * VERTICAL_MARGIN, 32));
    text_layer_set_font(layers.over, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_24)));
    text_layer_set_text_alignment(layers.over, GTextAlignmentCenter);
    text_layer_set_text_color(layers.over, GColorBlack);
    layer_add_child(layers.top, text_layer_get_layer(layers.over));

    layers.wicket = text_layer_create(GRect(HORIZONTAL_MARGIN, 5 * VERTICAL_MARGIN + 88, PEBBLE_WIDTH - 2 * VERTICAL_MARGIN, 28));
    text_layer_set_font(layers.wicket, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_20)));
    text_layer_set_text_alignment(layers.wicket, GTextAlignmentCenter);
    text_layer_set_text_color(layers.wicket, GColorBlack);
    layer_add_child(layers.top, text_layer_get_layer(layers.wicket));

    layers.statusBar = status_bar_layer_create();
    layer_add_child(layers.top, status_bar_layer_get_layer(layers.statusBar));
}

void handle_deinit()
{
    persist_write_data(INNINGS_KEY, &innings, sizeof(innings));

    window_destroy(window);

    gbitmap_destroy(windowIcon);
    gbitmap_destroy(menuIcons[0]);
    gbitmap_destroy(menuIcons[1]);
    gbitmap_destroy(menuIcons[2]);
}

int main()
{
    handle_init();
    app_event_loop();
    handle_deinit();

    return 0;
}

/* vim: set cin ai et ts=4 sw=4: */

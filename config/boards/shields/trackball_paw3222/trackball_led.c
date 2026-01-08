/*
 * Trackball LED Status Indicator
 * Custom implementation for peripheral devices
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(trackball_led, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/split/bluetooth/peripheral.h>

// LED GPIO definitions for Xiao nRF52840
#define LED_RED_NODE   DT_ALIAS(led_red)
#define LED_GREEN_NODE DT_ALIAS(led_green)
#define LED_BLUE_NODE  DT_ALIAS(led_blue)

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

// LED state
static struct k_work_delayable led_update_work;
static bool leds_initialized = false;
static bool last_connected = false;

// Battery thresholds
#define BATTERY_HIGH 80
#define BATTERY_LOW 20
#define BATTERY_CRITICAL 10

// Timing constants (in milliseconds)
#define LED_DISPLAY_DURATION 1000       // Display status for 1 second

// LED control functions
static void led_set(const struct gpio_dt_spec *led, bool on) {
    if (!device_is_ready(led->port)) {
        return;
    }
    gpio_pin_set_dt(led, on ? 1 : 0);
}

static void led_all_off(void) {
    led_set(&led_red, false);
    led_set(&led_green, false);
    led_set(&led_blue, false);
}

static void led_show_status(uint8_t battery_level, bool connected) {
    led_all_off();

    if (!connected) {
        // Disconnected: Slow blinking red to indicate connection issue
        led_set(&led_red, true);
    } else if (battery_level >= BATTERY_HIGH) {
        // High battery: Green
        led_set(&led_green, true);
    } else if (battery_level >= BATTERY_LOW) {
        // Medium battery: Blue (normal operation)
        led_set(&led_blue, true);
    } else if (battery_level >= BATTERY_CRITICAL) {
        // Low battery: Yellow (red + green) - warning
        led_set(&led_red, true);
        led_set(&led_green, true);
    } else {
        // Critical battery: Red - urgent warning
        led_set(&led_red, true);
    }
}

static void led_update_handler(struct k_work *work) {
    if (!leds_initialized) {
        return;
    }

    // Get battery level and connection status
    uint8_t battery_level = zmk_battery_state_of_charge();
    bool connected = zmk_split_bt_peripheral_is_connected();

    // Detect connection state change - show status when connected
    if (connected && !last_connected) {
        LOG_INF("Connected! Showing status. Battery: %d%%", battery_level);
        last_connected = true;

        // Show status for 1 second
        led_show_status(battery_level, connected);
        k_work_schedule(&led_update_work, K_MSEC(LED_DISPLAY_DURATION));
        return;
    }

    if (!connected && last_connected) {
        LOG_INF("Disconnected!");
        last_connected = false;
    }

    // Turn off LED after display duration
    led_all_off();
}

static void led_show_status_temporary(void) {
    if (!leds_initialized) {
        return;
    }

    uint8_t battery_level = zmk_battery_state_of_charge();
    bool connected = zmk_split_bt_peripheral_is_connected();

    LOG_INF("Showing status: Battery: %d%%, Connected: %d", battery_level, connected);

    // Show status for 1 second, then turn off
    led_show_status(battery_level, connected);
    k_work_schedule(&led_update_work, K_MSEC(LED_DISPLAY_DURATION));
}

static int led_listener_battery_changed(const zmk_event_t *eh) {
    // Show status briefly when battery level changes
    led_show_status_temporary();
    return 0;
}

ZMK_LISTENER(trackball_led_battery, led_listener_battery_changed);
ZMK_SUBSCRIPTION(trackball_led_battery, zmk_battery_state_changed);

static int trackball_led_init(void) {
    int ret;

    LOG_INF("Initializing trackball LED indicators");

    // Configure LED GPIOs
    if (!device_is_ready(led_red.port) ||
        !device_is_ready(led_green.port) ||
        !device_is_ready(led_blue.port)) {
        LOG_ERR("LED GPIO devices not ready");
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure red LED: %d", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure green LED: %d", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure blue LED: %d", ret);
        return ret;
    }

    // Turn off all LEDs initially
    led_all_off();

    // Initialize work item
    k_work_init_delayable(&led_update_work, led_update_handler);

    leds_initialized = true;

    // Show initial status on boot for 1 second
    led_show_status_temporary();

    LOG_INF("Trackball LED indicators initialized successfully");

    return 0;
}

SYS_INIT(trackball_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

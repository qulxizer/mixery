#include "bsp/board_api.h"
#include "layout.h"
#include <hardware/gpio.h>
#include <pico/time.h>

void blink(int delay) {
  board_led_on();
  sleep_ms(100);
  board_led_off();
  sleep_ms(100);
}

void button_irq_handler(uint gpio, uint32_t events) {
  gpio_acknowledge_irq(gpio, events);
  board_led_on();
  DeviceSlot *slot = lookup_button(layout, gpio);
  if (!slot) {
    return;
  }
  slot->button.pressed = !gpio_get(gpio);
  slot->changed = true;
}

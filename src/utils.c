#include "bsp/board_api.h"
#include "layout.h"
#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdint.h>

void blink(int delay) {
  board_led_on();
  sleep_ms(100);
  board_led_off();
  sleep_ms(100);
}

void button_irq_handler(uint gpio, uint32_t events) {
  DeviceSlot *slot = lookup_button(layout, gpio);
  if (!slot) {
    return;
  }
  slot->button.pressed = !gpio_get(gpio);
  slot->changed = true;
}

void encoder_irq_handler(uint gpio, uint32_t events) {
  DeviceSlot *slot = lookup_encoder(layout, gpio);

  if (!slot) {
    return;
  }

  bool a = gpio_get(slot->encoder.pinA);
  bool b = gpio_get(slot->encoder.pinB);

  uint8_t current = (a << 1) | b;
  uint8_t previous = slot->encoder.last_b_position;

  static const int8_t transition[4][4] = {
      {0, 1, -1, 0}, {-1, 0, 0, 1}, {1, 0, 0, -1}, {0, -1, 1, 0}};

  int8_t delta = transition[previous][current];

  if (delta != 0) {
    slot->encoder.presentage += delta;
    slot->changed = true;
  }

  slot->encoder.last_b_position = current;
}

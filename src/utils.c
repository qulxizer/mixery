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
  if (slot->encoder.pinA == 0 && slot->encoder.pinB == 0) {
    return;
  }

  bool a = gpio_get(slot->encoder.pinA);
  bool b = gpio_get(slot->encoder.pinB);
  if (b != slot->encoder.last_b_position) {
    if (a != b) {
      slot->encoder.presentage++;
    } else {
      slot->encoder.presentage--;
    }
    slot->encoder.last_b_position = b;
  }

  slot->changed = true;
}

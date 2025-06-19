#include "device/usbd.h"
#include "usb_descriptors.h"
#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdint.h>
#include <string.h>

#include "bsp/board_api.h"
#include "hardware/watchdog.h"
#include <pico/time.h>

#define BOOTSEL_MAGIC 0xB007C0DE

void reset_to_bootsel(uint32_t timeout) {
  // Write the magic value to 0x20041FF0 (end of SRAM)
  *((uint32_t *)0x20041FF0) = BOOTSEL_MAGIC;

  watchdog_enable(timeout, 1); // Reset in 1ms
  while (1)
    ; // wait for reset
}

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage,
                                tusb_control_request_t const *request) {
  // nothing to with DATA & ACK stage
  if (stage != CONTROL_STAGE_SETUP)
    return true;

  switch (request->bmRequestType_bit.type) {
  case TUSB_REQ_TYPE_VENDOR:
    switch (request->bRequest) {

    case VENDOR_REQUEST_MICROSOFT:
      if (request->wIndex == 7) {
        // Get Microsoft OS 2.0 compatible descriptor
        uint16_t total_len;
        memcpy(&total_len, desc_ms_os_20 + 8, 2);

        return tud_control_xfer(rhport, request,
                                (void *)(uintptr_t)desc_ms_os_20, total_len);
      } else {
        return false;
      }

    case VENDOR_REQUEST_BLINK:
      board_led_on();
      sleep_ms(100);
      board_led_off();
      return tud_control_status(rhport, request);
    case VENDOR_REQUEST_UPDATE:
      return tud_control_status(rhport, request); // Ack the request
      reset_to_bootsel(1);
    default:
      break;
    }
    break;

  default:
    break;
  }

  // stall unknown request
  return false;
}

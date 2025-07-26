#include "bsp/board_api.h"
#include "class/hid/hid_device.h"
#include "layout.h"
#include "report.h"
#include "usb_descriptors.h"
#include "utils.h"
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <pico/bootrom.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Layout *layout = NULL;

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {}

// Invoked when device is unmounted
void tud_umount_cb(void) {}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;
  switch (report_id) {

  // This is just a test not ment for real usage
  case GET_REPORT_ID_GET_SLOT: {
    if (!layout) {
      return 0;
    }
    const DeviceSlot *slot = get_slot(layout, 3, 4);
    if (!slot) {
      return 0;
    }
    memcpy(buffer, slot, sizeof(DeviceSlot));
    return sizeof(DeviceSlot);
  }
  }

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  switch (report_id) {
  case SET_REPORT_ID_CREATE_LAYOUT: {
    const DeviceSizeReport *report = (const DeviceSizeReport *)buffer;
    if (report->rows == 0 || report->cols == 0)
      return;

    if (layout) {
      free_layout(layout);
      layout = NULL;
    }
    layout = create_layout(report->rows, report->cols);
    layout->layout_header->rows = report->rows;
    layout->layout_header->cols = report->cols;
    if (!layout) {
      return;
    }
    return;
  }
  case SET_REPORT_ID_SET_SLOT: {
    const DeviceSlot *report = (const DeviceSlot *)buffer;
    void *ptr = malloc(64);

    switch (report->type) {
    case DEVICE_BUTTON: {
      set_slot(layout, *report);
      gpio_init(report->button.pin);
      gpio_pull_up(report->button.pin);
      gpio_set_dir(report->button.pin, GPIO_IN);
      gpio_set_irq_enabled_with_callback(
          report->button.pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true,
          button_irq_handler);
      break;

    case DEVICE_ENCODER: {
      set_slot(layout, *report);
      gpio_init(report->encoder.pinA);
      gpio_init(report->encoder.pinB);
      gpio_pull_up(report->encoder.pinA);
      gpio_pull_up(report->encoder.pinB);
      gpio_set_dir(report->encoder.pinA, GPIO_IN);
      gpio_set_dir(report->encoder.pinB, GPIO_IN);
      gpio_set_irq_enabled_with_callback(
          report->encoder.pinA, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true,
          encoder_irq_handler);
      break;
    }
    }
    }
    break;
  }
  case SET_REPORT_ID_RESET_TO_BOOT: {
    blink(100);
    reset_usb_boot(0, 0);
    break;
  }
  }
}

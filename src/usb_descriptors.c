/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "usb_descriptors.h"
#include "bsp/board_api.h"
#include "class/hid/hid.h"
#include "class/hid/hid_device.h"
#include "layout.h"

/* A combination of interfaces must have a unique product id, since PC will save
 * device driver after the first plug. Same VID/PID with different interface e.g
 * MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID                                                                \
  (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) |           \
   _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device = {.bLength = sizeof(tusb_desc_device_t),
                                        .bDescriptorType = TUSB_DESC_DEVICE,
                                        .bcdUSB = 0x0200,
                                        .bDeviceClass = 0x00,
                                        .bDeviceSubClass = 0x00,
                                        .bDeviceProtocol = 0x00,
                                        .bMaxPacketSize0 =
                                            CFG_TUD_ENDPOINT0_SIZE,

                                        .idVendor = 0xCafe,
                                        .idProduct = USB_PID,
                                        .bcdDevice = 0x0100,

                                        .iManufacturer = 0x01,
                                        .iProduct = 0x02,
                                        .iSerialNumber = 0x03,

                                        .bNumConfigurations = 0x01};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) {
  return (uint8_t const *)&desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

#define TUD_HID_REPORT_DESC_CUSTOM(report_size, ...)                           \
  HID_USAGE_PAGE_N(HID_USAGE_PAGE_VENDOR, 2), HID_USAGE(0xA0),                 \
      HID_COLLECTION(HID_COLLECTION_APPLICATION), /* Report ID if any */       \
      __VA_ARGS__                                 /* Input */                  \
          HID_USAGE(0xA1),                                                     \
      HID_LOGICAL_MIN(0x00), HID_LOGICAL_MAX_N(0xff, 2), HID_REPORT_SIZE(8),   \
      HID_REPORT_COUNT(8),                                                     \
      HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO |         \
                HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION),      \
      HID_USAGE(0xa3), HID_REPORT_COUNT(24),                                   \
      HID_FEATURE(HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO |       \
                  HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION |    \
                  HID_NON_VOLATILE),                                           \
      HID_COLLECTION_END

// Example HID Report Descriptor
uint8_t const desc_hid_report[] = {
    0x06, 0x00, 0xFF, // Usage Page (Vendor Defined)
    0x09, 0x01,       // Usage (Vendor Usage 1)
    0xA1, 0x01,       // Collection (Application)

    // Input report for DEVICE_BUTTON
    0x85, REPORT_ID_INPUT_BUTTON, // Report ID
    0x09, 0x06,                   // Usage (Vendor-defined)
    0x15, 0x00,                   // Logical Min
    0x26, 0xFF, 0x00,             // Logical Max
    0x75, 0x08,                   // Report Size (8 bits)
    0x95, sizeof(DeviceSlot),     // Report Count (match your struct)
    0x81, 0x02,                   // Input (Data,Var,Abs)

    // Feature Report: GET_SLOT (report ID 1)
    0x85, GET_REPORT_ID_GET_SLOT, // Report ID 1
    0x09, 0x01,                   //   Usage (Vendor Defined)
    0x15, 0x00,                   //   Logical Minimum (0)
    0x26, 0xFF, 0x00,             //   Logical Maximum (255)
    0x75, 0x08,                   //   Report Size (8 bits)
    0x95, 0x10,                   //   Report Count (16 bytes)
    0xB1, 0x02,                   //   Feature (Data,Var,Abs)

    // Feature Report: BLINK (report ID 1)
    0x85, SET_REPORT_ID_BLINK, // Report ID 1 (same ID, different direction)
    0x09, 0x02, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x10, 0xB1,
    0x02,

    // Feature Report: CREATE_LAYOUT (report ID 2)
    0x85, SET_REPORT_ID_CREATE_LAYOUT, 0x09, 0x03, 0x15, 0x00, 0x26, 0xFF, 0x00,
    0x75, 0x08, 0x95, 0x10, 0xB1, 0x02,

    // Feature Report: SET_SLOT (report ID 3)
    0x85, SET_REPORT_ID_SET_SLOT, 0x09, 0x04, 0x15, 0x00, 0x26, 0xFF, 0x00,
    0x75, 0x08, 0x95, sizeof(DeviceSlot), 0xB1, 0x02,

    // Feature Report: RESET_TO_BOOT (report ID 4)
    0x85, SET_REPORT_ID_RESET_TO_BOOT, 0x09, 0x05, 0x15, 0x00, 0x26, 0xFF, 0x00,
    0x75, 0x08, 0x95, 0x10, 0xB1, 0x02,

    0xC0 // End Collection
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
  (void)itf;
  return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum { ITF_NUM_HID, ITF_NUM_TOTAL };

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN)

#define EPNUM_HID 0x01

uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute,
    // power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

    // Interface number, string index, protocol, report descriptor len, EP Out &
    // In address, size & polling interval
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                             sizeof(desc_hid_report), EPNUM_HID,
                             0x80 | EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 10)};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  (void)index; // for multiple configurations
  return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// String Descriptor Index
enum {
  STRID_LANGID = 0,
  STRID_MANUFACTURER,
  STRID_PRODUCT,
  STRID_SERIAL,
};

// array of pointer to string descriptors
char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "Mijlas",                   // 1: Manufacturer
    "Mixery",                   // 2: Product
    NULL,                       // 3: Serials will use unique ID if possible
};

static uint16_t _desc_str[32 + 1];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void)langid;
  size_t chr_count;

  switch (index) {
  case STRID_LANGID:
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
    break;

  case STRID_SERIAL:
    chr_count = board_usb_get_serial(_desc_str + 1, 32);
    break;

  default:
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
      return NULL;

    const char *str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    size_t const max_count =
        sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
    if (chr_count > max_count)
      chr_count = max_count;

    // Convert ASCII string into UTF-16
    for (size_t i = 0; i < chr_count; i++) {
      _desc_str[1 + i] = str[i];
    }
    break;
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

  return _desc_str;
}

#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include "sys/types.h"
#include <stdbool.h>
#include <stdint.h>
#include "tusb_config.h"
#define STATIC_ASSERT_STRUCT_SIZE(type, max_size) \
    _Static_assert(sizeof(type) <= (max_size), #type " exceeds " #max_size " bytes")
typedef enum {
    DEVICE_NONE,
    DEVICE_BUTTON,
    DEVICE_ENCODER
} DeviceType;

typedef struct __attribute__((packed)) {
    uint8_t pin;
    uint8_t id;
    bool pressed;
} Button;
STATIC_ASSERT_STRUCT_SIZE(Button, CFG_TUD_HID_EP_BUFSIZE);

typedef struct __attribute__((packed)) {
    uint8_t pinA;
    uint8_t pinB;
    uint8_t id;
    uint8_t presentage;
} Encoder;
STATIC_ASSERT_STRUCT_SIZE(Encoder, CFG_TUD_HID_EP_BUFSIZE);

typedef struct __attribute__((packed)) {
    uint8_t row;
    uint8_t col;
    uint8_t changed;
    uint8_t type;
    char action[16];
    union {
        Button button;
        Encoder encoder;
    };
} DeviceSlot;
STATIC_ASSERT_STRUCT_SIZE(DeviceSlot, CFG_TUD_HID_EP_BUFSIZE);

typedef struct __attribute__((packed)){
    uint8_t rows;
    uint8_t cols;
} LayoutHeader;
STATIC_ASSERT_STRUCT_SIZE(LayoutHeader, CFG_TUD_HID_EP_BUFSIZE);

typedef struct __attribute__((packed)){
    DeviceSlot* slots;
    LayoutHeader *layout_header;
} Layout;
STATIC_ASSERT_STRUCT_SIZE(Layout, CFG_TUD_HID_EP_BUFSIZE);

extern Layout* layout;

Layout* create_layout(uint8_t rows, uint8_t cols);
void free_layout(Layout * layout);
void set_slot(Layout* layout, DeviceSlot slot);
DeviceSlot* get_slot(Layout* layout, uint8_t row, uint8_t col);
DeviceSlot *lookup_button(Layout *layout, uint pin);
#endif


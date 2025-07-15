#include "layout.h"
#include "bsp/board_api.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Layout *create_layout(uint8_t rows, uint8_t cols) {
  if (rows == 0 || cols == 0) {
    return NULL;
  }
  Layout *layout = calloc(1, sizeof(Layout));
  if (!layout)
    return NULL;

  layout->layout_header = calloc(1, sizeof(LayoutHeader));
  if (!layout->layout_header) {
    free(layout);
    return NULL;
  }

  layout->layout_header->rows = rows;
  layout->layout_header->cols = cols;

  layout->slots = calloc(rows * cols, sizeof(DeviceSlot));
  if (!layout->slots) {
    free(layout->layout_header);
    free(layout);
    return NULL;
  }

  return layout;
  return layout;
}

void free_layout(Layout *layout) {
  if (!layout)
    return;
  free(layout->slots);
  free(layout);
}

void set_slot(Layout *layout, DeviceSlot slot) {
  layout->slots[slot.row * layout->layout_header->cols + slot.col] = slot;
}

DeviceSlot *get_slot(Layout *layout, uint8_t row, uint8_t col) {
  if (!layout) {
    return NULL;
  }
  DeviceSlot *slot = &layout->slots[row * layout->layout_header->cols + col];
  return slot;
}

DeviceSlot *lookup_button(Layout *layout, uint pin) {
  if (!layout) {
    return NULL;
  }
  for (int i = 0; i <= layout->layout_header->cols; i++) {
    for (int j = 0; j <= layout->layout_header->rows; j++) {
      DeviceSlot *slot = get_slot(layout, j, i);
      if (!slot) {
        continue;
      }
      if (slot->type == DEVICE_BUTTON && slot->button.pin == pin) {
        return slot;
      }
    }
  }

  return NULL;
}

DeviceSlot *lookup_encoder(Layout *layout, uint pinA, uint pinB) {
  if (!layout) {
    return NULL;
  }
  for (int i = 0; i <= layout->layout_header->cols; i++) {
    for (int j = 0; j <= layout->layout_header->rows; j++) {
      DeviceSlot *slot = get_slot(layout, j, i);
      if (!slot) {
        continue;
      }
      if (slot->type == DEVICE_ENCODER && slot->encoder.pinA == pinA &&
          slot->encoder.pinB == pinB) {
        return slot;
      }
    }
  }

  return NULL;
}

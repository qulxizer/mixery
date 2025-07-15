#include "bsp/board_api.h"
#include "class/hid/hid_device.h"
#include "device/usbd.h"
#include "layout.h"
#include "usb_descriptors.h"
#include "utils.h"
#include <hardware/gpio.h>

void hid_task(Layout *layout);

/*------------- MAIN -------------*/
int main(void) {
  board_init();

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  while (1) {
    tud_task();
    if (layout) {
      hid_task(layout);
    }
  }
}

void hid_task(Layout *layout) {
  for (int i = 0; i <= layout->layout_header->cols; i++) {
    for (int j = 0; j <= layout->layout_header->rows; j++) {
      DeviceSlot *slot = get_slot(layout, j, i);
      if (!slot) {
        continue;
      }

      switch (slot->type)
      case DEVICE_BUTTON: {
        if (!slot->changed) {
          continue;
        }
        if (true) {
          tud_hid_report(REPORT_ID_INPUT_BUTTON, slot, sizeof(DeviceSlot));
          slot->changed = false;
        }
      }
    }
  }
}

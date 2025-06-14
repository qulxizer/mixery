#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"
int main(void) {

  tusb_rhport_init_t dev_init = {.role = TUSB_ROLE_DEVICE,
                                 .speed = TUSB_SPEED_AUTO};
  tusb_init(0, &dev_init); // initialize device stack on roothub port 0

  tusb_rhport_init_t host_init = {.role = TUSB_ROLE_HOST,
                                  .speed = TUSB_SPEED_AUTO};
  tusb_init(1, &host_init); // initialize host stack on roothub port 1

  while (1) {
    tud_task();
  }
}
void send_json(void) {
  cJSON *root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "status", "ok");
  char *json_str = cJSON_PrintUnformatted(root);

  if (tud_cdc_connected()) {
    tud_cdc_write(json_str, strlen(json_str));
    tud_cdc_write_flush();
  }

  cJSON_Delete(root);
  free(json_str);
}

void USB0_IRQHandler(void) { tusb_int_handler(0, true); }

void USB1_IRQHandler(void) { tusb_int_handler(1, true); }

#ifndef REPORTS_H
#define REPORTS_H

#include <stdint.h>

typedef struct {
  uint8_t rows;
  uint8_t cols;
} DeviceSizeReport;

typedef struct {
  uint8_t row;
  uint8_t col;
} DeviceSlotReport;

#endif


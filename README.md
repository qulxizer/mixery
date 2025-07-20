# Mixery

**Mixery** is minimal firmware for the Raspberry Pi Pico. It’s written in C using the [Pico SDK](https://github.com/raspberrypi/pico-sdk), with built-in TinyUSB support for USB HID.  

Supports hot-plugging buttons and rotary encoders—no reboot needed.

## Features

- [X] Written in C using the Pico SDK  
- [X] USB HID via TinyUSB (integrated in SDK)  
- [X] Hot-pluggable input support (buttons, rotary encoders)  

## Requirements

- Raspberry Pi Pico  
- Pico SDK with TinyUSB enabled  
- CMake + ARM GCC toolchain

## Build

```bash
git clone https://github.com/qulxizer/mixery
cd mixery
mkdir build && cd build
cmake ..
make

import hid
import struct

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0x4004
SET_REPORT_ID_SET_SLOT = 8

row = 3
col = 4
changed = 0  # using int to match uint8_t in C
device_type = 1

action_str = "test_action".encode("ascii")
action_padded = action_str.ljust(16, b"\0")

button_pin = 14
button_id = 1
button_pressed = 0  # using int to match uint8_t in C

button_bytes = struct.pack("<BBB", button_pin, button_id, button_pressed)

device_slot_data = struct.pack(
    "<BBBB16s4s",
    row,
    col,
    changed,
    device_type,
    action_padded,
    button_bytes,
)

report = bytes([SET_REPORT_ID_SET_SLOT]) + device_slot_data
print(f"Report length: {len(report)}")

try:
    dev = hid.Device(VENDOR_ID, PRODUCT_ID)
    print(f"Device opened: {dev.manufacturer}")
    ret = dev.send_feature_report(report)
    print(f"send_feature_report returned: {ret}")
except hid.HIDException as e:
    print(f"HID Error: {e}")

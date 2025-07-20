import hid
import struct

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0x4004
SET_REPORT_ID_SET_SLOT = 8  # your report ID

row = 3
col = 4
changed = 0
device_type = 2  # DEVICE_ENCODER

action_str = "test_action".encode("ascii")
action_padded = action_str.ljust(16, b"\0")

encoder_pinA = 26
encoder_pinB = 27
encoder_last_b_position = 0
encoder_id = 1
presentage = 0

encoder_bytes = struct.pack(
    "<BBBBB",
    encoder_pinA,
    encoder_pinB,
    encoder_last_b_position,
    encoder_id,
    presentage,
)

device_slot_data = struct.pack(
    "<BBBB16s5s",
    row,
    col,
    changed,
    device_type,
    action_padded,
    encoder_bytes,
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

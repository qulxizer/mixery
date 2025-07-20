import hid

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0x4004

with hid.Device(VENDOR_ID, PRODUCT_ID) as dev:
    response = dev.read(64)  # max length 64 bytes

    print(f"Received {len(response)} bytes:")
    print(response)

import hid

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0x4004
REPORT_ID = 7

rows = 3
cols = 4
payload = bytes([rows, cols])

report = bytes([REPORT_ID]) + payload

with hid.Device(VENDOR_ID, PRODUCT_ID) as dev:
    dev.send_feature_report(report)
    print(f"Sent SET_REPORT_ID_LAYOUT: rows={rows}, cols={cols}")

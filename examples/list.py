import hid

for d in hid.enumerate():
    print(
        f"VID: {hex(d['vendor_id'])}, PID: {hex(d['product_id'])}, Product: {d['product_string']}"
    )

# Test code only for testing of the sender devices!!

import asyncio
from bleak import BleakScanner

async def scan_ble():
    print("Scanning for BLE devices for 13 sec...")
    devices = await BleakScanner.discover(timeout=13.0)

    for device in devices:
        print(f"\n Device: {device.name} | Address: {device.address}")
        manufacturer = device.metadata.get("manufacturer_data",{})
        if manufacturer:
            for ad_type, data in manufacturer.items():
                try:
                    text = data.decode(errors="ignore")
                except:
                    text = str(data)
                print(f"Manufacturer Data (0x(ad_type:04X): {text}")
        else:
            print("No manufacturer data found :(.")


asyncio.run(scan_ble())
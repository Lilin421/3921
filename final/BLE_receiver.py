import asyncio

from winrt.windows.devices.bluetooth.advertisement import (
    BluetoothLEAdvertisementWatcher,
)


async def main():

    print("===================================")
    print(" BLE SCANNER - ALL DEVICES")
    print("===================================")
    print()
    print("Scanning ALL BLE advertisements...")
    print("Press Ctrl+C to stop.")
    print()

    watcher = BluetoothLEAdvertisementWatcher()

    count = 0

    def on_received(sender, args):

        nonlocal count
        count += 1

        print("=" * 60)

        print("Advertisement #", count)

        print("Bluetooth address:")
        print(hex(args.bluetooth_address))

        print("RSSI:")
        print(args.raw_signal_strength_in_dbm)

        print("Local name:")
        print(repr(args.advertisement.local_name))

        print("Service UUIDs:")
        for uuid in args.advertisement.service_uuids:
            print("   ", uuid)

        print()

    watcher.add_received(on_received)

    watcher.start()

    try:
        while True:
            await asyncio.sleep(1)

    except KeyboardInterrupt:
        print()
        print("Stopping...")

    finally:
        watcher.stop()


asyncio.run(main())
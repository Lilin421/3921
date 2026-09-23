import asyncio
import uuid

from winrt.windows.devices.bluetooth import BluetoothAdapter
from winrt.windows.devices.bluetooth.genericattributeprofile import (
    GattServiceProvider,
    GattServiceProviderAdvertisingParameters,
    GattLocalCharacteristicParameters,
    GattCharacteristicProperties,
    GattProtectionLevel,
)


SERVICE_UUID = uuid.UUID(
    "12345678-1234-5678-1234-56789abcdef0"
)

CHAR_UUID = uuid.UUID(
    "12345678-1234-5678-1234-56789abcdef1"
)


async def main():

    print("===================================")
    print(" BLE GATT SERVER")
    print("===================================")
    print()

    # -------------------------------------------------
    # 1. Bluetooth adapter
    # -------------------------------------------------

    adapter = await BluetoothAdapter.get_default_async()

    if adapter is None:
        print("No Bluetooth adapter found.")
        return

    print("Peripheral supported:",
          adapter.is_peripheral_role_supported)

    if not adapter.is_peripheral_role_supported:
        print("Peripheral role is NOT supported.")
        return

    print()

    # -------------------------------------------------
    # 2. Create GATT service
    # -------------------------------------------------

    print("Creating GATT service...")

    result = await GattServiceProvider.create_async(
        SERVICE_UUID
    )

    print("CreateAsync returned.")

    print("Error:", result.error)

    if result.service_provider is None:
        print("service_provider is None.")
        return

    service_provider = result.service_provider

    print("GATT service created.")
    print()

    # -------------------------------------------------
    # 3. Characteristic parameters
    # -------------------------------------------------

    parameters = GattLocalCharacteristicParameters()

    parameters.characteristic_properties = (
        GattCharacteristicProperties.READ
        | GattCharacteristicProperties.NOTIFY
    )

    parameters.read_protection_level = (
        GattProtectionLevel.PLAIN
    )

    parameters.write_protection_level = (
        GattProtectionLevel.PLAIN
    )

    # -------------------------------------------------
    # 4. Create characteristic
    # -------------------------------------------------

    print("Creating characteristic...")

    characteristic_result = (
        await service_provider.service.create_characteristic_async(
            CHAR_UUID,
            parameters
        )
    )

    print("Characteristic CreateAsync returned.")

    print("Error:", characteristic_result.error)

    if characteristic_result.characteristic is None:
        print("Characteristic creation failed.")
        return

    characteristic = characteristic_result.characteristic

    print("Characteristic created.")
    print()

    print("Service UUID:")
    print(SERVICE_UUID)

    print()

    print("Characteristic UUID:")
    print(CHAR_UUID)

    # -------------------------------------------------
    # 5. Start advertising
    # -------------------------------------------------

    advertising_parameters = (
        GattServiceProviderAdvertisingParameters()
    )



    print()
    print("Starting GATT advertising...")

    service_provider.start_advertising()

    print("GATT advertising started.")
    print()
    print("Service UUID:")
    print(SERVICE_UUID)
    print()
    print("Characteristic UUID:")
    print(CHAR_UUID)
    print()
    print("Waiting for PC B to connect...")

    print()
    print("Advertisement status:")
    print(service_provider.advertisement_status)

    print()
    print("Waiting for PC B...")
    print("Press Ctrl+C to stop.")

    try:

        while True:
            await asyncio.sleep(1)

    except KeyboardInterrupt:

        print()
        print("Stopping...")

    finally:

        service_provider.stop_advertising()


if __name__ == "__main__":
    asyncio.run(main())
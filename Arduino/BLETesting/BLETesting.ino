#include <ArduinoBLE.h> // For BLE Stuff


BLEService leftService("26548447-3cd0-4460-b683-43b332274c2b"); // LEFT HAND
// BLEService rightService("26548447-3cd0-4460-b683-43b332274c2b"); // RIGHT HAND

// Called once when sketch is loaded
void setup()
{
    Serial.begin(9600);
    // begin BLE
    if (!BLE.begin())
    {
        Serial.println("starting BLE failed");
        while (1)
            ;
    }

    BLE.setLocalName("Glove 1 (Left)");
    BLE.setAdvertisedService(theService);

    // Add data and heartbeat characteristic
    // theService.addCharacteristic(flexCharacteristic);
    BLE.addService(leftService);

    // start advertising
    BLE.advertise();
    Serial.println("Bluetooth device active, waiting for connections...");
}

void loop()
{
    BLEDevice central = BLE.central();
    // If central exists, BLE is connected
    if (central)
    {
        Serial.println("Connected to central device!");
    }
    else
    {
        Serial.println("Not connected!");
    }
    delay(500);
}
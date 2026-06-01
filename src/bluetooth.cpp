#include <BluetoothSerial.h>
#include <global.h>
#include <config.h>
#include <bluetooth.h>

BT::BT() {}

BluetoothSerial SerialBT;

bool BT::begin() {
    SerialBT.begin(Device_Name, true);

    bool connected = SerialBT.connect(bt_address);

    return connected;
}
#include <BluetoothSerial.h>
#include <global.h>
#include <config.h>
#include <bluetooth.h>

BluetoothSerial SerialBT;

bool BT::begin() {
    SerialBT.begin("Natascha Control", true);

    bool connected = SerialBT.connect(bt_address);

    return connected;
}
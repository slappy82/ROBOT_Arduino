/*
 * Campbell Maxwell
 * August 2020 - Present
 */

#include "BLE_Communication_Module.h"

/*
 * Constructor for BLE_Communication_Module class
 */
BLE_Communication_Module::BLE_Communication_Module() : BLE_STATE_PIN(4),
                                                       LEFT(0),
                                                       RIGHT(1) {                                                 
}
/*
 * Setup the HM-10 module to peripheral state to begin advertisement
 */
void BLE_Communication_Module::BLESetup() {
    // Initialize and set the baud rate for data transfer via UART
    btDevice.begin(9600);
    console.begin(9600);

    // Configure settings for HM-10 unit
    // Sends code to test bluetooth (also disconnect it from a device), if it works it should reply 'OK' 
    btDevice.print("AT");  
    console.println(btDevice.readString());  
    delay(2);
    // Return the MAC address of the adapter
    btDevice.print("AT+ADDR?"); 
    console.println(btDevice.readString());   
    delay(2); 
    // Activate notifications for gaining or losing a connection (OK+CONN or OK+LOST)
    btDevice.print("AT+NOTI1"); 
    console.println(btDevice.readString());   
    delay(2); 
    // Set the role of the module - peripheral
    btDevice.print("AT+ROLE0"); 
    console.println(btDevice.readString());   
    delay(2);  
    // Set the mode of the module - restrict commands until it is connected ONLY NEEDED FOR CENTRAL ROLE
    btDevice.print("AT+IMME0"); 
    console.println(btDevice.readString());   
    delay(2); 
    // Reset module to allow for changes to occur
    btDevice.print("AT+RESET"); 
    console.println(btDevice.readString());   
    delay(20);  
    
    // Setup the interrupt / status pin
    pinMode(BLE_STATE_PIN, INPUT);          // Needed if I wish to use an interrupt on connection (HIGH when connected, LOW otherwise)  
    
    // Initialise member variables
    packetType = 254;                       // Set to highest value to pass as true and go to default in baseAI switch case
    packetData[0] = 0;
    packetData[1] = 0;
}  
/*
 * Checks if a connection has been made to the HM-10 unit and sets / returns appropriate member statuses. Sets packetType to let base module send
 * packet data to the correct module. Copies packet data to the packetData variable.
 * @return - True if android has paired, FALSE if not
 */
bool BLE_Communication_Module::checkBLEConnection() {
    bool isConnected = BLE_Communication_Module::isBLEConnected();
    if (isConnected) {                                     
        uint8_t messageLength = btDevice.available();   
        if (messageLength == 2) {   
            byte newBytes[messageLength];
            btDevice.readBytes(newBytes, messageLength);
            //packetType = (uint8_t)newBytes[0];                    // correct call to set packet type variable
            //BLE_Communication_Module::decodeByte(newBytes[1])     // correct call to depackage data of packet ready for appropriate module                                                  
            packetData[0] = (uint8_t)newBytes[0];        // Used until packet protocol fixed on android end+++++++++
            packetData[1] = (uint8_t)newBytes[1];        // Used until packet protocol fixed on android end+++++++++
        } 
    }
    return isConnected;                              
}
/*
 * Method to compress 2 byte packet to a single byte (3 and 4 would become 0x34). Only needed until I fix android component to send 1 byte packet
 * This should be redundant when I fix the android packets - first byte as an opcode (bit switch for a data type etc) second for payload (maybe more
 * if needed later)
 * @param data - Byte array containing a full packet sent via BLE
 * @return - Uint8_t containing the data portion of the packet parameter
 */
byte BLE_Communication_Module::compressPacket(byte data[]) {
    return ((data[0]*16)+data[1]);
}
/*
 * Converts a byte into two bytes with four bits of original byte each. Eg: 0x34 becomes 0x03 and 0x04 in that order. 
 * @param data - The original 'packet' to be split into two bytes
 * @return - Uint8_t pointer pointing to a two element array containing the two new bytes
 */
void BLE_Communication_Module::decodeByte(byte data) {
    packetData[0] = (uint8_t)(data / 16);
    packetData[1] = (uint8_t)(0x0f & data); 
}
/*
 * Checks the state pin to see if pairing has been made with HM-10 unit
 * @return - Return TRUE if connected to android, FALSE if not
 */
bool BLE_Communication_Module::isBLEConnected() {
    if (digitalRead(BLE_STATE_PIN) == HIGH) {
        return true;
    }
    return false;
}
/*
 * Read only return for the packet type value of the last packet
 * @return - Uint8_t value stored in packetType variable - the type of the last packet
 */
uint8_t BLE_Communication_Module::getPacketType() {
    return packetType;
}
/*
 * Read only return for a portion of the packet data
 * @param - The index of the part of data to receive
 * @return - The data portion of the last packet's data
 */
uint8_t BLE_Communication_Module::getPacketData(uint8_t dataIndex) {
    return packetData[dataIndex];
}

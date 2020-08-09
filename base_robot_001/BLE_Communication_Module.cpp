/*
 * Campbell Maxwell
 * August 2020 - Present
 */

#include "BLE_Communication_Module.h"

/*
 * Constructor for BLE_Communication_Module class
 */
BLE_Communication_Module::BLE_Communication_Module() : BLE_STATE_PIN(20),
                                                       LEFT(0),
                                                       RIGHT(1) {
}
/*
 * Setup the HM-10 module to peripheral state to begin advertisement
 */
void BLE_Communication_Module::BLESetup() {
    // Setup the interrupt pin
    pinMode(BLE_STATE_PIN, INPUT);  // Only needed if I wish to use an interrupt

    // Initialize and set the baud rate for data transfer via UART
    btDevice.begin(9600);
    console.begin(9600);
    
    // Sends code to test bluetooth (also disconnect it from a device), if it works it should reply 'OK' 
    btDevice.print("AT");  
    console.println(btDevice.readString());   // receiving and printing the return code to console
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
    // Initialise member variables
    isConnected = false;
    currentCommand = 0;
    packetData[0] = 0;
    packetData[1] = 0;
}  
/*
 * Checks if a connection has been made to the HM-10 unit and sets / returns appropriate member statuses. Calls BLERemoteControl method if
 * connection has already been made to listen for and return the next packet
 * @return - Uint8_t containing the data portion of the last packet
 */
uint8_t BLE_Communication_Module::checkBLEConnection() {
    if (!isConnected) {                                     
        if (btDevice.available() > 0) {      
            String btStatus = btDevice.readString();
            if (btStatus.equals("OK+CONN")) {
                console.println(btStatus); 
                isConnected = true; 
                currentCommand = 1;
                return currentCommand;                                   // Set to non-zero to pass as true as listener call no longer blocks
            }
        }
        return currentCommand;
    }
    return BLE_Communication_Module::BLERemoteControl();
}
/*
 * Listens for input from android app then handles it accordingly, ideally making a movement call or returning to AI method
 * @return - Uint8_t containing the data portion of the last packet
 */
uint8_t BLE_Communication_Module::BLERemoteControl() {  
    uint8_t messageLength = btDevice.available();   
    //console.println("test connected");
    if (messageLength > 0) {
        // Check input for necessary flags  **could either halt for 0x44 or just continue regardless**
        if (messageLength == 2) {   
                byte newBytes[messageLength];
                btDevice.readBytes(newBytes, messageLength);                                                       
                currentCommand = BLE_Communication_Module::compressPacket(newBytes);                        // Return data compressed into 1 byte
            }
        else {
            String AT = btDevice.readString();
            console.println(AT);                                                // Print message 
            if (AT.equals("OK+LOST")) {                                         // Connection has been lost (ensure AT+NOTI is enabled - AT+NOTI1)
                isConnected = false;
                currentCommand = 0;
            }  
        }
    }
    return currentCommand;                                                         
}
/*
 * Method to compress 2 byte packet to a single byte (3 and 4 would become 0x34). Only needed until I fix android component to send 1 byte packet
 * This should be redundant when I fix the android packets - first byte as an opcode (bit switch for a data type etc) second for payload (maybe more
 * if needed later)
 * @param data - Byte array containing a full packet sent via BLE
 * @return - Uint8_t containing the data portion of the packet parameter
 */
uint8_t BLE_Communication_Module::compressPacket(byte data[]) {
    return ((data[0]*16)+data[1]);
}
/*
 * Converts a byte into two bytes with four bits of original byte each. Eg: 0x34 becomes 0x03 and 0x04 in that order. 
 * @param data - The original 'packet' to be split into two bytes
 * @return - Uint8_t pointer pointing to a two element array containing the two new bytes
 */
void BLE_Communication_Module::decodeByte(uint8_t data) {
    packetData[0] = data / 16;
    packetData[1] = 0x0f & data; 
}

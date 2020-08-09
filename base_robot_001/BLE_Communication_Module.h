/*
 * Campbell Maxwell
 * August 2020 - Present
 * Bluetooth low energy functionality for communication between android and robot.
 */

 #ifndef BLE_COMMUNICATION_MODULE_H
 #define BLE_COMMUNICATION_MODULE_H
 
 #include <Arduino.h>

 #define console Serial                                     // Used to communicate with the arduino console
 #define btDevice Serial1                                   // Used to communicate with the HM-10 device

 class BLE_Communication_Module {
    private:
    // PINS
        const uint8_t BLE_STATE_PIN;                        // Determine state of HM-10 eg: if there are bytes to be read
    // VARIABLES
        bool isConnected;                                   // Set to true if android is connected to this module
        uint8_t currentCommand;                             // Contains the last sent packet data
    // FUNCTIONS
        uint8_t BLERemoteControl();                         // Used to return the data portion of latest packet (called by checkBLEConnection)
        uint8_t compressPacket(byte[]);                     // Compress current packet into future format, will be redundant soon
 
    public:
    // VARIABLES
        const uint8_t LEFT;
        const uint8_t RIGHT;
        uint8_t packetData[2];
    // FUNCTIONS
        BLE_Communication_Module();                         // Constructor will initialise the pins and handle any initial setup required for this class
        /*
         * Called to setup this module
         */
        void BLESetup();                                   
        /*
         * Checks if a connection has been made to the HM-10 unit and sets / returns appropriate member statuses. Calls BLERemoteControl method if
         * connection has already been made to listen for and return the next packet
         * @return - Returns the data portion of the last packet
         */
        uint8_t checkBLEConnection();
        /*
         * Converts a byte into two bytes with four bits of original byte each. Eg: 0x34 becomes 0x03 and 0x04 in that order. 
         * @param - The original 'packet' to be split into two bytes
         * @return - Uint8_t pointer pointing to a two element array containing the two new bytes
         */
        void decodeByte(uint8_t);
 };

 #endif

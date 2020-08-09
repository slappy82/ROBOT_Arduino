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
        uint8_t packetType;                                 // Variable containing a value to determine the module for packet data
        uint8_t packetData[2];                              // Variable containing packet data of last packet
    // FUNCTIONS
        bool isBLEConnected();                              // Checks the state pin to see if pairing has been made with HM-10 unit
        byte compressPacket(byte[]);                        // Compress current packet into future format, will be redundant soon
        void decodeByte(byte);                              // Converts a byte into two bytes with four bits of original byte each. Eg: 0x34 becomes 0x03 and 0x04 in that order. 
 
    public:
    // VARIABLES
        const uint8_t LEFT;                                 // Constant variable indicating index for LEFT motor data
        const uint8_t RIGHT;                                // Constant variable indicating index for RIGHT motor data
        static const uint8_t MOVEMENT_MODULE = 1;           // Static constant to check against packet types (data meant for movement module)
        static const uint8_t DISABLE_ANDROID = 0;           // Static constant to check against packet types (if android toggles control switch)
    // FUNCTIONS
        /*
         * Constructor will initialise the pins and handle any initial setup required for this class
         */
        BLE_Communication_Module();                       
        /*
         * Called to setup this module
         */
        void BLESetup();                               
        /*
         * Checks if a connection has been made to the HM-10 unit and sets / returns appropriate member statuses. Sets packetType to let base module send
         * packet data to the correct module. Copies packet data to the packetData variable.
         * @return - True if android has paired, FALSE if not
         */
        bool checkBLEConnection();
        /*
         * Read only return for the packet type value of the last packet
         * @return - Uint8_t value stored in packetType variable - the type of the last packet
         */
        uint8_t getPacketType();
        /*
         * Read only return for a portion of the packet data
         * @param - The index of the part of data to receive
         * @return - The data portion of the last packet's data
         */
        uint8_t getPacketData(uint8_t);

 };

 #endif

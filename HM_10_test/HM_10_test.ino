

#define console Serial
#define btDevice Serial1

const uint8_t BLE_STATE_PIN = 2;


void BLESetup() {
    // Initialize and set the baud rate for data transfer via UART
    btDevice.begin(9600);
    console.begin(9600);
    pinMode(BLE_STATE_PIN, INPUT);
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
}

void checkBLEConnection() {
    if (digitalRead(BLE_STATE_PIN) == HIGH) {                                     
        uint8_t messageLength = btDevice.available();   
        if (messageLength > 0) {                                        // Putting this here to reduce impact (more likely no new packet sent)
            if (messageLength >= 2) {   
                byte newBytes[messageLength];
                btDevice.readBytes(newBytes, messageLength);
                console.println(messageLength);
                console.println(newBytes[0]);
                console.println(newBytes[1]);
            }
            /*else {
                console.println(btDevice.readString());                 // Print message to console 
            }*/
        }  
    }                            
}

void setup() {
    BLESetup();
}

void loop() {
    checkBLEConnection();
    delay(500);
}

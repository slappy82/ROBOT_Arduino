/*
 * Campbell Maxwell
 * 2020
 */

const uint8_t SET_CONFIG_ON = 0;            // 0x00 for Exit, 0x01 for Enter Config mode
const uint8_t GET_READ = 1;                 // Get a data packet from LiDAR containing distance and strength values (manual mode)
const uint8_t SET_MODE_AUTO = 2;            // 0x00 for Manual, 0x01 for Automatic
const uint8_t SET_BAUD = 3;                 // Range 0x00 - 0x0c gives range 9600 - 512000 check the chart on datasheet
const uint8_t SET_UNITS_CM = 4;             // 0x00 for mm, 0x01 for cm
const uint8_t SET_RESET = 5;                // Reset the lidar module settings
const uint8_t ON = 1;                       // Used with toggle type settings
const uint8_t OFF = 0;                      // Used with toggle type settings

byte *lastPacket = new byte[9];                         // Packet used to receive data from the lidar module

#define console Serial
#define lidar Serial2

//void packetMode(uint8_t select, byte value); 

void setup() {   
    lidar.begin(115200, SERIAL_8N1);
    console.begin(9600);
    lidar.setTimeout(5);

    packetMode(SET_CONFIG_ON, ON);
    delay(2);
    //packetMode(SET_BAUD, getBAUDCode(115200));
    //delay(2);
    packetMode(SET_MODE_AUTO, ON);
    delay(2);
    packetMode(SET_UNITS_CM, OFF);
    delay(2);
    packetMode(SET_CONFIG_ON, OFF);
    delay(2);
    console.println("Starting...");
    delay(2);
}

void loop() {
    manualData();
}

/**
 *  Waits for any input from console then takes a distance reading from lidar and decodes packet and sends data to console
 */
void manualData() {
    const byte MAX_BUFFER_SIZE = 64;
    const byte DATA_PACKET_SIZE = 9;
    //delay(10);
    if (console.available() > 0) {                                                               // Clear the UART buffer
        //packetMode(GET_READ, ON);   
        byte inBufferSize = lidar.available();     
        if (inBufferSize >= DATA_PACKET_SIZE) {
            // Stuck on auto, remove the head of queue   
            lidar.readString();
            /*byte discardData[MAX_BUFFER_SIZE-DATA_PACKET_SIZE];
            console.print(lidar.readBytes(discardData, inBufferSize - DATA_PACKET_SIZE));*/
            byte dataPacket[DATA_PACKET_SIZE];
            lidar.readBytes(dataPacket, DATA_PACKET_SIZE);
            if (dataPacket[0] == 0x59 && dataPacket[1] == 0x59) {                                       // Check for correct packet format            
                if (lastPacket[2] == dataPacket[2] && lastPacket[3] == dataPacket[3]) {                 // TODO: Checksum check
                    uint16_t distance = byteToShort(dataPacket[3], dataPacket[2]);
                    uint16_t strength = byteToShort(dataPacket[5], dataPacket[4]);
                    console.print("Distance: ");
                    console.print(distance);
                    console.print("    Strength: ");
                    console.println(strength);
                    console.readString();    // Clear the console buffer  
                } 
                lastPacket = dataPacket;
            }
        }
    }    
}

void autoData() {
    
}

uint16_t byteToShort(uint8_t high, uint8_t low) {
    uint16_t out = high * 256;
    out += low;
    return out;
}

void packetMode(uint8_t select, byte value) {
    switch (select) {
        case 0:
            packetSetup(0x42, 0x57, 0x02, 00, 00, 00, value, 0x02);
            break;
        case 1:
            packetSetup(0x42, 0x57, 0x02, 00, 00, 00, 00, 0x41); 
            break;
        case 2:
            packetSetup(0x42, 0x57, 0x02, 00, 00, 00, value, 0x40);
            break;
        case 3:
            packetSetup(0x42, 0x57, 0x02, 00, 00, 00, value, 0x08);
            break;
        case 4:
            packetSetup(0x42, 0x57, 0x02, 0x00, 0x00, 0x00, value, 0x1A);
            break;  
        case 5:
            packetSetup(0x42, 0x57, 0x02, 00, 0xFF, 0xFF, 0xFF, 0xFF);
            break;
        default:
            break;
    }
}


void packetSetup(byte zero, byte one, byte two, byte three, byte four, byte five, byte six, byte seven) {
    byte packet[8] = {zero, one, two, three, four, five, six, seven};
    lidar.write(packet, 8);
    delay(2);
}

byte getBAUDCode(int baud) {
    switch(baud) {
        case 9600:
            return 0x00;
        case 14400:
            return 0x01;
        case 19200:
            return 0x02;
        case 38400:
            return 0x03;
        case 56000:
            return 0x04;
        case 57600:
            return 0x05;
        case 115200:
            return 0x06;
        case 128000:
            return 0x07;
        case 230400:
            return 0x08;
        case 256000:
            return 0x09;
        case 460800:
            return 0x0a;
        case 500000:
            return 0x0b; 
        case 512000:
            return 0x0c;
        default:
            return 0x06;                      
    }
}

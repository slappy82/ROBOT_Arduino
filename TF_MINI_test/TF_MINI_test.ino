/*
 * Campbell Maxwell
 * 2020
 */

const uint8_t CONFIG_TOGGLE = 0;        // 0x00 for Exit, 0x01 for Enter Config mode
const uint8_t GET_READ = 1;
const uint8_t MANUAL_TOGGLE = 2;        // 0x00 for Manual, 0x01 for Automatic
const uint8_t SET_BAUD = 3;             // Range 0x00 - 0x0c gives range 9600 - 512000 check the chart on datasheet
const uint8_t SET_UNITS = 4;            // 0x00 for mm, 0x01 for cm
const uint8_t RESET = 5;

byte packet[8];
byte data[9];

#define console Serial
#define lidar Serial2

void setup() {
    lidar.begin(115200, SERIAL_8N1);
    console.begin(9600);
    packetMode(CONFIG_TOGGLE, 1);
    packetMode(RESET, 0);
    //packetMode(SET_BAUD, 6);
    //packetMode(MANUAL_TOGGLE, 1);
    packetMode(SET_UNITS, 0);
    //packetMode(GET_READ, 0);
    packetMode(CONFIG_TOGGLE, 0);
    console.println("Starting...");
    //packetMode(GET_READ, 0);
    
}

void loop() {
    console.println(lidar.available());
    displayBytes();
    delay(100);
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

void displayBytes() {
    if (lidar.available() >= 9) {
        lidar.readBytes(data, 9);
        for (int i = 0; i < 9; i++) {
            console.print(data[i]);
            console.print(" ");
        }
        console.println();
    }
}

void packetSetup(byte zero, byte one, byte two, byte three, byte four, byte five, byte six, byte seven) {
    packet[0] = zero;
    packet[1] = one;
    packet[2] = two;
    packet[3] = three;
    packet[4] = four;
    packet[5] = five;
    packet[6] = six;
    packet[7] = seven;
    lidar.write(packet, 8);
    delay(2);
    displayBytes();
    delay(2);
}

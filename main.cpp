#include "mbed.h"
#include "arm_book_lib.h"

DigitalIn driverSeatOccSensor(D7);
DigitalIn passengerSeatOccSensor(D6);
DigitalIn driverSeatbeltSensor(D5);
DigitalIn passengerSeatbeltSensor(D4);
DigitalIn ignition(D3);

DigitalInOut alarm(PE_10);

DigitalOut greenIndicator(LED1);
DigitalOut blueIndicator(LED2);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

bool driverSatDown = false;
bool lockedOut = false;

void inputsInit();
void outputsInit();
void displayWelcome();

void checkSafety();
void ignitionCheck();
void soundAlarm();

// main() runs in its own thread in the OS
int main()
{

    inputsInit();
    outputsInit();

    while (true) {
        displayWelcome();
        checkSafety();
        ignitionCheck();
        soundAlarm();
    }
}


void inputsInit() {
    driverSeatOccSensor.mode(PullDown);
    passengerSeatOccSensor.mode(PullDown);
    driverSeatbeltSensor.mode(PullDown);
    passengerSeatbeltSensor.mode(PullDown);
    ignition.mode(PullDown);
    alarm.mode(OpenDrain);
    alarm.input();
}

void outputsInit() {
    greenIndicator = OFF;
    blueIndicator = OFF;
}

void displayWelcome() {
    if(driverSeatOccSensor && !driverSatDown) {
        uartUsb.write("Welcome to enhanced alarm system model 218-W24\r\n", 48);
        driverSatDown = true;
    }
}

void checkSafety() {
    if(driverSeatOccSensor && driverSeatbeltSensor && passengerSeatOccSensor && passengerSeatbeltSensor) {
        greenIndicator = ON;
    } else {
        greenIndicator = OFF;
    }
}

void ignitionCheck() {
    if(ignition && !lockedOut) {
        if(greenIndicator) {
            greenIndicator = OFF;
            blueIndicator = ON;
            uartUsb.write("Engine started.\r\n", 17);
        } else {
            lockedOut = true;
            
            uartUsb.write("Ignition inhibited\r\n", 20);

            if(!driverSeatOccSensor) {
                uartUsb.write("Driver seat not occupied.\r\n", 27);
            }

            if(!passengerSeatOccSensor) {
                uartUsb.write("Passenger seat not occupied.\r\n", 30);
            }

            if(!driverSeatbeltSensor) {
                uartUsb.write("Driver seatbelt not fastened.\r\n", 31);
            }
            
            if(!passengerSeatbeltSensor) {
                uartUsb.write("Passenger seatbelt not fastened.\r\n", 34);
            }
        }
    }
    
}

void soundAlarm() {
    if(lockedOut) {
        alarm.output();
        alarm = LOW;
    }
}
/* Embed libraries */
#include "mbed.h"
#include "arm_book_lib.h"

/* Declare inputs & outputs */
DigitalIn driverSeatOccSensor(D7);
DigitalIn passengerSeatOccSensor(D6);
DigitalIn driverSeatbeltSensor(D5);
DigitalIn passengerSeatbeltSensor(D4);
DigitalIn ignition(D3);

DigitalInOut alarm(PE_10);

DigitalOut greenIndicator(LED1);
DigitalOut blueIndicator(LED2);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

/* Declare global variables and functions */
bool driverSatDown = false;
bool lockedOut = false;
bool engineOn = false;

void inputsInit();
void outputsInit();
void displayWelcome();

void checkSafety();
void ignitionCheck();
void soundAlarm();

// Create logic for program
int main()
{

    // initialize inputs & outputs
    inputsInit();
    outputsInit();

    // continually run logic
    while (true) {
        displayWelcome();
        checkSafety();
        ignitionCheck();
        soundAlarm();
    }
}

/* Function implementation to initialize all inputs */
void inputsInit() {
    driverSeatOccSensor.mode(PullDown);
    passengerSeatOccSensor.mode(PullDown);
    driverSeatbeltSensor.mode(PullDown);
    passengerSeatbeltSensor.mode(PullDown);
    ignition.mode(PullDown);
    alarm.mode(OpenDrain);
    alarm.input();
}

/* Function implementation to initialize all outputs */
void outputsInit() {
    greenIndicator = OFF;
    blueIndicator = OFF;
}

/* Function implementation to decide whether or not to display the welcome message */
void displayWelcome() {
    if(driverSeatOccSensor && !driverSatDown) {
        uartUsb.write("Welcome to enhanced alarm system model 218-W24\r\n", 48);
        driverSatDown = true;
    }
}

/* Function implementation to turn on the Green LED (or not) based on sensor data */
void checkSafety() {
    if(driverSeatOccSensor && driverSeatbeltSensor && passengerSeatOccSensor && passengerSeatbeltSensor && !engineOn) {
        greenIndicator = ON;
    } else {
        greenIndicator = OFF;
    }
}

/* Function implementation to check if car can start (or not) and display appropriate messages */
void ignitionCheck() {

    // if ignition is pressed and we are not locked out and the engine isn't already on
    if(ignition && !lockedOut && !engineOn) {

        // if our green safety indicator is lit
        if(greenIndicator) {
            // turn on engine
            greenIndicator = OFF;
            blueIndicator = ON;
            engineOn = ON;
            uartUsb.write("Engine started.\r\n", 17);
        } else {
            // lock user out and display why
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

/* Function implementation to check if alarm should sound or not */
void soundAlarm() {
    if(lockedOut) {
        alarm.output();
        alarm = LOW;
    }
}
#include <Arduino.h> 
#include <TimerOne.h> 
#include "CInCAN.h"

//Transceiver IO
#define PIN_RX 2 //Rx from transceiver 
#define PIN_TX 3 //Tx from transceiver

void setup() {
    Serial.begin(9600);

    //Initializing Arduino IOs   
    pinMode(PIN_RX, INPUT);
    pinMode(PIN_TX, OUTPUT);

    //Initializing Bit Timing Logic 
    BTL_init();
    //Initializing Can Controller
    CCL_init();

    //Initializing Timer1 Interrupt
    Timer1.initialize(BTL_TIME_QUANTA);

    //Attaching Interrupts
    //call BTLsync when a falling edge is detected at PIN_RX
    attachInterrupt(digitalPinToInterrupt(PIN_RX), BTL_edge_detector, FALLING);
    Timer1.attachInterrupt(BTL_new_time_quanta);

}

void loop() {
    BTL_sm();
}


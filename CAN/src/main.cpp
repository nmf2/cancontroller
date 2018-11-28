#include <Arduino.h> 
#include <TimerOne.h> 
#include "CInCAN.h"


void setup() {
    Serial.begin(9600);

    //Initializing Arduino IOs   
    pinMode(PIN_RX, INPUT);
    pinMode(PIN_TX, OUTPUT);

    //Initializing Bit Timing Logic 
    BTL_init();
    //Initializing Can Controller
    CCL_init();
    //Initializing TOGGLE
    TOGGLE_init();

    //Initializing Timer1 Interrupt
    Timer1.initialize(BTL_TIME_QUANTA);

    //Attaching Interrupts
    //call BTL_edge_detector when a falling edge is detected at PIN_RX
    //BTL_edge_detector: sets a flag called rx_falling which is used by
    //the bit timing state machine to handle synchronizations
    attachInterrupt(digitalPinToInterrupt(PIN_RX), BTL_edge_detector, FALLING);
    //call BTL_new_time_quanta after Timer1 trigger its interruption.
    //BTL_new_time_quanta: sets a flag called tq_flag which refreshes the
    //bit timing state machine
    Timer1.attachInterrupt(BTL_new_time_quanta);

}

void loop() {
    //TOGGLE_write();
    //TOGGLE_write_serial(); //Ctrl+Shift+L to see in Serial Plotter.
                           //Must comment other Serial prints
                           //CInCAN.cpp line 54,59,86,92,197
    BTL_sm(); //runs the Bit Timing Logic State Machine
    CCL_sm();
    //TOGGLE_print();
}

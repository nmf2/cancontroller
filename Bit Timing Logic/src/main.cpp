#include <Arduino.h> 
#include <TimerOne.h> 
#include "BTL.h"


//############################ INSTRUCTIONS ###################################
//##                                                                         ##
//##  -> Parameters,Flags,Variables and Functions with the prefixes BTL,CCL  ##
//##    TOGGLE are defined in the files BTL.h and BTL.cpp              ##
//##                                                                         ##           
//##  -> BTL represents Bit Timing Logic.                                    ##                                                                      ## 
//##                                                                         ##
//##  -> CCL represents CAN Controller.                                      ##
//##                                                                         ##
//##  -> Parameters,Flags,Variables and Functions with the prefix Toggle     ## 
//##     are part of the project evaluation. They are not important to the   ##
//##     CAN protocol.                                                       ##
//##                                                                         ## 
//##  -> Time Quantum Monitoring: Uncomment line 59 (BTL.cpp).            ##
//##                                                                         ##
//##  -> Rx Falling Edge Monitoring: Uncomment line 54 (BTL.cpp).         ##
//##                                                                         ##
//##  -> Hard Sync Monitoring: Uncomennt line 86 (BTL.cpp).               ##
//##                                                                         ##
//##  -> Resync Monitoring: Uncomment line 92 (BTL.cpp).                  ##
//##                                                                         ##
//##  -> BTL State Machine Monitoring: Uncomment line 197 (BTL.cpp).      ##
//##                                                                         ##
//##  -> Initializing TOGGLE: Uncomment line 55 (main.cpp).                  ##    
//##                                                                         ##
//##  -> Monitoring TOGGLE vector: Uncomment line 198 (BTL.cpp).          ##      
//##        via IOs:    Uncomment line 73 (main.cpp).                        ##
//##        via Serial: Uncomment line 74 (main.cpp).                        ##
//##        *NOTE: monitor via Serial Plotter, uncomment any other use of    ##
//##               Serial port. (on Arduino IDE).                            ##
//##                                                                         ##
//#############################################################################

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
    TOGGLE_write();
    //TOGGLE_write_serial(); //Ctrl+Shift+L to see in Serial Plotter.
                           //Must comment other Serial prints
                           //BTL.cpp line 54,59,86,92,197
    BTL_sm(); //runs the Bit Timing Logic State Machine
}

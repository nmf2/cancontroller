#include <Arduino.h>
#include <TimerOne.h>

//Transceiver IO
#define PIN_RX 2 //Rx from transceiver 
#define PIN_TX 3 //Tx from transceiver

//Bit Timing Logic Parameters
#define TIME_QUANTA 1000000 //tq of 1s 
#define SJW 4
#define SYNC_SEG_SIZE 1
#define PROP_SEG_SIZE 1
#define PHASE1_SEG_SIZE 1
#define SEG1_SIZE 8 //PROP_SEG_SIZE + PHASE1_SEG_SIZE
#define PHASE2_SEG_SIZE 7
#define SEG2_SIZE 7 //PHASE2_SEG_SIZE
#define BIT_NOMINAL_SIZE 16 //SYNC_SIZE + SEG1_SIZE + SEG2_SIZE
#define BTL_STATE_SYNC 0
#define BTL_STATE_SEG1 1
#define BTL_STATE_PESTATE1 2
#define BTL_STATE_SEG2 3
#define BTL_STATE_PESTATE2 4



void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
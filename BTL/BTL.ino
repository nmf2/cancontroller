#include <TimerOne.h>

//Transceiver IO
#define PIN_RX 2 //Rx from transceiver 
#define PIN_TX 3 //Tx from transceiver

//Bit Timing Logic Parameters
#define TIME_QUANTA 1000000 //tq of 1000000 microseconds -> 1s (for testing reasons)
#define SJW 1
#define SYNC_SEG_SIZE 1
#define PROP_SEG_SIZE 1
#define PHASE1_SEG_SIZE 1
#define SEG1_SIZE 8 //PROP_SEG_SIZE + PHASE1_SEG_SIZE
#define PHASE2_SEG_SIZE 7
#define SEG2_SIZE 7 //PHASE2_SEG_SIZE
#define BIT_NOMINAL_SIZE 16 //SYNC_SIZE + SEG1_SIZE + SEG2_SIZE
#define STATE_SYNC 0
#define STATE_SEG1 1
#define STATE_PESTATE1 2
#define STATE_SEG2 3
#define STATE_PESTATE2 4

//Bit Timing Logic Variables
int last_btl_state; //this variable stores the state of the current state, it may be usefull for the next iteration
int btl_state; //this variable stores the state of the next iteration
int tq_cnt; //this variable counts the amount of time quanta since the beginning of Sync Segment.
int tq_cnt_seg1; //this variable counts the amount of time quanta since the beginning of Propagation Segment. 
int tq_cnt_seg2; //this variable counts the amount of time quanta since the beginning of Phase Buffer Segment 2.
int phase_error; //this variable is used to calculate the current phase error of bit timing logic.
int resize; //this variable is used to calculate the bit timing adjust for resychronization due to any phase_error.
bool samplePoint; //this variable is used to indicate when the sample point happened.
bool writingPoint; //this variable is used to indicate when the writing point happened.
bool hardSync; //this variable is used to indicate a hard synchronization.
bool resync; //this variable is sued to indicate a resynchronization (if the falling edge have happened out of sync segment)
(
//Bit Timing Logic Functions
void BTLinit(); //Initialize the variables of Bit Timing Logic
void BTLogic(); //Run the Bit Timing Logic state machine, it is refreshed every time quanta.
void BTLsync(); //Set the resync or hardsync flag, it is triggered when a falling edge is detected at PIN_RX 

//CAN Controller Variables
bool idle_bus; //Indicate if the CAN bus is idle or not

//CAN Controller Functions
void CCLinit(); //Initialize the variables of CAN Controller

void setup() {
  //Initializing Arduino IOs
  pinMode(PIN_RX,INPUT);
  pinMode(PIN_TX,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_RX),BTLsync,FALLING);//call BTLsync when a falling edge is detected at PIN_RX


  //Initializing Bit Timing Logic variables
  BTLinit();
  Timer1.initialize(TIME_QUANTA); //interruption using timer1 every 0.1ms
  Timer1.attachInterrupt( BTLogic );
}

void loop() {
  // put your main code here, to run repeatedly:

}

void BTLinit(){
  last_btl_state = STATE_SYNC;
  btl_state = STATE_SEG1;
  tq_cnt = 0;
  tq_cnt_seg1 = 0;
  tq_cnt_seg2 = 0;
  phase_error = 0;
  resize = 0;
  samplePoint = false;
  writingPoint = false;
  hardSync = false;
  resync = false;
}

void BTLogic(){
  //Syncing Logic
  if(hardSync){
    last_btl_state = STATE_SYNC;
    btl_state = STATE_SEG1;
    //hardSync = false;
  }
  else if(resync && (btl_state == STATE_SEG1)){
    last_btl_state = STATE_SEG1;
    btl_state = STATE_PESTATE1;
    //resync = false;
  }
  else if(resync && (btl_state == STATE_SEG2)){
    last_btl_state = STATE_SEG2;
    btl_state = STATE_PESTATE2;
    //resync = false;
  }
  else if(resync)
    resync = false; 

  //BTL - State Machine
  switch(btl_state){
    case STATE_SYNC:
      writingPoint = true;
      tq_cnt = 0;
      last_btl_state = STATE_SYNC;
      btl_state = STATE_SEG1;
      break;

    case STATE_SEG1:
      if(hardSync){
        tq_cnt = 0;
        hardSync = false;
      }
      if(last_btl_state == STATE_SYNC){
        writingPoint = false;
        resize = 0;
        tq_cnt_seg1 = 0;
        tq_cnt++;
        last_btl_state = STATE_SEG1;
      }
      else{
        tq_cnt_seg1++;
        tq_cnt++;
      }
      phase_error = tq_cnt;
      if(tq_cnt_seg1 >= (SEG1_SIZE - 1)){
        btl_state = STATE_SEG2;
      }
      break;

    case STATE_PESTATE1:
      if(resync)
        resync = false;
      if(last_btl_state == STATE_SEG1){
        resize = min(SJW,phase_error);
        last_btl_state = STATE_PESTATE1;
      }
      tq_cnt_seg1++;
      tq_cnt++;
      if(tq_cnt_seg1 >= (SEG1_SIZE + resize - 1))
        btl_state = STATE_SEG2;
      break;

    case STATE_SEG2:
      if((last_btl_state == STATE_SEG1) || (last_btl_state == STATE_PESTATE1)){
        samplePoint = true;
        tq_cnt_seg2 = 0;
        tq_cnt++;
        last_btl_state = STATE_SEG2;
      }
      else{
        if(tq_cnt_seg2 == 1)
          samplePoint = false;
        tq_cnt_seg2++;
        tq_cnt++;
      }
      phase_error = BIT_NOMINAL_SIZE - (tq_cnt + resize);
      if(tq_cnt_seg2 >= (SEG2_SIZE - 1)){
        btl_state = STATE_SYNC;
      }
      break;

    case STATE_PESTATE2:
      if(resync)
        resync = false;
      if(last_btl_state == STATE_SEG2){
        resize = max(-SJW,phase_error);
        if(tq_cnt_seg2 == 1)
          samplePoint = false;
        last_btl_state = STATE_PESTATE2;
      }
      tq_cnt_seg2++;
      tq_cnt++;
      if((tq_cnt_seg2 >= (SEG2_SIZE + resize - 1)) && (phase_error >= SJW)){
        writingPoint = true;
        tq_cnt = 0;
        last_btl_state = STATE_SYNC;
        btl_state = STATE_SEG1;
      }
      else if((tq_cnt_seg2 >= (SEG2_SIZE + resize - 1)) && (phase_error < SJW)){
        btl_state = STATE_SYNC;
      }
      break;

  }
}

void BTLsync(){
  if(idle_bus){
    hardSync = true;
    writingPoint = true;
  }
  else
    resync = true;
}

void CCLinit(){
  idle_bus = true;
}
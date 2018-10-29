#include <TimerOne.h>

//Transceiver IO
#define PIN_RX 2
#define PIN_TX 3

//Bit Timing Logic Parameters
#define TIME_QUANTA 100000 //tq of 100000 microseconds -> 100ms (for testing reasons)
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
int btl_state;
int tq_cnt;
int tq_cnt_seg1;
int tq_cnt_seg2;
bool samplePoint;
bool writingPoint;
bool hardSync;
bool resync;

//Bit Timing Logic Functions
void BTLinit();
void BTLogic();
void BTLsync();

//CAN Controller Variables
bool idle_bus;

//CAN Controller Functions
void CCLinit();

void setup() {
  //Initializing Arduino IOs
  pinMode(PIN_RX,INPUT);
  pinMode(PIN_TX,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_RX),BTLsync,FALLING);//call BTLsync when a falling edge is detect at PIN_RX


  //Initializing Bit Timing Logic variables
  BTLinit();
  Timer1.initialize(TIME_QUANTA); //interruption using timer1 every 0.1ms
  Timer1.attachInterrupt( BTLogic );

}

void loop() {
  // put your main code here, to run repeatedly:

}

void BTLinit(){
  btl_state = STATE_SEG1;
  tq_cnt = 0;
  tq_cnt_seg1 = 0;
  tq_cnt_seg2 = 0;
  samplePoint = false;
  writingPoint = false;
  hardSync = false;
  resync = false;
}

void BTLogic(){
  //Syncing Logic
  if(hardSync){
    btl_state = STATE_SEG1;
    hardSync = false;
  }
  else if(resync && (btl_state == STATE_SEG1)){
    btl_state = STATE_PESTATE1;
    resync = false;
  }
  else if(resync && (btl_state == STATE_SEG2)){
    btl_state = STATE_PESTATE2;
    resync = false;
  }
  //States Logics -- INCOMPLETE
  if(btl_state == STATE_SYNC){

  }
  else if(btl_state == STATE_SEG1){

  }
  else if(btl_state == STATE_PESTATE1){

  }
  else if(btl_state == STATE_SEG2){

  }
  else if(btl_state == STATE_PESTATE2){
    
  }
}

void BTLsync(){
  if(idle_bus)
    hardSync = true;
  else
    resync = true;
}

void CCLinit(){
  idle_bus = true;
}
#include <TimerOne.h>

//Transceiver IO
#define PIN_RX 2 //Rx from transceiver 
#define PIN_TX 3 //Tx from transceiver

//LEDs
#define LED_PHASE1 8 //blue
#define LED_PHASE2 9 //red 
#define ON HIGH
#define OFF LOW

//Bit Timing Logic Parameters
#define TIME_QUANTA 1000000 //tq of 100000 microseconds -> .1s (for testing reasons)
#define SJW 4
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
bool sample_point; //this variable is used to indicate when the sample point happened.
bool writing_point; //this variable is used to indicate when the writing point happened.
bool hard_sync; //this variable is used to indicate a hard synchronization.
bool resync; //this variable is sued to indicate a resynchronization (if the falling edge have happened out of sync segment)

//Bit Timing Logic Functions
void BTLinit(); //Initialize the variables of Bit Timing Logic
void BTLogic(); //Run the Bit Timing Logic state machine, it is refreshed every time quanta.
void BTLsync(); //Set the resync or hard_sync flag, it is triggered when a falling edge is detected at PIN_RX 

//CAN Controller Variables
bool idle_bus = false; //Indicate if the CAN bus is idle or not

//CAN Controller Functions
void CCLinit(); //Initialize the variables of CAN Controller

void setup() {
  Serial.begin(9600);
  //Initializing Arduino IOs
  pinMode(LED_PHASE1,OUTPUT);
  pinMode(LED_PHASE2,OUTPUT);
  pinMode(PIN_RX,INPUT);
  pinMode(PIN_TX,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_RX),BTLsync,FALLING);//call BTLsync when a falling edge is detected at PIN_RX

  //
  //CCLinit();

  //Initializing Bit Timing Logic variables
  BTLinit();
  Timer1.initialize(TIME_QUANTA); //interruption using timer1 every time quanta
  Timer1.attachInterrupt( BTLogic );
}

// void loop() {
//   // put your main code here, to run repeatedly:
//   //Serial.println(btl_state);
// }

void BTLinit(){
  last_btl_state = STATE_SYNC; 
  btl_state = STATE_SEG1;
  tq_cnt = 0;
  tq_cnt_seg1 = 0;
  tq_cnt_seg2 = 0;
  phase_error = 0;
  resize = 0;
  sample_point = false;
  writing_point = false;
  hard_sync = false;
  resync = false;
}

void BTLogic(){
  //Syncing Logic
  if(hard_sync){
    // Set last state to SYNC simulating that the bit has just started.
    last_btl_state = STATE_SYNC;
    btl_state = STATE_SEG1;
  }
  else if(resync && (btl_state == STATE_SEG1)){
    // If a resync happens in the STATE_SEG 1, the next state will be the STATE_PESTATE1
    last_btl_state = STATE_SEG1;
    btl_state = STATE_PESTATE1;
    //resync = false;
  }
  else if(resync && (btl_state == STATE_SEG2)){
    // If a resync happens in the STATE_SEG 1, the next state will be the STATE_PESTATE1
    last_btl_state = STATE_SEG2;
    btl_state = STATE_PESTATE2;
    //resync = false;
  }
  else if(resync)
    resync = false; 

  //debug
  if(btl_state == STATE_SEG1){
    digitalWrite(LED_PHASE1,ON);
    digitalWrite(LED_PHASE2,OFF);
  }
  else if(btl_state == STATE_SEG2){
    digitalWrite(LED_PHASE1,OFF);
    digitalWrite(LED_PHASE2,ON);
  }
  else if((btl_state == STATE_PESTATE1) || (btl_state == STATE_PESTATE2)){
    digitalWrite(LED_PHASE1,ON);
    digitalWrite(LED_PHASE2,ON);
  }
  else{ // btl_state == Sync
    digitalWrite(LED_PHASE1,OFF);
    digitalWrite(LED_PHASE2,OFF);
  }
  //end debug

  //BTL - State Machine
  switch(btl_state){
    case STATE_SYNC:
      writing_point = true; // Write time!
      tq_cnt = 0; // Reset global counter
      last_btl_state = STATE_SYNC; // This state'll be the last on the next tq
      btl_state = STATE_SEG1; // Go to SEG1 on the next tq
                              // Note that the switch uses btl_state
      break;

    case STATE_SEG1:
      if(hard_sync){
        tq_cnt = 0; // will be incremented soon, that's why it's not 1.
        hard_sync = false; // unset flag
      }
      tq_cnt++
      if(last_btl_state == STATE_SYNC){ // If arrived from SYNC
        writing_point = false;
        resize = 0;
        tq_cnt_seg1 = 0;
        last_btl_state = STATE_SEG1;
      }
      else{
        tq_cnt_seg1++;
      }
      phase_error = tq_cnt;
      if(tq_cnt_seg1 >= (SEG1_SIZE - 1)){
        btl_state = STATE_SEG2;
      }
      break;

    case STATE_PESTATE1:
      resync = false;
      if(last_btl_state == STATE_SEG1){
        resize = min(SJW,phase_error);
        // Serial.print("phase_error: ");
        // Serial.println(phase_error);
        // Serial.print("resize: ");
        // Serial.println(resize);
        last_btl_state = STATE_PESTATE1;
      }
      tq_cnt_seg1++;
      tq_cnt++;
      if(tq_cnt_seg1 >= (SEG1_SIZE + resize - 1))
        btl_state = STATE_SEG2;
      break;

    case STATE_SEG2:
      if((last_btl_state == STATE_SEG1) || (last_btl_state == STATE_PESTATE1)){
        sample_point = true;
        tq_cnt_seg2 = 0;
        tq_cnt++;
        last_btl_state = STATE_SEG2;
      }
      else{
        if(tq_cnt_seg2 == 1)
          sample_point = false;
        tq_cnt_seg2++;
        tq_cnt++;
      }
      phase_error = -BIT_NOMINAL_SIZE + (tq_cnt + resize);
      if(tq_cnt_seg2 >= (SEG2_SIZE - 1)){
        btl_state = STATE_SYNC;
      }
      break;

    case STATE_PESTATE2:
      resync = false
      if(last_btl_state == STATE_SEG2){
        resize = max(-SJW,phase_error);
        //Serial.print("phase_error: ");
        //Serial.println(phase_error);
        //Serial.print("resize: ");
        //Serial.println(resize);
        if(tq_cnt_seg2 == 1)
          sample_point = false;
        last_btl_state = STATE_PESTATE2;
      }
      tq_cnt_seg2++;
      tq_cnt++;
      if((tq_cnt_seg2 >= (SEG2_SIZE + resize - 1)) && (-phase_error <= SJW)){
        writing_point = true;
        tq_cnt = 0;
        last_btl_state = STATE_SYNC;
        btl_state = STATE_SEG1;
      }
      else if((tq_cnt_seg2 >= (SEG2_SIZE + resize - 1)) && (-phase_error > SJW)){
        btl_state = STATE_SYNC;
      }
      break;

  }
}

void BTLsync(){
  if(idle_bus){
    hard_sync = true;
    writing_point = true;
  }
  else
    resync = true;
}

void CCLinit(){
  idle_bus = true;
}

#include <Arduino.h>
#include <TimerOne.h>
#include "CInCAN.h"

//Bit Timing Logic Flags
volatile bool rx_falling;
volatile bool tq_flag;
bool btl_resync_enable;

//Bit Timing Logic Variables
int btl_current_state; 
int btl_next_state;
int btl_tq_cnt; // Global time quanta counter (0 -> (BIT_NOMINAL_SIZE - 1))
int btl_tq_cnt_seg1; // SEG1 counter (0 -> (TSEG1_SIZE - 1))
int btl_tq_cnt_seg2; // SEG1 counter (0 -> (TSEG2_SIZE - 1))
int btl_phase_error; // CAN Protocol Phase Error
int btl_resize; // calculate the bit timing adjustment for resychronization due 
                // to any phase_error.
bool btl_sample_point;  // indicate when the sample point happened.
bool btl_writing_point; // indicate when the writing point happened.
bool btl_hard_sync; // indicate a hard synchronization.
bool btl_resync; // indicate a resynchronization (if the falling edge have 
                 // happened out of sync segment).

//CAN Controller Flags
//CAN Controller Variables
bool ccl_bus_idle; //indicate when the bus is idle

//TOGGLE Vector
bool toggle[TOGGLE_SIZE];


//Bit Timing Logic Functions Definition
void BTL_init(){ 
   // Initializing Variables 
    btl_next_state = BTL_STATE_SYNC; //First State: Sync Segment
    btl_tq_cnt = 0;
    btl_tq_cnt_seg1 = 0;
    btl_tq_cnt_seg2 = 0;
    btl_phase_error = 0;
    btl_resize = 0;
    btl_sample_point = false;
    btl_writing_point = false;
    btl_hard_sync = false;
    btl_resync = false;

    //Initializing Flags
    rx_falling = false;
    tq_flag = false;
    btl_resync_enable = true;
}

void BTL_edge_detector(){
    //Serial.println("EDGE");
    rx_falling = true;
}

void BTL_new_time_quanta(){
    //Serial.println("CLOCK");
    tq_flag = true;
    toggle[TOGGLE_INDEX_TQ] = !toggle[TOGGLE_INDEX_TQ];//TOGGLE
}

void BTL_sm(){
    if(rx_falling){
        cli();
        if(ccl_bus_idle){
            btl_hard_sync = true;
            toggle[TOGGLE_INDEX_HARDSYNC] = !toggle[TOGGLE_INDEX_HARDSYNC];//TOGGLE
            Timer1.restart();   //it will trigger Timer1 Interruption and 
                                //reset its clock. The ISR will be called after
                                //the functino sei() is called
        }
        else {
            btl_resync = true;
            toggle[TOGGLE_INDEX_RESYNC] = !toggle[TOGGLE_INDEX_RESYNC];//TOGGLE
        }
        rx_falling = false;
        sei();
    }

    if(tq_flag){
        //state machine here!
        tq_flag = false;
        if(btl_hard_sync){
            //Serial.println("HARD_SYNC");
            btl_next_state = BTL_STATE_SYNC; //Forces the Sync state.
            btl_hard_sync = false;
        }
        else if(btl_resync){
            if(btl_resync_enable){
                //Serial.println("RESYNC");
                if(btl_current_state == BTL_STATE_TSEG1){
                    btl_resize = min(BTL_SJW,btl_phase_error);  
                                                        //in this case, the phase
                                                        //error ir positive. The
                                                        //adjustment due to a 
                                                        //phase error is limited
                                                        //to SJW
                    btl_next_state = BTL_STATE_PESTATE1;
                    btl_resync_enable = false;
                }
                else if(btl_current_state == BTL_STATE_TSEG2){
                    btl_resize = min(BTL_SJW,-btl_phase_error);
                                                        //in this case, the phase
                                                        //error is negative. The 
                                                        //adjustment due to a 
                                                        //phase error is limited
                                                        //to -SJW
                    if(-btl_phase_error <= BTL_SJW)
                    //The protocol can handle all the phase error
                        btl_next_state = BTL_STATE_SYNC;
                    else
                    //The protocol cannot handle all the phase error due to SJW
                        btl_next_state = BTL_STATE_PESTATE2;
                    btl_resync_enable = false;
                }

            }

            btl_resync = false;
        }

        switch(btl_next_state){

            //Synchronization Segment
                //Nominal Length: SYNC_SEG_SIZE = 1tq (Default)
                //must set 'writing_points' during its time quanta.
            case BTL_STATE_SYNC:
                btl_writing_point = true;
                btl_resync_enable = true;
                btl_tq_cnt = 0;
                btl_resize = 0;
                btl_phase_error = 0;
                btl_current_state = BTL_STATE_SYNC;
                btl_next_state = BTL_STATE_TSEG1;
                break;

            //Tseg1 (Propagation Segment and Phase Buffer Segment 1)
                //Nominal Length: BTL_TSEG1_SIZE = 8tq (currently)
            case BTL_STATE_TSEG1:
                btl_tq_cnt++;
                if(btl_current_state == BTL_STATE_SYNC){
                    //This if runs at the first time quanta of TSEG1
                    btl_writing_point = false;
                    btl_tq_cnt_seg1 = 0;
                    btl_current_state = BTL_STATE_TSEG1;
                }
                else 
                    btl_tq_cnt_seg1++;
                //The phase_error in TSEG1 matches with the value of the
                //time quanta counter.
                btl_phase_error = btl_tq_cnt;
                if(btl_tq_cnt_seg1 >= BTL_TSEG1_SIZE - 1) 
                    btl_next_state = BTL_STATE_TSEG2;
                break; 

            //Pestate1(Phase Error State 1)
                //This state is reached after a resync during TSEG1
            case BTL_STATE_PESTATE1:
                if(btl_current_state == BTL_STATE_TSEG1)
                    //This if runs at the first time quanta of PESTATE1
                    btl_current_state = BTL_STATE_PESTATE1;
                btl_tq_cnt++;
                btl_tq_cnt_seg1++;
                if(btl_tq_cnt_seg1 >= BTL_TSEG1_SIZE -1 + btl_resize)    
                    btl_next_state = BTL_STATE_TSEG2;                             
                break;

            case BTL_STATE_TSEG2:
                btl_tq_cnt++;
                if((btl_current_state == BTL_STATE_TSEG1) || (btl_current_state == BTL_STATE_PESTATE1) ){
                    //This if runs at the first time quanta of TSEG2
                    btl_sample_point = true;
                    btl_tq_cnt_seg2 = 0;
                    btl_current_state = BTL_STATE_TSEG2;
                }
                else{
                    btl_sample_point = false;
                    btl_tq_cnt_seg2++;
                }
                btl_phase_error = btl_tq_cnt_seg2 + BTL_SYNC_SEG_SIZE + BTL_TSEG1_SIZE - BTL_BIT_NOMINAL_SIZE;
                if(btl_tq_cnt_seg2 >= BTL_TSEG2_SIZE -1)
                    btl_next_state = BTL_STATE_SYNC; 
                break;

            case BTL_STATE_PESTATE2:
                    if(btl_current_state == BTL_STATE_TSEG2)
                    //This if runs at the first time quanta of PESTATE2
                        btl_current_state = BTL_STATE_PESTATE2;
                    btl_tq_cnt++;
                    btl_tq_cnt_seg2++;
                    if(btl_tq_cnt_seg2 >= BTL_TSEG2_SIZE -1 -btl_resize)
                        btl_next_state = BTL_STATE_SYNC;
                break;
        } 
    //BTL_print();
    //TOGGLE_state();
    }
}

void BTL_print(){
    if(!(rx_falling && ccl_bus_idle)){
        Serial.print("tq_cnt: ");
        Serial.print(btl_tq_cnt);
        Serial.print("\t,current_btl_state: ");
        switch(btl_current_state){
            case BTL_STATE_SYNC:
                Serial.print("SYNC");
                break;
            case BTL_STATE_TSEG1:
                Serial.print("TSEG1");
                break;
            case BTL_STATE_PESTATE1:
                Serial.print("PESTATE1");
                break;
            case BTL_STATE_TSEG2:
                Serial.print("TSEG2");
                break;
            case BTL_STATE_PESTATE2:
                Serial.print("PESTATE2");
                break;
        }
        Serial.print("\t,sample_point: ");
        Serial.print(btl_sample_point);
        Serial.print("\t,writing_point: ");
        Serial.print(btl_writing_point);
        Serial.print("\t,phase_error: ");
        Serial.print(btl_phase_error);
        Serial.print("\t,bus_idle: ");
        Serial.println(ccl_bus_idle);
    }
}

//CAN Controller Functions Definition
void CCL_init(){
    ccl_bus_idle = false;
}

//TOGGLE Lines
//Time Quanta -> 61
//HardSync -> 69
//Resync -> 76
//state -> 198

//TOGGLE Functions Definition
void TOGGLE_init(){//main.cpp line 21
    toggle[TOGGLE_INDEX_TQ] = false;
    toggle[TOGGLE_INDEX_HARDSYNC] = false;
    toggle[TOGGLE_INDEX_RESYNC] = false;
    toggle[TOGGLE_INDEX_STATE_MSB] = false;
    toggle[TOGGLE_INDEX_STATE_LSB] = false;
    pinMode(TOGGLE_PIN_TQ,OUTPUT);     
    pinMode(TOGGLE_PIN_HARDSYNC,OUTPUT);
    pinMode(TOGGLE_PIN_RESYNC,OUTPUT);
    pinMode(TOGGLE_PIN_STATE_MSB,OUTPUT);
    pinMode(TOGGLE_PIN_STATE_LSB,OUTPUT);
}

void TOGGLE_write(){//main.cpp line 39
    digitalWrite(TOGGLE_PIN_TQ,toggle[TOGGLE_INDEX_TQ]);
    digitalWrite(TOGGLE_PIN_HARDSYNC,toggle[TOGGLE_INDEX_HARDSYNC]);
    digitalWrite(TOGGLE_PIN_RESYNC,toggle[TOGGLE_INDEX_RESYNC]);
    digitalWrite(TOGGLE_PIN_STATE_MSB,toggle[TOGGLE_INDEX_STATE_MSB]);
    digitalWrite(TOGGLE_PIN_STATE_LSB,toggle[TOGGLE_INDEX_STATE_LSB]);
}

void TOGGLE_write_serial(){//main.cpp line 40
    Serial.print(toggle[TOGGLE_INDEX_TQ]);
    Serial.print(" ");
    Serial.print(toggle[TOGGLE_INDEX_HARDSYNC]);
    Serial.print(" ");
    Serial.print(toggle[TOGGLE_INDEX_RESYNC]);
    Serial.print(" ");
    Serial.println(2*toggle[TOGGLE_INDEX_STATE_MSB]+ toggle[TOGGLE_INDEX_STATE_LSB]);
}


void TOGGLE_state(){//line 198
    if(btl_current_state == BTL_STATE_SYNC){
        toggle[TOGGLE_INDEX_STATE_LSB] = false;
        toggle[TOGGLE_INDEX_STATE_MSB] = false;
    }
    else if((btl_current_state == BTL_STATE_TSEG1) || (btl_current_state == BTL_STATE_PESTATE1)){
        toggle[TOGGLE_INDEX_STATE_LSB] = true;
        toggle[TOGGLE_INDEX_STATE_MSB] = false;
    }
    else{
        toggle[TOGGLE_INDEX_STATE_LSB] = false;
        toggle[TOGGLE_INDEX_STATE_MSB] = true;
    }
}
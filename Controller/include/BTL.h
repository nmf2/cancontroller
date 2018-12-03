#ifndef _CIN_CAN_H_
#define _CIN_CAN_H_

//Bit Timing Logic Parameters
#define BTL_TIME_QUANTA 10000 //tq of 1s 
#define BTL_SJW 4
#define BTL_SYNC_SEG_SIZE 1
#define BTL_PROP_SEG_SIZE 1
#define BTL_PHASE1_SEG_SIZE 1
#define BTL_TSEG1_SIZE 8 //PROP_SEG_SIZE + PHASE1_SEG_SIZE
#define BTL_PHASE2_SEG_SIZE 7
#define BTL_TSEG2_SIZE 7 //PHASE2_SEG_SIZE
#define BTL_BIT_NOMINAL_SIZE 16 //SYNC_SIZE + TSEG1_SIZE + TSEG2_SIZE
#define BTL_STATE_SYNC 0
#define BTL_STATE_TSEG1 1
#define BTL_STATE_PESTATE1 2
#define BTL_STATE_TSEG2 3
#define BTL_STATE_PESTATE2 4

//Bit Timing Logic Functions
void BTL_init(); //Initialize Bit Timing Logic Variables
void BTL_edge_detector(); //ISR of PIN_RX falling edge
void BTL_new_time_quanta(); //ISR of Timer1
void BTL_sm(); //Runs the Bit Timing Logic State Machine
void BTL_print(); //helps when debugging

//TOGGLE Parameters
#define TOGGLE_SIZE 5
#define TOGGLE_INDEX_TQ 0
#define TOGGLE_INDEX_HARDSYNC 1
#define TOGGLE_INDEX_RESYNC 2
#define TOGGLE_INDEX_STATE_MSB 3
#define TOGGLE_INDEX_STATE_LSB 4
#define TOGGLE_PIN_TQ 8
#define TOGGLE_PIN_HARDSYNC 9
#define TOGGLE_PIN_RESYNC 10
#define TOGGLE_PIN_STATE_MSB 11
#define TOGGLE_PIN_STATE_LSB 12

//TOGGLE Functions
void TOGGLE_init();     //Initialize Toggle Vector
void TOGGLE_write();    //Refresh Toggle outputs
void TOGGLE_write_serial(); //outputs on Serial
void TOGGLE_state();    //Refresh toggle[TOGGLE_PIN_STATE_X], X=LSB or X=MSB

extern bool volatile btl_writing_point;
extern bool volatile btl_sample_point;

#endif
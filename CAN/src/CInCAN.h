#ifndef _CIN_CAN_H_
#define _CIN_CAN_H_

//Transceiver IO
#define PIN_RX 2 //Rx from transceiver 
#define PIN_TX 3 //Tx from transceiver

//Bit Timing Logic Parameters
#define BTL_TIME_QUANTA 50000 //tq of 1s 
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

//CAN Controller Parameters
#define BIT_START_OF_FRAME  0
#define BIT_START_ID_A  1
#define BIT_END_ID_A  11
#define BIT_RTR_A  12
#define BIT_SRR_B  12
#define BIT_IDE  13
#define BIT_R0_A  14
#define BIT_START_ID_B  14
#define BIT_END_ID_B  31
#define BIT_RTR_B  32
#define BIT_R1_B  33
#define BIT_R0_B  34
#define BIT_START_DLC_A  15
#define BIT_START_DLC_B  35
#define BIT_END_DLC_A  18
#define BIT_END_DLC_B  38
#define BIT_START_DATA_A  19
#define BIT_START_DATA_B  39

#define BIT_REMOTE_START_CRC_A  19
#define BIT_REMOTE_START_CRC_B  39 
#define BIT_REMOTE_END_CRC_A  33
#define BIT_REMOTE_END_CRC_B  53
#define BIT_REMOTE_CRC_DELIMITER_A  34
#define BIT_REMOTE_CRC_DELIMITER_B  54
#define BIT_REMOTE_ACK_A  35
#define BIT_REMOTE_ACK_B 55
#define BIT_REMOTE_ACK_DELIMITER_A 36
#define BIT_REMOTE_ACK_DELIMITER_B 56
#define BIT_REMOTE_START_EOF_A 37
#define BIT_REMOTE_START_EOF_B 57
#define BIT_REMOTE_END_EOF_A 43
#define BIT_REMOTE_END_EOF_B 63
#define IFS_START_INTERMISSION1 0
#define IFS_END_INTERMISSION1 1
#define IFS_INTERMISSION2 2
#define EOL_START_FLAG 0
#define EOL_MINIMUM_END_FLAG 5

#define DOMINANT 0
#define RECESSIVE 1

#define DATA_FRAME 0
#define REMOTE_FRAME 

//CAN Controller Functions
void CCL_init(); //Initialize CAN Controller Variables
void CCL_sm();
void frame_walker();

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
void TOGGLE_print();

#endif
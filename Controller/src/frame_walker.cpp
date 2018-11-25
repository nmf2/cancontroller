#include <stdbool.h>
#include "../include/interface.h"
//#include "../include/util.h"

// To mark the frame as invalid do Frame[0] = 1 (should be zero)

// Vars

State state = IDLE; // This var indicates the current sm's state.
Frame frame;
bool Rx = 0; // Data from transceiver
bool err = 0; // Error Flag 
int bit_index = 0;
int ifs_index = 0; //Inter-frame space index
int idle_bus = 1; // Bus-is-idle flag
int DLC_value = 0; // Integer value of the DLC field.
int eol_recessive_count = 0;
int eol_dominant_count = 0;

// Dynamic bit indexes 
int BIT_START_DLC_X,
    BIT_END_DLC_X,
    BIT_START_DATA_X,
    BIT_END_DATA_X,
    BIT_Y_START_CRC_X,
    BIT_Y_END_CRC_X,
    BIT_Y_CRC_DELIMITER_X,
    BIT_Y_ACK_X,
    BIT_Y_ACK_DELMITER_X,
    BIT_Y_START_EOF_X,
    BIT_Y_END_EOF_X;

#ifndef min

int min(int a, int b){
    return a < b ? a : b;
}

#endif

void frame_walker(){
    if (Rstuff_flag == true || sp == 0){ // Simply return
        return;
    }
    else if (err == true){
        state = ERROR_FLAG;
    }
    else if (state < INTERMISSION1){
        bit_index++; // Simply go to the next bit
        frame.data[bit_index] = Rx; // take data in
    }

    switch(state){
        case IDA:
            if (bit_index == BIT_END_ID_A){ // this is the last bit of the IDA
                state = RTRA_SRR;
            }
            break;

        case RTRA_SRR:
            frame.type = Rx; // If 0, data frame, else, remote frame.
            state = IDE;
            break;

        case IDE:
            frame.extended = Rx; // If 0, standard, if 1, extended.
            if (frame.extended == true){
                state = IDB;
            }
            else {
                state = r0;
            }
            break;
        
        // Not extended
        case r0:
            state = DLC;
            // Use standard frame's DLC index
            BIT_START_DLC_X = BIT_START_DLC_A; 
            BIT_END_DLC_X = BIT_END_DLC_A;
            break;
        
        // Extended
        case IDB:
            if (bit_index == BIT_END_ID_B){
                state = RTRB;
            }
            break;
        
        case RTRB:
            state = r1_r0;
            break;
        
        case r1_r0:
            if(bit_index == BIT_START_DLC_B - 1){ // Two bits have passed?
                state = DLC; 
                // Use extended frame's DLC index
                BIT_START_DLC_X = BIT_START_DLC_B;
                BIT_END_DLC_X = BIT_END_DLC_B;
            }
            break;
        // End extended
        // TODO Add 'X' and 'Y' variables to simplify these conditions
        case DLC:
            /* Since BIT_END_DLC_X is used, we don't have to worry about if
             * the frame is extendend, only if it's remote.
             */  
            if (bit_index == BIT_END_DLC_X) {
                DLC_value = bits_to_int(BIT_START_DLC_X, BIT_END_DLC_X, frame.data);
                frame.payload_size = DLC_value;
                if (frame.type == REMOTE_FRAME || DLC_value == 0){
                    state = CRC;
                    // Set up next state's limit indexes
                    BIT_Y_START_CRC_X = BIT_END_DLC_X + 1;
                    BIT_Y_END_CRC_X = BIT_Y_START_CRC_X + 15; // CRC size is 15
                }
                else { // It's a DATA_FRAME and DLC_value is greater than zero.
                    state = PAYLOAD;
                    // Set up next state's limit indexes
                    BIT_START_DATA_X = BIT_END_DLC_X + 1;
                    BIT_END_DATA_X = BIT_START_DATA_X + min(64, DLC_value*8);
                }
            }
        // Data Frame
        case PAYLOAD:
            if (bit_index == BIT_END_DATA_X){
                state = CRC;
                BIT_Y_START_CRC_X = BIT_END_DATA_X + 1;
                BIT_Y_END_CRC_X = BIT_Y_START_CRC_X + 15; // CRC size is 15
            }
            break;
        
        //Remote Frame OR End of Data
        case CRC:
            if (bit_index == BIT_Y_END_CRC_X){
                state = CRCd;
                BIT_Y_CRC_DELIMITER_X = BIT_Y_END_CRC_X + 1;
            }
            break;

        case CRCd:
            state = ACK;
            BIT_Y_ACK_X = BIT_Y_CRC_DELIMITER_X + 1;
            break;
        
        case ACK:
            state = ACKd;
            BIT_Y_ACK_DELMITER_X = BIT_Y_ACK_X + 1;
            break;

        case ACKd:
            state = EOFR;
            BIT_Y_START_EOF_X = BIT_Y_ACK_DELMITER_X + 1;
            BIT_Y_END_EOF_X = BIT_Y_START_EOF_X + 7; // EOF is 7 bits long

            break;

        case EOFR:
            if (bit_index == BIT_Y_END_EOF_X){
                state = INTERMISSION1;
                ifs_index = 0;
            }
            break;
        
        // Start of intermission
        case INTERMISSION1:
            if(Rx == 0){
                state = ERROR_FLAG;
            }
            else if(ifs_index == IFS_END_INTERMISSION1){
                state = INTERMISSION2;
            }
            ifs_index++;
            break;

        case INTERMISSION2:
            state = IDLE;
            ifs_index = 0;
            if(Rx != 1){ // If Rx == 0 execute the statements for IDLE as well
                break;
            }
        
        case IDLE:
            if (Rx == 1) {
                idle_bus = 1;
            }
            else {
                idle_bus = 0;
                bit_index = 0;
                frame.data[0] = 0;
                DLC_value = 0;
                state = IDA;
            }
            break;
        
        case ERROR_FLAG:
            if (Rx == 0){
                eol_dominant_count++;
                if (eol_dominant_count >= 13){ 
                    /* need to reset the state, the fisrt bit of the delimiter
                       is dominant. 
                    */
                    eol_dominant_count = 0;
                }
            } 
            else if (Rx == 1 && eol_dominant_count >= 6){
                state = ERROR_DELIMITER;
                eol_recessive_count = 0;
            }
            break;
        
        case ERROR_DELIMITER:
            if (Rx == 0){ //error, go back to ERROR_FLAG
                state = ERROR_FLAG;
                eol_dominant_count = 1;
            } else {
                eol_recessive_count++;
                if (eol_recessive_count == 7){
                    state = INTERMISSION1;
                    eol_recessive_count = 0;
                    eol_dominant_count = 0;
                    ifs_index = 0;
                }
            }

            break;
    }
}

void log(){
    //printf("STATE");
}
#include <stdbool.h>
#include "../include/params.h" // some include containing the Frame vector
#include "../lib/util.h"

typedef
enum State {IDLE, IDA, RTRA_SRR, IDE, r0, IDB, RTRB, r1_r0, DLC, PAYLOAD,
            CRC, CRCd, ACK, ACKd, EOFR, INTERMISSION1, INTERMISSION2, 
            ERROR_OVERLOAD}
State;
// To mark the frame as invalid do Frame[0] = 1 (should be zero)
// Vars

State state; // This var indicates the current sm's state.

bool rx = 0; // Data from transceiver
bool err = 0; // Error Flag 
int bit_index = 0;
int ifs_index = 0; //Inter-frame space index
int idle_bus = 1; // Bus-is-idle flag
int DLC_value = 0; // Integer value of the DLC field.

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


void controller_sm(){
    if (err == true){
        state = ERROR_OVERLOAD;
    }
    else if (state < INTERMISSION1){
        bit_index++; // Simply go to the next bit
        frame.data[bit_index] = rx; // take data in
    }

    switch(state){
        case IDLE:
            if (rx == 1) {
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

        case IDA:
            if (bit_index == BIT_END_ID_A){ // this is the last bit of the IDA
                state = RTRA_SRR;
            }
            break;

        case RTRA_SRR:
            frame.type = rx; // If 0, data frame, else, remote frame.
            state = IDE;
            break;

        case IDE:
            frame.extended = rx; // If 0, standard, if 1, extended.
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
            BIT_START_DLC_X = BIT_END_DLC_A; 
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
                BIT_START_DLC_X = BIT_END_DLC_B;
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
                    BIT_END_DATA_X = BIT_START_DLC_A + min(64, DLC_value*8);
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
            if(rx == 0){
                state = ERROR_OVERLOAD;
            }
            else if(ifs_index == IFS_END_INTERMISSION1){
                state = INTERMISSION2;
            }
            ifs_index++;
            break;

        case INTERMISSION2:
            state = IDLE;
            
            if(rx != 1){ // If rx == 0 execute the statements for IDLE as well
                break;
            }
        
        //TODO Figure out error state.
    }
}
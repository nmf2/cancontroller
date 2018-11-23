#include <stdbool.h>
#include "include/params.h" // some include containing the Frame vector
#include "lib/util.h"

typedef
enum State {IDLE, SOF, IDA, RTRA_SRR, IDE, r0, IDB, RTRB, r1_r0, DLC, PAYLOAD,
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
int DLC_value = 0;

int BIT_START_DLC_


void controller_sm(){
    switch(state){
        bit_index++; // Simply go to the next bit
        frame.data[bit_index] = rx; // take data in

        case SOF:
            bit_index = 0;
            state = IDA;
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
            if (frame.extended == TRUE){
                state = IDB;
            }
            else {
                state = r0;
            }
            break;
        
        // Not extended
        case r0:
            state = DLC;
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
            }
            break;
        // End extended
        // TODO Add 'X' and 'Y' variables to simplify these conditions
        case DLC:
            if (frame.extended == TRUE && bit_index == BIT_END_DLC_B) {
                DLC_value = bits_to_int(BIT_START_DLC_B, BIT_END_DLC_B, frame.data)
                
                if (frame.type == REMOTE_FRAME || DLC_value = 0){
                    state = CRC;
                }
                else { // It's a DATA_FRAME and DLC_value is greater than zero.
                    state = PAYLOAD;
                }
            }
            else if (frame.extended == FALSE && bit_index == BIT_END_DLC_A){
                DLC_value = bits_to_int(BIT_START_DLC_A, BIT_END_DLC_A, frame.data)

                if (frame.type == REMOTE_FRAME || DLC_value = 0){
                    state = CRC;
                }
                else { // It's a DATA_FRAME and DLC_value is greater than zero.
                    state = PAYLOAD;
                }
            }
    }
}
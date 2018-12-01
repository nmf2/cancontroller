#include <stdbool.h>
#include "../include/interface.h"
//#include "../include/util.h"

// To mark the frame as invalid do Frame[0] = 1 (should be zero)

// Vars

State state = IDLE; // This var indicates the current sm's state.
State last_state = IDLE; // This var indicates the current sm's state.
Frame frame;

int bit_index = 0;
int ifs_index = 0; //Inter-frame space index
int idle_bus = 1; // Bus-is-idle flag
int DLC_value = 0; // Integer value of the DLC field.
int eol_recessive_count = 0;
int eol_dominant_count = 0;
bool bus_data[300] = { 0 };
int bdi = 0;

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
        bus_data[bdi++] = Rx;
        return;
    }
    if (err == true){
        state = ERROR_FLAG;
    }
    else if (state < INTERMISSION1){
        bit_index++; // Simply go to the next bit
        frame.data[bit_index] = Rx; // take data in
        bus_data[bdi++] = Rx;
    }
    
    last_state = state; // This state will be the last after this runs
    switch(state){
        case IDA:
            if (bit_index == BIT_END_ID_A){ // this is the last bit of the IDA
                state = RTRA_SRR;
                print_array2(frame.data, BIT_START_ID_A, BIT_END_ID_A);
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
                print_array2(frame.data, BIT_START_ID_B, BIT_END_ID_B);
            }
            break;
        
        case RTRB:
            frame.type = Rx; // If 0, data frame, else, remote frame.
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
        
        case DLC:
            /* Since BIT_END_DLC_X is used, we don't have to worry about if
             * the frame is extendend, only if it's remote.
             */  
            if (bit_index == BIT_END_DLC_X) {
                DLC_value = bits_to_int(BIT_START_DLC_X, BIT_END_DLC_X, 
                                        frame.data);
                frame.payload_size = DLC_value;
                if (frame.type == REMOTE_FRAME || DLC_value == 0){
                    state = CRC;
                    // Set up next state's limit indexes
                    BIT_Y_START_CRC_X = BIT_END_DLC_X + 1;
                    // CRC size is 15
                    BIT_Y_END_CRC_X = BIT_Y_START_CRC_X + 15 - 1; 
                }
                else { // It's a DATA_FRAME and DLC_value is greater than zero.
                    state = PAYLOAD;
                    // Set up next state's limit indexes
                    BIT_START_DATA_X = BIT_END_DLC_X + 1;
                    BIT_END_DATA_X = BIT_START_DATA_X + min(64, DLC_value*8)-1;
                    // The reason for the -1 is because the var is an
                    // index.
                }
                print_array2(frame.data, BIT_START_DLC_X, BIT_END_DLC_X);
            }
            break;
        // Data Frame
        case PAYLOAD:
            if (bit_index == BIT_END_DATA_X){
                state = CRC;
                BIT_Y_START_CRC_X = BIT_END_DATA_X + 1;
                BIT_Y_END_CRC_X = BIT_Y_START_CRC_X + 15 - 1; // CRC size is 15
                print_array2(frame.data, BIT_START_DATA_X, BIT_END_DATA_X);
            }
            break;
        
        // Remote Frame OR End of Payload
        case CRC:
            if (bit_index == BIT_Y_END_CRC_X){
                state = CRCd;
                BIT_Y_CRC_DELIMITER_X = BIT_Y_END_CRC_X + 1;
                print_array2(frame.data, BIT_Y_START_CRC_X, BIT_Y_END_CRC_X);
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
            BIT_Y_END_EOF_X = BIT_Y_START_EOF_X + 7 - 1; // EOF is 7 bits long

            break;

        case EOFR:
            if (bit_index == BIT_Y_END_EOF_X){
                state = INTERMISSION1;
                ifs_index = 0;
            }
            break;
        
        // Start of intermission
        case INTERMISSION1:
            if(ifs_index == IFS_END_INTERMISSION1){
                state = INTERMISSION2;
            }
            ifs_index++;
            break;

        case INTERMISSION2:
            state = IDLE; 
            ifs_index = 0;
            if(Rx != 0){ // If Rx == 0 execute the statements for IDLE as well
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
                bdi = 0;
            }
            break;
        
        case ERROR_FLAG:
            if (Rx == 0){
                eol_dominant_count++;
                // if (eol_dominant_count >= 13){ 
                //     /* need to reset the state, the fisrt bit of the delimiter
                //        is dominant. 
                //     */
                //     eol_dominant_count = 0;
                // }
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
            } else { // Rx == RECESSIVE
                eol_recessive_count++;
                if (eol_recessive_count == 7){
                    state = INTERMISSION1;
                    eol_recessive_count = 0;
                    eol_dominant_count = 0;
                    ifs_index = 0;
                }
            }
            break;
            
        case OVERLOAD_FLAG:
            if (Rx == 0){
                eol_dominant_count++;
                // if (eol_dominant_count >= 13){ 
                //     state = ERROR_FLAG;
                //     eol_dominant_count = 0;
                // }
            } 
            else if (Rx == 1 && eol_dominant_count >= 5){
                state = OVERLOAD_DELIMITER;
                eol_recessive_count = 0;
            }
            break;
        
        case OVERLOAD_DELIMITER:
            if (Rx == 0){ //error, go back to ERROR_FLAG
                state = ERROR_FLAG;
                eol_dominant_count = 1;
            } else { // Rx == RECESSIVE
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
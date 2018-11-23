#include "Frame.h" // some include containing the Frame vector

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


void controller_sm(){
    switch(state){
        case SOF:
            bit_index = 0;
            Frame[BIT_]

    }
}
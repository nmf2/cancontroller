#include "stdbool.h"
#include "interface.h"
#include "BTL.h"

bool lost_arbitration = false;
bool bit_err = false;
// int arbitration_state = IDE;

void bit_monitor(){
    if (bit_err == true){
        bit_err = false;
    }
    // if (frame.data[IDE] == 1){
    //     arbitration_state = RTRB;
    // } else {
    //     arbitration_state = IDE;
    // }
    if (writing_mode){
        if(last_state <= IDE + frame.data[BIT_IDE]*(RTRB - IDE)){
            //Serial.println("Entrou");
            if (Rx != Tx){
                lost_arbitration = true;
            } else {
                lost_arbitration = false;
            }
        } 
        else if (last_state >= CRCd && last_state <= EOFR && last_state != ACK){
            // If the state is between CRCd and EOFR and it's not ACK
            if(Rx != Tx) {
                bit_err = true;
            }
        }
    }

}


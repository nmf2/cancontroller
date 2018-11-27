#include "stdbool.h"
#include "../include/interface.h"

bool lost_arbitration;

void bit_monitor(){
    if(wp == false){
        return; 
    }
    if(writing_mode){
        if(last_state < CRCd){
            if (Rx != Tx){
                lost_arbitration = true;
            } else {
                lost_arbitration = false;
            }
        } 
        else if (last_state >= CRCd && last_state <= EOFR && last_state != ACK) {
            // If the state is between CRCd and EOFR and it's not ACK
            if(Rx != Tx) {
                bit_err = true;
            }
        }
    }
}


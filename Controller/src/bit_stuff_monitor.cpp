#include "stdbool.h"
#include "../include/interface.h"

bool Rstuff_flag;
bool stuff_err;
bool bsm_last_bit;
int bsm_bit_count;

void bit_stuff_monitor(){
    if (sp == false){
        return;
    }
    if (stuff_err == true){
        stuff_err = false;
    }
    if(state == IDLE){
        bsm_bit_count = 1; // SOF bit.
    } 
    else if (state < CRCd){
        if (Rstuff_flag == true){
            Rstuff_flag = false;
        }

        if (bsm_bit_count == 5){
            Rstuff_flag = true;
        } 
        else if (bsm_bit_count == 6){
            stuff_err = true;
        }

        if (bsm_last_bit == Rx){
            bsm_bit_count++;
        } 
        else {
            bsm_bit_count = 1;
        }
        bsm_last_bit = Rx;
        // if(Rstuff_flag == true && bsm_last_bit == Rx){
        //     stuff_err = false;
        // } else if (Rstuff_flag == true  && bsm_last_bit != Rx) {
        //     Rstuff_flag = false;
        // }
        // if (bsm_bit_count == 5){
        //     Rstuff_flag = true;
        // }
        // else if (bsm_bit_count > 5){
        //     stuff_err = true;
        // } 
        // if (bsm_last_bit == Rx){
        //     // Last bit equals current one
        //     bsm_bit_count++;
        // } else {
        //     bsm_bit_count = 0;
        // }
        // bsm_last_bit = Rx;
    }
}
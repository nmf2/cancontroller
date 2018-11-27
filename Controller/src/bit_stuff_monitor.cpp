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
    if (state < CRCd){
        if(Rstuff_flag == true){ // This is the bit after a stuff bit
            Rstuff_flag = false;
        }
        if (bsm_bit_count == 5){
            Rstuff_flag = true;
            bsm_bit_count = 0;
        }
        else if (bsm_bit_count > 5){
            stuff_err = true;
        } 
        if (bsm_last_bit == Rx){
            // Last bit equals current one
            bsm_bit_count++;
        }
        else {
            stuff_err = false;
        }
        bsm_last_bit = Rx;
    }
}
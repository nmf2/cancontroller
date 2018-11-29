#include "../include/interface.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

void printb(bool bit){
    printf("%d|", bit);
}

void framer(int id, unsigned long long int payload, bool extended, bool type, 
             int payload_size, Frame *frm){

    frm->type = type;
    frm->extended = extended;
    frm->payload_size = payload_size;
    
    int id_size;

    if (extended){ // Array will have 29 bits for id
        id_size = 29;
    } else { // Id will have 11 bits
        id_size = 11;
    }

    bool id_arr[id_size];
    int_to_bits(id, id_arr, id_size);

    int i, j, // Array's indexes
        rtr,
        start_dlc,
        end_dlc,
        start_payload,
        end_payload,
        start_crc,
        end_crc,
        crcd,
        ack,
        ackd,
        start_eof,
        end_eof;
    
    // Defining indicies
    if (extended == false){
        rtr = BIT_RTR_A;
        frm->data[BIT_IDE] = 0;
        printf("IDE: %d\n", frm->data[BIT_IDE]);
        frm->data[BIT_R0_A] = 0;
        start_dlc = BIT_START_DLC_A;
        end_dlc = BIT_END_DLC_A;
        start_payload = BIT_START_DATA_A;
    }
    else { // extended == true
        frm->data[BIT_SRR_B] = 1;
        
        frm->data[BIT_IDE] = 1;
        printf("IDE: %d\n", frm->data[BIT_IDE]);

        // IDB
        printf("IDB: |");
        for(i = BIT_START_ID_B, j = 11; i <= BIT_END_ID_B; i++, j++){
            frm->data[i] = id_arr[j];
            printb(frm->data[i]);
        }
        printf("\n");
        rtr = BIT_RTR_B;
        
        frm->data[BIT_R0_B] = 0;
        frm->data[BIT_R1_B] = 0;

        start_dlc = BIT_START_DLC_B;
        end_dlc = BIT_END_DLC_B;
        start_payload = BIT_START_DATA_B;
    }

    if(type == DATA_FRAME){
        if (payload_size == 0){
            end_payload = start_payload - 1; 
            // Makes sure the payload's for loop won't run 
            start_crc = end_dlc + 1;
        } 
        else {
            end_payload = start_payload + 8*payload_size - 1;
            start_crc = end_payload + 1;
        }
        frm->data[rtr] = DATA_FRAME;
        printf("RTR: %d\n", frm->data[rtr]);
    } else {
        start_payload = end_dlc - 1;
        end_payload = end_dlc;
        start_crc = end_dlc + 1;
        frm->data[rtr] = REMOTE_FRAME;
        printf("RTR: %d\n", frm->data[rtr]);
    }
    
    end_crc = start_crc + 15 - 1;
    
    crcd = end_crc + 1;
    ack = crcd + 1;
    ackd = ack + 1;

    start_eof = ackd + 1;
    end_eof = start_eof + 7 - 1;

    // printf("rtr: %d\n", rtr);
    // printf("start_dlc: %d\n", start_dlc);
    // printf("end_dlc: %d\n", end_dlc);
    // printf("start_payload: %d\n", start_payload);
    // printf("end_payload: %d\n", end_payload);
    // printf("start_crc: %d\n", start_crc);
    // printf("end_crc: %d\n", end_crc);
    // printf("crcd: %d\n", crcd);
    // printf("ack: %d\n", ack);
    // printf("ackd: %d\n", ackd);
    // printf("start_eof: %d\n", start_eof);
    // printf("end_eof: %d\n", end_eof);

    // SOF
    frm->data[0] = 0; 

    // IDA
    printf("IDA: |");
    for (i = BIT_START_ID_A, j = 0; i <= BIT_END_ID_A; i++, j++){
        frm->data[i] = id_arr[j];
        printb(frm->data[i]);
    }
    printf("\n");

    // DLC
    printf("DLC: |");
    bool dlc_arr[4] = { 0 };
    int_to_bits(payload_size, dlc_arr, 4);
    for (i = start_dlc, j = 0; i <= end_dlc; i++, j++){
        frm->data[i] = dlc_arr[j];
        printb(frm->data[i]);
    }
    printf("\n");

    // PAYLOAD
    printf("PAYLOAD: |");
    bool payload_arr[payload_size*8];
    int_to_bits(payload, payload_arr, 8*payload_size);
    for (i = start_payload, j = 0; i <= end_payload; i++, j++){
        frm->data[i] = payload_arr[j];
        printb(frm->data[i]);
    }
    printf("\n");
    
    // CRC
    printf("CRC: |");
    for (i = start_crc, j = 0; i <= end_crc; i++, j++){
        frm->data[i] = 0;
        printb(frm->data[i]);
    }
    printf("\n");
    
    //CRCd
    frm->data[crcd] = 1;

    //ACK
    frm->data[ack] = 1;

    //ACKd
    frm->data[ackd] = 1;

    //EOF
    printf("EOF: |");
    for (i = start_eof; i <= end_eof; i++){
        frm->data[i] = 1;
        printb(frm->data[i]);
    }
    printf("\n");

    frm->frame_size = end_eof + 1;
    
    //printf_arr(frm->data, end_eof);
}

unsigned long long id_calc(unsigned long long id1,unsigned long long id2){
    return (id1 << 18) + id2;
}

int main(){
    Frame test;
    printf("\n");
    printf("0x%llx\n", id_calc(0x0449,0x3007A));
    //             id               data             ide      type     dlc 
    framer(id_calc(0x0449,0x3007A), 0xAAAAAAAAAAAAAAAA, true, DATA_FRAME, 8, &test);
    printf("\n");
    print_frame(test);
    printf_arr(test.data, test.frame_size - 1);

    return 0;
}
#include <Arduino.h>
#include <stdbool.h>
#include "interface.h"
#include <stdio.h>
// Convert part of an array of bits into an integer
int bits_to_int(int start, int end, bool * data){
    int i, value = 0;

    for (i = start; i <= end; i++){
        value = (value << 1) | data[i];
    }

    return value;
}
     
int int_to_bits(unsigned long long value, bool * array, int size){
    /*
        Params:
            value: The int value to transform into a bit array
            *array: The array where the result will be written into.
        Return: 
            Length of the resulting bit string
    */
    int i = 0, j = 0;
    if (size == 0){
        size = 32;
    }
    bool inv_arr[size]; // will hold the inverted bit sequence
    for (i = 0; i < size; i++) inv_arr[i] = 0; // clear array
    
    array[0] = 0;
    i = 0;

    while(value != 0){
        if(value % 2 == 1){
            inv_arr[i] = 1;
        } else {
            inv_arr[i] = 0;
        }
        value = value >> 1;
        i++;
    }

    for(i = size - 1, j = 0; i >= 0; i--, j++){
        array[j] = inv_arr[i];
    }

    return 0; // Making sure size is at least 1.
}

void print_array(bool *array, int max){
    int i = 0;
    for (; i <= max; i++){
        Serial.print(array[i]);
    }
    Serial.println();
}

void print_array2(bool *array, int inf, int sup){
    int i = inf;
    for (; i <= sup; i++){
        Serial.print(array[i]);
    }
    Serial.println();
}

void printf_arr(bool *array, int end_index){
    //printf("|");
    int i = 0;
    for (; i <= end_index; i++){
        //printf("%d|", array[i]);
        printf("%d", array[i]);
    }
    printf("\n");
}



short next_crc(short crc_rg, bool nxt_bit){
    bool crc_rg_15th = (crc_rg >> 14) & 0x01; // crc_rg's fifteenth bit
    bool crc_nxt = (nxt_bit ^ crc_rg_15th) != 0;
    crc_rg = (crc_rg << 1) & 0x7FFF ;  // Shift left by 1 position
    if (crc_nxt){
        crc_rg = crc_rg ^ 0x4599;
    }
    return crc_rg;
}

void pfd(bool *array, int begin, int end){
    unsigned long long int value = bits_to_int(begin, end, array);
    int i;
    printf("Bin: |");
    for (i = begin; i <= end; i++){
        printf("%d|", array[i]);
    }
    printf("|\n");
    printf("Hex: %llx\n", value); 
    printf("Dec: %lld\n\n", value);
}

void print_frame(Frame frm, bool use_printf){
    if (use_printf){
        printf("Attributes:\n");
        printf("Extended: %s\n", frm.extended ? "Yes" : "No");
        printf("Type: %s\n", frm.type ? "REMOTE" : "DATA");
        printf("Frame size: %d\n", frm.frame_size);
        printf("Payload size: %d bytes\n\n", frm.payload_size);
    } else{
        Serial.print("Attributes: ");
        Serial.print("Extended: ");
        Serial.println(frm.extended ? "Yes" : "No");
        Serial.print("Type: ");
        Serial.println(frm.type ? "REMOTE" : "DATA");
        Serial.print("Frame size: ");
        Serial.println(frm.frame_size);
        Serial.print("Payload size: ");
        Serial.println(frm.payload_size);
        Serial.println(); 
        Serial.println();
    }

    // printf("IDA");
    // pfd(frm.data, BIT_START_ID_A, BIT_END_ID_A);
    
    // if (frm.extended){
    //     printf("SRR");
    // } else {
    //     printf("RTR");
    // }
    // pfd(frm.data, BIT_RTR_A, BIT_RTR_A);

    // printf("IDE");
    // pfd(frm.data, BIT_IDE, BIT_IDE);

    // if(frm.extended){
    //     printf("IDB");
    //     pfd(frm.data, BIT_START_ID_B, BIT_END_ID_B);
    // } else {
    //     printf("");
    // }
    
}

unsigned long long id_calc(unsigned long long id1,unsigned long long id2){
    return (id1 << 18) | id2;
}

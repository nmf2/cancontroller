#include <stdbool.h>
#include "../include/interface.h"
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

void printf_arr(bool* array, int end_index){
    //printf("|");
    int i = 0;
    for (; i <= end_index; i++){
        //printf("%d|", array[i]);
        printf("%d", array[i]);
    }
    printf("\n");
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

void print_frame(Frame frm){
    printf("Attributes:\n");
    printf("Extended: %s\n", frm.extended ? "Yes" : "No");
    printf("Type: %s\n", frm.type ? "REMOTE" : "DATA");
    printf("Frame size: %d\n", frm.frame_size);
    printf("Payload size: %d bytes\n\n", frm.payload_size);

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


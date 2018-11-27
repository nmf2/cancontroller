#include <stdbool.h>
#include "../include/interface.h"
// Convert part of an array of bits into an integer
int bits_to_int(int start, int end, bool * data){
    int i, value = 0;

    for (i = start; i <= end; i++){
        value = (value << 1) | data[i];
    }

    return value;
}
     
int int_to_bits(int value, bool * array){
    /*
        Params:
            value: The int value to transform into a bit array
            *array: The array where the result will be written into.
        Return: 
            Length of the resulting bit string
    */
    int i = 0, j = 0, size;
    bool inv_arr[32] = { 0 }; // will hold the inverted bit sequence
    // In case value == 0:
    array[0] = 0;
    while(value != 0){
        if(value % 2 == 1){
            inv_arr[i] = 1;
        } else {
            inv_arr[i] = 0;
        }
        value = value >> 1;
        i++;
    }
    
    size = i;

    for(--i, j = 0; i >= 0; i--, j++){
        array[j] = inv_arr[i];
    }

    return (size == 0) ? 1 : size; // Making sure size is at least 1.
}

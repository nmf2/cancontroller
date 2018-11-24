

// Convert part of an array of bits into an integer
int bits_to_int(int start, int end, bool * data){
    int i, value = 0;

    for (i = start; i <= end; i++){
        value = (value << 1) | data[i];
    }

    return value;
}

int min(int a, int b){
    return a < b ? a : b;
}
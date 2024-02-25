#include "formater.h"

extern Screen screen;

void Formater::Format(char *res, const char *str, ...){
    if (str == NULL || str[0] == EOL) {
        return;
    }

    va_list argPtr;
    va_start(argPtr, str);
    uint64_t num;
    uint32_t stri = 1, buffi = 0;
    char prevChar = str[stri - 1], currChar = str[stri];
    while(currChar != EOL){
        if (prevChar == FORMAT_SIGN && currChar != FORMAT_SIGN){
            switch (currChar) {
                case LONG_SIGN:{
                    num = va_arg(argPtr, int64_t);
                    buffi += putNumber(res, buffi, num);
                    break;
                }
                case DECIMAL_SIGN:{
                    num = va_arg(argPtr, uint32_t);
                    buffi += putNumber(res, buffi, num);
                    break;
                }
                case HEX_SIGN:{
                    num = va_arg(argPtr, uint32_t);
                    buffi += putNumberAsHex(res, buffi, num);
                    break;
                }
                case LONG_HEX_SIGN:{
                    num = va_arg(argPtr, uint64_t);
                    buffi += putNumberAsHex(res, buffi, num);
                    break;
                }
                case BIN_SIGN:{
                    num = va_arg(argPtr, uint32_t);
                    buffi += putNumberAsBin(res, buffi, num);
                    break;
                }
                case PTR_SIGN:{
                    num = va_arg(argPtr, uintptr_t);
                    buffi += putNumberAsHex(res, buffi, num);
                    break;
                }
            }
            prevChar = currChar;
            currChar = str[++stri];
            continue;
        }

        if (stri - 1 == 0){
            res[buffi++] = str[stri-1];
        }

        if (currChar != '%'){
            res[buffi++] = str[stri];
        }
        prevChar = currChar;
        currChar = str[++stri];
    }
    res[buffi] = '\0';
    return;
}

uint32_t Formater::putNumber(char *buf, uint32_t startIdx, uint64_t num){
    char bf[20]; // max number 2^64 - 1 is 20 chars
    int li = 19;
    uint32_t changes = 0;

    if (num == 0){
        buf[startIdx] = '0';
        return 1;
    }

    for (;num > 0;) {
        int res = num % 10;
        num = num / 10;
        bf[li] = '0' + res;
        li--;
        changes++;
    }

    for (li++;li < 20; li++){
        buf[startIdx++] = bf[li];
    }
    return changes;
}

uint32_t Formater::putNumberAsHex(char *buf, uint32_t startIdx, uint64_t num){
    char bf[18]; // 64 bits number can be represented in max 18 hex chars
    int li = 17;
    uint32_t changes = 0;

    if (num == 0){
        buf[startIdx] = '0';
        return 1;

    }

    for (;num > 0;) {
        int res = num & 0xf;
        num = num >> 4;
        if (res < 10) {
            bf[li] = '0' + res;
        } else {
            switch(res){
                case 10:
                    bf[li] = 'A';
                    break;
                case 11:
                    bf[li] = 'B';
                    break;
                case 12:
                    bf[li] = 'C';
                    break;
                case 13:
                    bf[li] = 'D';
                    break;
                case 14:
                    bf[li] = 'E';
                    break;
                case 15:
                    bf[li] = 'F';
                    break;
                // TODO: handle here.
            }
        }
        li--;
        changes++;
    }

    li++;
    for (;li < 64; li++){
        buf[startIdx++] = bf[li];
    }
    return changes;
}

uint32_t Formater::putNumberAsBin(char *buf, uint32_t startIdx, uint64_t num){
    char bf[64];
    int li = 63;
    uint32_t changes = 0;

    if (num == 0){
        buf[startIdx] = '0';
        return 1;
    }

    for (;num > 0;) {
        int res = num & 1;
        num = num >> 1;
        bf[li] = res == 0 ? '0' : '1';
        li--;
        changes++;
    }

    for (li++;li < 64; li++){
        buf[startIdx++] = bf[li];
    }
    return changes;
}

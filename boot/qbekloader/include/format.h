#ifndef FORMATER_H
#define FORMATER_H

#include "include/common.h"

#define FORMATER_BUFFER_SIZE_BYTES 2048
#define EOL '\0'
#define FORMAT_SIGN '%'
#define DECIMAL_SIGN 'd'
#define HEX_SIGN 'x'
#define LONG_HEX_SIGN 'X'
#define BIN_SIGN 'b'
#define LONG_SIGN 'l'
#define PTR_SIGN 'p'

void sprintf(char *res, const char *str, ...);

#endif /* FORMATER_H */

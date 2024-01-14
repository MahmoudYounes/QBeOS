#ifndef FORMATER_H
#define FORMATER_H

#include "common.h"

#define FORMATER_BUFFER_SIZE_BYTES 2048
#define EOL '\0'
#define FORMAT_SIGN '%'
#define DECIMAL_SIGN 'd'
#define HEX_SIGN 'x'
#define BIN_SIGN 'b'
#define LONG_SIGN 'l'
#define PTR_SIGN 'p'


// TODO: There is a bad, a very bad practice here. I am doing conversion to string
// and appending to buffer in putX[AsY] functions. this is because I don't know
// how to allocate memory yet. once I can allocate memory "virtual Mem" or have
// a nice heap implementation that allows me to implement new, this should be
// revisited. This will allow me then to have the conversion logic in a separate
// class that can be reused somewhere else and will simplify the internal
// implementation.
class Formater {
    private:
        uint32_t putNumber(char *buf, uint8_t startIdx, uint64_t num);
        uint32_t putNumberAsHex(char *buf, uint8_t startIdx, uint64_t num);
        uint32_t putNumberAsBin(char *buf, uint8_t startIdx, uint64_t num);
    public:
        void Format(char *res, const char *str, ...);

};

#define sprintf(resBuf, str, ...) Formater().Format(resBuf, str, __VA_ARGS__)
#define printf(resBuf, str, ...)                \
    do {                                        \
    sprintf(resBuf, str, __VA_ARGS__);          \
    screen.WriteString(resBuf);                 \
    } while (0)

#define print(str)                              \
    do {                                        \
    screen.WriteString(str);                    \
    } while (0)


#endif /* FORMATER_H */

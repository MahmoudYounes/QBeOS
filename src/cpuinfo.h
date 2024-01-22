#ifndef CPUINFO_H
#define CPUINFO_H

#include "common.h"
#include "cpuid.h"
#include "screen.h"
#include "formater.h"

enum CPUModel{
AMD,
AMDOLD,
INTEL,
};

class CPUInfo{
    private:
        // cpu model is stored in 12 bytes + 1 null byte
        char cpuModel[13];

        void getCPUModel();
        char *parseRegister();
    public:
        CPUInfo();

};


#endif /* CPUINFO_H */

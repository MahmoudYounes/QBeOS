#include "cpuinfo.h"

extern Screen screen;

CPUInfo::CPUInfo(){
    print("Detecting cpu...\n\0");

    getCPUModel();

    print("Detected cpu \0");
    print(cpuModel);
    print("\n");

    getCPUFeatures();
}

void CPUInfo::getCPUModel(){
    int unused, ebx, ecx, edx;
    __cpuid(0, unused, ebx, ecx, edx);

    // parse retgister
    char *mp = cpuModel;
    for (int i = 0; i < 4; i++){
        char c = ebx & 0xff;
        *mp++ = c;
        ebx >>= 8;
    }

    for (int i = 0; i < 4; i++){
        char c = edx & 0xff;
        *mp++ = c;
        edx >>= 8;
    }

    for (int i = 0; i < 4; i++){
        char c = ecx & 0xff;
        *mp++ = c;
        ecx >>= 8;
    }

    *mp ='\0';
}

void CPUInfo::getCPUFeatures(){
    int unused, ecx, edx;
    __cpuid(1, unused, unused, ecx, edx);

    cpuFeaturesECX = ecx;
    cpuFeaturesEDX = edx;
}

bool CPUInfo::IsCPUFeatureSupported(CPUFeatureECX feature){
    return cpuFeaturesECX & feature;
}

bool CPUInfo::IsCPUFeatureSupported(CPUFeatureEDX feature){
    return cpuFeaturesEDX & feature;
}



CPUInfo cpu;

#include "arch/x86/include/cpuinfo.h"

CPUInfo::CPUInfo() {
  kprint("Detecting cpu...\n\0");

  getCPUModel();

  kprint("Detected cpu:\n\0");
  kprint(
      cpuModel); // TODO: support string formatter and remove these three lines

  getCPUFeatures();
}

void CPUInfo::getCPUModel() {
  int eax, ebx, ecx, edx;
  __cpuid(0, eax, ebx, ecx, edx);

  maxSupportedValue = eax;

  // parse retgister
  char *mp = cpuModel;
  for (int i = 0; i < 4; i++) {
    char c = ebx & 0xff;
    *mp++ = c;
    ebx >>= 8;
  }

  for (int i = 0; i < 4; i++) {
    char c = edx & 0xff;
    *mp++ = c;
    edx >>= 8;
  }

  for (int i = 0; i < 4; i++) {
    char c = ecx & 0xff;
    *mp++ = c;
    ecx >>= 8;
  }

  *mp = '\0';
}

void CPUInfo::getCPUFeatures() {
  int unused, ebx, ecx, edx;
  __cpuid(1, unused, ebx, ecx, edx);

  cpuFeaturesECX = ecx;
  cpuFeaturesEDX = edx;
}

bool CPUInfo::IsCPUFeatureSupported(CPUFeatureECX feature) {
  return cpuFeaturesECX & feature;
}

bool CPUInfo::IsCPUFeatureSupported(CPUFeatureEDX feature) {
  return cpuFeaturesEDX & feature;
}

uint32_t CPUInfo::getCPUMaxLogicalAddress() {
  int unused, ebx, ecx, edx;
  __cpuid(1, unused, ebx, ecx, edx);

  return ebx & 0xff000000;
}

uint32_t CPUInfo::GetInitialAPICID() {
  int eax = 1, ebx, unused;
  __cpuid(0, eax, ebx, unused, unused);

  return ebx >> 24;
}

bool CPUInfo::IsConstantTimer() {
  int eax, unused;
  __cpuid(0x6, eax, unused, unused, unused);

  return eax & 0x2;
}

uint32_t CPUInfo::GetClockFrequency() {
  int eax, ebx, ecx, edx;

  if (maxSupportedValue > 0x15) {
    __cpuid(0x15, eax, ebx, ecx, edx);
    return 0;
  } else {
    panic("can not read TSC\n\0");
  }

  // TODO: is this the right value?
  return 1;
}

CPUInfo cpu;

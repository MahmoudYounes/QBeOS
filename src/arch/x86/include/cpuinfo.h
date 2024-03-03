#ifndef CPUINFO_H
#define CPUINFO_H

#include "cpuid.h"
#include "include/common.h"
#include "include/logger.h"

// TODO: imagine you provide a shell command like lscpu. this class needs to
// have and support reading all CPU information. todo that, check the table 3-8
// in intel manual vol 2 page 814

enum CPUModel {
  AMD,
  AMDOLD,
  INTEL,
};

enum CPUFeatureECX {
  CPU_FEAT_SSE3 = 1 << 0,
  CPU_FEAT_PCLMUL = 1 << 1,
  CPU_FEAT_DTES64 = 1 << 2,
  CPU_FEAT_MONITOR = 1 << 3,
  CPU_FEAT_DS_CPL = 1 << 4,
  CPU_FEAT_VMX = 1 << 5,
  CPU_FEAT_SMX = 1 << 6,
  CPU_FEAT_EST = 1 << 7,
  CPU_FEAT_TM2 = 1 << 8,
  CPU_FEAT_SSSE3 = 1 << 9,
  CPU_FEAT_CID = 1 << 10,
  CPU_FEAT_SDBG = 1 << 11,
  CPU_FEAT_FMA = 1 << 12,
  CPU_FEAT_CX16 = 1 << 13,
  CPU_FEAT_XTPR = 1 << 14,
  CPU_FEAT_PDCM = 1 << 15,
  CPU_FEAT_PCID = 1 << 17,
  CPU_FEAT_DCA = 1 << 18,
  CPU_FEAT_SSE4_1 = 1 << 19,
  CPU_FEAT_SSE4_2 = 1 << 20,
  CPU_FEAT_X2APIC = 1 << 21,
  CPU_FEAT_MOVBE = 1 << 22,
  CPU_FEAT_POPCNT = 1 << 23,
  CPU_FEAT_TSC_DEADLINE = 1 << 24,
  CPU_FEAT_AES = 1 << 25,
  CPU_FEAT_XSAVE = 1 << 26,
  CPU_FEAT_OSXSAVE = 1 << 27,
  CPU_FEAT_AVX = 1 << 28,
  CPU_FEAT_F16C = 1 << 29,
  CPU_FEAT_RDRAND = 1 << 30,
  CPU_FEAT_HYPERVISOR = 1 << 31
};

enum CPUFeatureEDX {
  CPU_FEAT_FPU = 1 << 0,
  CPU_FEAT_VME = 1 << 1,
  CPU_FEAT_DE = 1 << 2,
  CPU_FEAT_PSE = 1 << 3,
  CPU_FEAT_TSC = 1 << 4,
  CPU_FEAT_MSR = 1 << 5,
  CPU_FEAT_PAE = 1 << 6,
  CPU_FEAT_MCE = 1 << 7,
  CPU_FEAT_CX8 = 1 << 8,
  CPU_FEAT_APIC = 1 << 9,
  CPU_FEAT_SEP = 1 << 11,
  CPU_FEAT_MTRR = 1 << 12,
  CPU_FEAT_PGE = 1 << 13,
  CPU_FEAT_MCA = 1 << 14,
  CPU_FEAT_CMOV = 1 << 15,
  CPU_FEAT_PAT = 1 << 16,
  CPU_FEAT_PSE36 = 1 << 17,
  CPU_FEAT_PSN = 1 << 18,
  CPU_FEAT_CLFLUSH = 1 << 19,
  CPU_FEAT_DS = 1 << 21,
  CPU_FEAT_ACPI = 1 << 22,
  CPU_FEAT_MMX = 1 << 23,
  CPU_FEAT_FXSR = 1 << 24,
  CPU_FEAT_SSE = 1 << 25,
  CPU_FEAT_SSE2 = 1 << 26,
  CPU_FEAT_SS = 1 << 27,
  CPU_FEAT_HTT = 1 << 28,
  CPU_FEAT_TM = 1 << 29,
  CPU_FEAT_IA64 = 1 << 30,
  CPU_FEAT_PBE = 1 << 31
};

class CPUInfo {
private:
  // cpu model is stored in 12 bytes + 1 null byte
  char cpuModel[13];
  uint32_t cpuFeaturesECX;
  uint32_t cpuFeaturesEDX;

  void getCPUModel();
  void getCPUFeatures();
  uint32_t getCPUMaxLogicalAddress();

public:
  CPUInfo();
  bool IsCPUFeatureSupported(CPUFeatureECX feature);
  bool IsCPUFeatureSupported(CPUFeatureEDX feature);
};

#endif /* CPUINFO_H */

#include "acpi/include/facp.h"

FACPM::FACPM(){}
FACPM::FACPM(uintptr_t addr){
  bool valid;

  valid = validateTableSignature(addr);
  if (!valid){
    panic("found invalid FACP/FADT table: incorrect signature\n\0");
  }

  memcpy((void *)&facphdr, (void *)addr, sizeof(FACPHDR));

  if (!validateChecksum()){
    panic("found invalid FACP/FADT table: incorrect csum\n\0");
  }
}

bool FACPM::validateTableSignature(uintptr_t addr){
  char *sig = new char[5];
  memcpy((void *)sig, (void *)addr, 4);
  sig[4] = '\0';

  if (strcmp(sig, "FACP\0") != 0) {
    return false;
  }
  return true;
}

bool FACPM::validateChecksum(){
  uint32_t csum, length;
  
  length = *(uint32_t *)facphdr.length;
  csum = calculateChecksum((uintptr_t)&facphdr, length);  
  if (csum) {
    return false;
  }
  return true;
}

bool FACPM::IsPS2Supported(){
  uint16_t flags = *(uint16_t *)facphdr.bootArchitectureFlags; 
  return flags & 2;
}

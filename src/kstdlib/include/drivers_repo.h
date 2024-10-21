#ifndef DRIVERS_REPO_H
#define DRIVERS_REPO_H

#include "include/common.h"
#include "include/logger.h"
#include "drivers/include/driver.h"

// This class manages the drivers that are supported by the kernel 
class DriversRepo{
public:
  DriversRepo();
  void RegisterDriver(const char* name, Driver* driver);
  Driver *GetDriver(const char* name);
};

#endif

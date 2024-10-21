#ifndef DRIVER_H
#define DRIVER_H

/**
 * Driver interface
 * Initialize: initializes the driver
 * Handler: does the driver operation. this can be anything from interrupt handler
 * to just writing some data somewhere
 */
class Driver{
  public:
  virtual void Initialize() = 0;
  virtual void Handler() = 0;
};

#endif

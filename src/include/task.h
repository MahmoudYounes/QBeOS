#ifndef TASK_H
#define TASK_H

/* design notes:
 *
 * target: abstract away task management from task state and info expected by
 * hardware. the task state and task loading facilities will be implemented per
 * hardware in the respective architecture the kernel is built for. the task
 * management will be implemented in the kernel.
 *
 * */

#include "arch/include/vmm.h"
#include "include/common.h"
#include "include/logger.h"

namespace Kernel {
// interface for task structures
class Task {
public:
  // every architecutre will have its own task class implemention. every task
  // implemention will have to load the task in its own way.
  virtual void LoadTask() = 0;

  Task *next;
  Task *prev;
};

// the kernel task manager
class TaskManager {
private:
  // linked list of tasks
  Task *initTask;

public:
  TaskManager();
  Task *CreateTask();
};

} // namespace Kernel
#endif /* TASK_H */

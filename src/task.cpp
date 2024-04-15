#include "include/task.h"
// this include has to be here to avoid cyclic inclusion
#include "arch/x86/include/task.h"

using namespace Kernel;

TaskManager::TaskManager() { initTask = ::Task::CreateTask(); }

Kernel::Task *TaskManager::CreateTask() {
  Task *newTask = ::Task::CreateTask();
  return newTask;
}

TaskManager taskManager;

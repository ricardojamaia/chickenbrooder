#ifndef PERSISTENCE_MANAGER_H
#define PERSISTENCE_MANAGER_H

#include <Preferences.h>
#include "State.h"

class PresistanceManager {
  public:
  PresistanceManager(const char* namespaceName = "AppNamespace");
    ~PresistanceManager();

    void begin();
    void end();

    template <typename T>
    void manageState(State<T>* state, const char* key);

  private:
    Preferences preferences;
    const char* namespaceName;
};

#endif // PERSISTENCE_MANAGER_H
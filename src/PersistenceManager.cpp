#include "PresistanceManager.h"

PresistanceManager::PresistanceManager(const char* namespaceName)
    : namespaceName(namespaceName) {

}

PresistanceManager::~PresistanceManager() {
}

void PresistanceManager::begin() {
    preferences.begin(namespaceName, false); // Open the Preferences namespace
}
void PresistanceManager::end() {
    preferences.end(); // Close the Preferences library
}

template <typename T>
void PresistanceManager::manageState(State<T>& state, const char* key) {
    // Load the persisted value if it exists, otherwise use the default value
    DEBUG_BROODER_PRINT("Loading state for key: ");
    DEBUG_BROODER_PRINT(key);
    if (preferences.isKey(key)) {
        DEBUG_BROODER_PRINT("Found persisted value for key: ");
        DEBUG_BROODER_PRINT(key);
    } else {
        DEBUG_BROODER_PRINT("No persisted value found for key: ");
        DEBUG_BROODER_PRINT(key);
        return;
    }
    
    if constexpr (std::is_same<T, float>::value) {
        state.setValue(preferences.getFloat(key, state.getValue()));
    } else if constexpr (std::is_same<T, int>::value) {
        state.setValue(preferences.getInt(key, state.getValue()));
    } else if constexpr (std::is_same<T, bool>::value) {
        state.setValue(preferences.getBool(key, state.getValue()));
    }

    DEBUG_BROODER_PRINT("Persisted value: ");
    DEBUG_BROODER_PRINTLN(state.getValue());

    // Attach a listener to persist the state whenever it changes
    state.addListener([this, key](T newValue, void* context) {
        DEBUG_BROODER_PRINT("Persisting state for key: ");
        DEBUG_BROODER_PRINT(key);
        if constexpr (std::is_same<T, float>::value) {
            preferences.putFloat(key, newValue);
        } else if constexpr (std::is_same<T, int>::value) {
            preferences.putInt(key, newValue);
        } else if constexpr (std::is_same<T, bool>::value) {
            preferences.putBool(key, newValue);
        }
    });
}

template void PresistanceManager::manageState<float>(State<float>&, const char*);
template void PresistanceManager::manageState<int>(State<int>&, const char*);
template void PresistanceManager::manageState<bool>(State<bool>&, const char*);

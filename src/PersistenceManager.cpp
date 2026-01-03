#include "PresistanceManager.h"
#include <DebugBrooder.h>

PresistanceManager::PresistanceManager(const char* namespaceName)
    : namespaceName(namespaceName) {

}

PresistanceManager::~PresistanceManager() {
}

void PresistanceManager::begin() {
    if (!preferences.begin(namespaceName, false)) {
        DEBUG_BROODER_PRINTLN("Failed to initialize preferences with namespace.");
    } else {
        DEBUG_BROODER_PRINTLN("Preferences initialized successfully.");
    }
}
void PresistanceManager::end() {
    preferences.end(); // Close the Preferences library
}

template <typename T>
void PresistanceManager::manageState(State<T>* state, const char* key) {
    // Load the persisted value if it exists, otherwise use the default value
    DEBUG_BROODER_PRINT("Loading state for key: ");
    DEBUG_BROODER_PRINTLN(key);
    if (preferences.isKey(key)) {
        DEBUG_BROODER_PRINT("Found persisted value for key: ");
        DEBUG_BROODER_PRINTLN(key);

        if (std::is_same<T, float>::value) {
            state->setValue(preferences.getFloat(key, state->getValue()));
        } else if (std::is_same<T, int>::value) {
            state->setValue(preferences.getInt(key, state->getValue()));
        } else if (std::is_same<T, bool>::value) {
            state->setValue(preferences.getBool(key, state->getValue()));
        }
    
        DEBUG_BROODER_PRINT("Persisted value: ");
        DEBUG_BROODER_PRINTLN(state->getValue());
    } else {
        DEBUG_BROODER_PRINT("No persisted value found for key: ");
        DEBUG_BROODER_PRINTLN(key);
    }

    // Attach a listener to persist the state whenever it changes
    state->addListener([this, key](T newValue) {
        size_t writtenBytes = 9999;
            if (key != nullptr && strlen(key) > 0) {
                writtenBytes = preferences.putFloat(key, newValue);
            } else {
                DEBUG_BROODER_PRINTLN("Invalid key provided for persisting float value.");
                writtenBytes = 0;
            }
        DEBUG_BROODER_PRINTLN(key);
        if (std::is_same<T, float>::value) {
            DEBUG_BROODER_PRINT("Persisting float value.");
            writtenBytes = preferences.putFloat(key, newValue);
        } else if (std::is_same<T, int>::value) {
            DEBUG_BROODER_PRINT("Persisting int value.");
            writtenBytes = preferences.putInt(key, newValue);
        } else if (std::is_same<T, bool>::value) {
            DEBUG_BROODER_PRINT("Persisting bool value.");
            writtenBytes = preferences.putBool(key, newValue);
        }
        DEBUG_BROODER_PRINT("Persisting state for key: ");
        DEBUG_BROODER_PRINT(key);
        DEBUG_BROODER_PRINT(" with ");
        DEBUG_BROODER_PRINTLN(preferences.isKey(key) ? "success" : "failure");
        DEBUG_BROODER_PRINT("Written bytes: ");
        DEBUG_BROODER_PRINTLN(writtenBytes);
    });

}

template void PresistanceManager::manageState<float>(State<float>*, const char*);
template void PresistanceManager::manageState<int>(State<int>*, const char*);
template void PresistanceManager::manageState<bool>(State<bool>*, const char*);

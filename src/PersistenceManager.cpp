#include "PresistanceManager.h"
#include <BrooderLog.h>

PresistanceManager::PresistanceManager(const char* namespaceName)
    : namespaceName(namespaceName) {

}

PresistanceManager::~PresistanceManager() {
}

void PresistanceManager::begin() {
    if (!preferences.begin(namespaceName, false)) {
        BROODER_LOG_E("Failed to initialize preferences with namespace.");
    } else {
        BROODER_LOG_D("Preferences initialized successfully.");
    }
}
void PresistanceManager::end() {
    preferences.end(); // Close the Preferences library
}

template <typename T>
void PresistanceManager::manageState(State<T>* state, const char* key) {
    // Load the persisted value if it exists, otherwise use the default value
    BROODER_LOG_D("Loading state for key: %s", key);
    if (preferences.isKey(key)) {
        if (std::is_same<T, float>::value) {
            state->setValue(preferences.getFloat(key, state->getValue()));
            BROODER_LOG_D("Found persisted value for key: %s -> %f", key, state->getValue());
        } else if (std::is_same<T, int>::value) {
            state->setValue(preferences.getInt(key, state->getValue()));
            BROODER_LOG_D("Found persisted value for key: %s -> %d", key, state->getValue());
        } else if (std::is_same<T, bool>::value) {
            state->setValue(preferences.getBool(key, state->getValue()));
            BROODER_LOG_D("Found persisted value for key: %s -> %s", key, state->getValue() ? "True" : "False");
        }
    } else {
        BROODER_LOG_D("No persisted value found for key: %s", key);
    }

    // Attach a listener to persist the state whenever it changes
    state->addListener([this, key](T newValue) {
        size_t writtenBytes = 9999;
            if (key != nullptr && strlen(key) > 0) {
                writtenBytes = preferences.putFloat(key, newValue);
            } else {
                BROODER_LOG_E("Invalid key provided for persisting float value.");
                writtenBytes = 0;
            }
        if (std::is_same<T, float>::value) {
            BROODER_LOG_D("Persisting float value: %f", newValue);
            writtenBytes = preferences.putFloat(key, newValue);
        } else if (std::is_same<T, int>::value) {
            BROODER_LOG_D("Persisting int value: %d", newValue);
            writtenBytes = preferences.putInt(key, newValue);
        } else if (std::is_same<T, bool>::value) {
            BROODER_LOG_D("Persisting bool value: %s", newValue ? "True" : "False");
            writtenBytes = preferences.putBool(key, newValue);
        }
        BROODER_LOG_D("Persisting state for key: %s with %s. Written bytes: %d", key, preferences.isKey(key) ? "success" : "failure", writtenBytes);
    });

}

template void PresistanceManager::manageState<float>(State<float>*, const char*);
template void PresistanceManager::manageState<int>(State<int>*, const char*);
template void PresistanceManager::manageState<bool>(State<bool>*, const char*);

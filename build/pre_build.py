import os
import re

# Import the PlatformIO build environment
Import("env")

# Path to the version.h file
version_file_path = os.path.join(env["PROJECT_INCLUDE_DIR"], "version.h")

# Increment the build version
def increment_build_version():
    if not os.path.exists(version_file_path):
        # Create the version.h file if it doesn't exist
        with open(version_file_path, "w") as version_file:
            version_file.write("#ifndef VERSION_H\n")
            version_file.write("#define VERSION_H\n\n")
            version_file.write("#define BUILD_VERSION 1\n")
            version_file.write("#define BUILD_DATE __DATE__\n")
            version_file.write("#define BUILD_TIME __TIME__\n\n")
            version_file.write("#endif // VERSION_H\n")
        print(f"Created {version_file_path} with BUILD_VERSION 1")
        return

    # Read the current version.h file
    with open(version_file_path, "r") as version_file:
        content = version_file.readlines()

    # Find and increment the BUILD_VERSION
    new_content = []
    version_updated = False
    for line in content:
        match = re.match(r"#define BUILD_VERSION (\d+)", line)
        if match:
            current_version = int(match.group(1))
            new_version = current_version + 1
            new_content.append(f"#define BUILD_VERSION {new_version}\n")
            version_updated = True
        else:
            new_content.append(line)

    # If BUILD_VERSION was not found, add it
    if not version_updated:
        new_content.insert(1, "#define BUILD_VERSION 1\n")

    # Write the updated version.h file
    with open(version_file_path, "w") as version_file:
        version_file.writelines(new_content)

    print(f"Updated {version_file_path} with new BUILD_VERSION")

def check_secrets(*args, **kwargs):
    secrets_path = "include/secrets.h"
    
    # If secrets.h already exists, do nothing
    if os.path.exists(secrets_path):
        print("✅  secrets.h found!")
        return
    
    print("\n" + "="*60)
    print("⚠️  secrets.h not found - Let's create it!")
    print("="*60 + "\n")
    
    # Prompt for each value
    print("WiFi SSID: ")
    wifi_ssid = input().strip()
    print("WiFi Password: ")
    wifi_password = input().strip()
    print("MQTT Server (IP or hostname): ")
    mqtt_server = input().strip()
    print("MQTT Port (default 1883): ")
    mqtt_port = input().strip() or "1883"
    print("MQTT Username: ")
    mqtt_user = input().strip()
    print("MQTT Password: ")
    mqtt_password = input().strip()
    
    # Create the secrets.h content
    secrets_content = f"""#ifndef SECRETS_H
#define SECRETS_H

// WiFi credentials
#define WIFI_SSID "{wifi_ssid}"
#define WIFI_PASSWORD "{wifi_password}"

// MQTT configuration
#define MQTT_SERVER "{mqtt_server}"
#define MQTT_PORT {mqtt_port}
#define MQTT_USER "{mqtt_user}"
#define MQTT_PASSWORD "{mqtt_password}"

#endif
"""
    
    # Ensure include directory exists
    os.makedirs("include", exist_ok=True)
    
    # Write the file
    with open(secrets_path, 'w') as f:
        f.write(secrets_content)
    
    print("\n" + "="*60)
    print(f"✓ Created {secrets_path}")
    print("="*60 + "\n")
    print("You can now build your project!")
    print(f"To change values later, edit: {secrets_path}")
    print("="*60 + "\n")

# Check if secrets exist and create one if necessary
check_secrets()

# Increment the build version before the build starts
increment_build_version()
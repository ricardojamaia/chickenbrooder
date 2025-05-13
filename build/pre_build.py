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

# Increment the build version before the build starts
increment_build_version()
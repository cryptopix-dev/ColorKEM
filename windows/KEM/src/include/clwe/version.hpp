#ifndef CLWE_VERSION_HPP
#define CLWE_VERSION_HPP

#define CLWE_VERSION_MAJOR 1
#define CLWE_VERSION_MINOR 0
#define CLWE_VERSION_PATCH 0
#define CLWE_VERSION_STRING "1.0.0"

namespace clwe {

// Version information
constexpr int VERSION_MAJOR = CLWE_VERSION_MAJOR;
constexpr int VERSION_MINOR = CLWE_VERSION_MINOR;
constexpr int VERSION_PATCH = CLWE_VERSION_PATCH;
constexpr const char* VERSION_STRING = CLWE_VERSION_STRING;

// Get version as string
inline const char* get_version_string() {
    return VERSION_STRING;
}

// Get version components
inline int get_version_major() { return VERSION_MAJOR; }
inline int get_version_minor() { return VERSION_MINOR; }
inline int get_version_patch() { return VERSION_PATCH; }

} // namespace clwe

#endif // CLWE_VERSION_HPP
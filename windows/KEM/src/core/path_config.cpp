#include "path_config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <windows.h>

namespace clwe {
namespace config {

// PathConfig implementation
PathConfig& PathConfig::getInstance() {
    static PathConfig instance;
    return instance;
}

bool PathConfig::loadFromFile(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << config_file << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Store in paths map
            paths_[key] = expandEnvironmentVariables(value);
        }
    }
    
    file.close();
    return true;
}

bool PathConfig::loadFromEnvironment() {
    // Load paths from environment variables
    const char* env_vars[] = {
        "COLOR_KEM_KEY_OUTPUT_DIR",
        "COLOR_KEM_KEY_INPUT_DIR", 
        "COLOR_KEM_TEMP_DIR",
        "COLOR_KEM_LOG_DIR",
        "COLOR_KEM_LIB_DIR",
        "COLOR_KEM_INCLUDE_DIR",
        "COLOR_KEM_ENV"
    };
    
    const char* path_keys[] = {
        "key_output_dir",
        "key_input_dir",
        "temp_dir", 
        "log_dir",
        "lib_dir",
        "include_dir",
        "environment"
    };
    
    for (size_t i = 0; i < sizeof(env_vars) / sizeof(env_vars[0]); ++i) {
        const char* value = std::getenv(env_vars[i]);
        if (value != nullptr) {
            paths_[path_keys[i]] = expandEnvironmentVariables(value);
        }
    }
    
    return true;
}

bool PathConfig::loadFromArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--key-output-dir" && i + 1 < argc) {
            setKeyOutputDirectory(argv[++i]);
        } else if (arg == "--key-input-dir" && i + 1 < argc) {
            setKeyInputDirectory(argv[++i]);
        } else if (arg == "--temp-dir" && i + 1 < argc) {
            setTemporaryDirectory(argv[++i]);
        } else if (arg == "--log-dir" && i + 1 < argc) {
            setLogDirectory(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            loadFromFile(argv[++i]);
        } else if (arg == "--env" && i + 1 < argc) {
            setEnvironment(argv[++i]);
        }
    }
    
    return true;
}

void PathConfig::setKeyOutputDirectory(const std::string& path) {
    paths_["key_output_dir"] = expandEnvironmentVariables(path);
}

void PathConfig::setKeyInputDirectory(const std::string& path) {
    paths_["key_input_dir"] = expandEnvironmentVariables(path);
}

void PathConfig::setTemporaryDirectory(const std::string& path) {
    paths_["temp_dir"] = expandEnvironmentVariables(path);
}

void PathConfig::setLogDirectory(const std::string& path) {
    paths_["log_dir"] = expandEnvironmentVariables(path);
}

void PathConfig::setLibraryDirectory(const std::string& path) {
    paths_["lib_dir"] = expandEnvironmentVariables(path);
}

void PathConfig::setIncludeDirectory(const std::string& path) {
    paths_["include_dir"] = expandEnvironmentVariables(path);
}

std::string PathConfig::getKeyOutputDirectory() const {
    auto it = paths_.find("key_output_dir");
    if (it != paths_.end()) {
        return it->second;
    }
    return getCurrentDirectory(); // Default to current directory
}

std::string PathConfig::getKeyInputDirectory() const {
    auto it = paths_.find("key_input_dir");
    if (it != paths_.end()) {
        return it->second;
    }
    return getCurrentDirectory(); // Default to current directory
}

std::string PathConfig::getTemporaryDirectory() const {
    auto it = paths_.find("temp_dir");
    if (it != paths_.end()) {
        return it->second;
    }
    
    // Fallback to system temp directory
    char temp_path[MAX_PATH];
    if (GetTempPathA(MAX_PATH, temp_path) > 0) {
        return std::string(temp_path) + "ColorKEM\\";
    }
    return getCurrentDirectory() + "\\temp\\";
}

std::string PathConfig::getLogDirectory() const {
    auto it = paths_.find("log_dir");
    if (it != paths_.end()) {
        return it->second;
    }
    return getCurrentDirectory() + "\\logs\\";
}

std::string PathConfig::getLibraryDirectory() const {
    auto it = paths_.find("lib_dir");
    if (it != paths_.end()) {
        return it->second;
    }
    return getApplicationDirectory() + "\\lib\\";
}

std::string PathConfig::getIncludeDirectory() const {
    auto it = paths_.find("include_dir");
    if (it != paths_.end()) {
        return it->second;
    }
    return getApplicationDirectory() + "\\include\\";
}

std::string PathConfig::getKeyFilePath(const std::string& filename, bool is_output) const {
    std::string directory = is_output ? getKeyOutputDirectory() : getKeyInputDirectory();
    return path_utils::joinPaths(directory, filename);
}

std::string PathConfig::getLogFilePath(const std::string& filename) const {
    return path_utils::joinPaths(getLogDirectory(), filename);
}

std::string PathConfig::getTemporaryFilePath(const std::string& filename) const {
    return path_utils::joinPaths(getTemporaryDirectory(), filename);
}

bool PathConfig::validatePaths() const {
    // Check if critical directories exist or can be created
    std::vector<std::string> critical_dirs = {
        getKeyOutputDirectory(),
        getKeyInputDirectory(),
        getTemporaryDirectory(),
        getLogDirectory()
    };
    
    for (const auto& dir : critical_dirs) {
        if (!path_utils::directoryExists(dir) && !path_utils::createDirectory(dir)) {
            std::cerr << "Error: Cannot create or access directory: " << dir << std::endl;
            return false;
        }
    }
    
    return true;
}

bool PathConfig::createDirectories() const {
    std::vector<std::string> dirs = {
        getKeyOutputDirectory(),
        getKeyInputDirectory(),
        getTemporaryDirectory(),
        getLogDirectory()
    };
    
    bool success = true;
    for (const auto& dir : dirs) {
        if (!path_utils::createDirectory(dir)) {
            std::cerr << "Warning: Could not create directory: " << dir << std::endl;
            success = false;
        }
    }
    
    return success;
}

void PathConfig::setDefaultPaths() {
    // Set platform-appropriate default paths
    setKeyOutputDirectory(getCurrentDirectory());
    setKeyInputDirectory(getCurrentDirectory());
    setTemporaryDirectory(getTemporaryDirectory());
    setLogDirectory(getLogDirectory());
    setLibraryDirectory(getLibraryDirectory());
    setIncludeDirectory(getIncludeDirectory());
}

void PathConfig::setEnvironment(const std::string& env) {
    environment_ = env;
    
    // Apply environment-specific presets
    if (env == "development") {
        presets::applyDevelopmentConfig(*this);
    } else if (env == "production") {
        presets::applyProductionConfig(*this);
    } else if (env == "testing") {
        presets::applyTestingConfig(*this);
    } else if (env == "ci") {
        presets::applyCIConfig(*this);
    }
}

std::string PathConfig::getEnvironment() const {
    return environment_;
}

std::string PathConfig::expandEnvironmentVariables(const std::string& path) const {
    std::string result = path;
    size_t pos = 0;
    
    // Find and replace %ENV_VAR% patterns
    while ((pos = result.find('%', pos)) != std::string::npos) {
        size_t end_pos = result.find('%', pos + 1);
        if (end_pos != std::string::npos) {
            std::string env_var = result.substr(pos + 1, end_pos - pos - 1);
            const char* env_value = std::getenv(env_var.c_str());
            if (env_value != nullptr) {
                result.replace(pos, end_pos - pos + 1, env_value);
                pos += strlen(env_value);
            } else {
                pos = end_pos + 1;
            }
        } else {
            break;
        }
    }
    
    return result;
}

std::string PathConfig::getCurrentDirectory() const {
    char buffer[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, buffer) > 0) {
        return std::string(buffer);
    }
    return ".";
}

std::string PathConfig::getHomeDirectory() const {
    // Try to get from environment variables first
    const char* home_env = std::getenv("USERPROFILE");
    if (home_env != nullptr) {
        return std::string(home_env);
    }
    
    // Fallback to current directory
    return getCurrentDirectory();
}

std::string PathConfig::getApplicationDirectory() const {
    char buffer[MAX_PATH];
    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) > 0) {
        std::string path = buffer;
        return path_utils::getParentDirectory(path);
    }
    return getCurrentDirectory();
}

// Path utilities implementation
namespace path_utils {
    std::string joinPaths(const std::string& path1, const std::string& path2) {
        if (path1.empty()) return path2;
        if (path2.empty()) return path1;
        
        std::string result = path1;
        if (result.back() != '\\' && result.back() != '/') {
            result += '\\';
        }
        result += path2;
        return result;
    }
    
    std::string getAbsolutePath(const std::string& relative_path) {
        char buffer[MAX_PATH];
        if (GetFullPathNameA(relative_path.c_str(), MAX_PATH, buffer, nullptr) > 0) {
            return std::string(buffer);
        }
        return relative_path;
    }
    
    std::string getParentDirectory(const std::string& path) {
        size_t pos = path.find_last_of("\\/");
        if (pos != std::string::npos) {
            return path.substr(0, pos);
        }
        return ".";
    }
    
    bool directoryExists(const std::string& path) {
        DWORD attributes = GetFileAttributesA(path.c_str());
        return (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
    }
    
    bool createDirectory(const std::string& path) {
        if (directoryExists(path)) {
            return true;
        }
        
        // Create parent directories first
        std::string parent = getParentDirectory(path);
        if (parent != "." && !directoryExists(parent)) {
            if (!createDirectory(parent)) {
                return false;
            }
        }
        
        return CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
    }
    
    std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern) {
        std::vector<std::string> files;
        std::string search_path = joinPaths(directory, pattern);
        
        WIN32_FIND_DATAA find_data;
        HANDLE handle = FindFirstFileA(search_path.c_str(), &find_data);
        
        if (handle != INVALID_HANDLE_VALUE) {
            do {
                if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    files.push_back(find_data.cFileName);
                }
            } while (FindNextFileA(handle, &find_data));
            FindClose(handle);
        }
        
        return files;
    }
}

// Configuration presets
namespace presets {
    void applyDevelopmentConfig(PathConfig& config) {
        config.setKeyOutputDirectory("./debug_output");
        config.setKeyInputDirectory("./debug_input");
        config.setTemporaryDirectory("./debug_temp");
        config.setLogDirectory("./debug_logs");
    }
    
    void applyProductionConfig(PathConfig& config) {
        config.setKeyOutputDirectory(config.getHomeDirectory() + "\\ColorKEM\\keys\\output");
        config.setKeyInputDirectory(config.getHomeDirectory() + "\\ColorKEM\\keys\\input");
        config.setTemporaryDirectory(config.getHomeDirectory() + "\\ColorKEM\\temp");
        config.setLogDirectory(config.getHomeDirectory() + "\\ColorKEM\\logs");
    }
    
    void applyTestingConfig(PathConfig& config) {
        config.setKeyOutputDirectory("./test_output");
        config.setKeyInputDirectory("./test_input");
        config.setTemporaryDirectory("./test_temp");
        config.setLogDirectory("./test_logs");
    }
    
    void applyCIConfig(PathConfig& config) {
        config.setKeyOutputDirectory("./ci_keys");
        config.setKeyInputDirectory("./ci_keys");
        config.setTemporaryDirectory("./ci_temp");
        config.setLogDirectory("./ci_logs");
    }
}

} // namespace config
} // namespace clwe
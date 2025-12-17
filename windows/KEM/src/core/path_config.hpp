#ifndef PATH_CONFIG_HPP
#define PATH_CONFIG_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace clwe {
namespace config {

// Configuration manager for dynamic path settings
class PathConfig {
public:
    // Singleton pattern for global access
    static PathConfig& getInstance();
    
    // Configuration loading methods
    bool loadFromFile(const std::string& config_file);
    bool loadFromEnvironment();
    bool loadFromArgs(int argc, char* argv[]);
    
    // Path configuration setters
    void setKeyOutputDirectory(const std::string& path);
    void setKeyInputDirectory(const std::string& path);
    void setTemporaryDirectory(const std::string& path);
    void setLogDirectory(const std::string& path);
    void setLibraryDirectory(const std::string& path);
    void setIncludeDirectory(const std::string& path);
    
    // Path configuration getters
    std::string getKeyOutputDirectory() const;
    std::string getKeyInputDirectory() const;
    std::string getTemporaryDirectory() const;
    std::string getLogDirectory() const;
    std::string getLibraryDirectory() const;
    std::string getIncludeDirectory() const;
    std::string getHomeDirectory() const;
    
    // File path helpers
    std::string getKeyFilePath(const std::string& filename, bool is_output = true) const;
    std::string getLogFilePath(const std::string& filename) const;
    std::string getTemporaryFilePath(const std::string& filename) const;
    
    // Configuration validation
    bool validatePaths() const;
    bool createDirectories() const;
    
    // Default configuration
    void setDefaultPaths();
    
    // Environment-specific configuration
    void setEnvironment(const std::string& env);
    std::string getEnvironment() const;
    
private:
    PathConfig() = default;
    ~PathConfig() = default;
    
    // Disable copy and assignment
    PathConfig(const PathConfig&) = delete;
    PathConfig& operator=(const PathConfig&) = delete;
    
    // Configuration storage
    std::unordered_map<std::string, std::string> paths_;
    std::string environment_;
    
    // Helper methods
    std::string expandEnvironmentVariables(const std::string& path) const;
    std::string getCurrentDirectory() const;
    std::string getApplicationDirectory() const;
};

// Utility functions for path operations
namespace path_utils {
    std::string joinPaths(const std::string& path1, const std::string& path2);
    std::string getAbsolutePath(const std::string& relative_path);
    std::string getParentDirectory(const std::string& path);
    bool directoryExists(const std::string& path);
    bool createDirectory(const std::string& path);
    std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern = "*");
}

// Configuration presets for different environments
namespace presets {
    void applyDevelopmentConfig(PathConfig& config);
    void applyProductionConfig(PathConfig& config);
    void applyTestingConfig(PathConfig& config);
    void applyCIConfig(PathConfig& config);
}

} // namespace config
} // namespace clwe

#endif // PATH_CONFIG_HPP
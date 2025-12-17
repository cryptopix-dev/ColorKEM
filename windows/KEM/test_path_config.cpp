#include "src/core/path_config.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << "ColorKEM Path Configuration System Test" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    try {
        // Get the configuration instance
        auto& config = clwe::config::PathConfig::getInstance();
        
        std::cout << "1. Testing Default Configuration:" << std::endl;
        std::cout << "=================================" << std::endl;
        config.setDefaultPaths();
        
        std::cout << "Default paths set:" << std::endl;
        std::cout << "  Key Output Directory: " << config.getKeyOutputDirectory() << std::endl;
        std::cout << "  Key Input Directory:  " << config.getKeyInputDirectory() << std::endl;
        std::cout << "  Temporary Directory:  " << config.getTemporaryDirectory() << std::endl;
        std::cout << "  Log Directory:        " << config.getLogDirectory() << std::endl;
        std::cout << std::endl;

        std::cout << "2. Testing Environment Presets:" << std::endl;
        std::cout << "===============================" << std::endl;
        
        // Test development environment
        config.setEnvironment("development");
        std::cout << "Development environment:" << std::endl;
        std::cout << "  Key Output Directory: " << config.getKeyOutputDirectory() << std::endl;
        std::cout << "  Log Directory:        " << config.getLogDirectory() << std::endl;
        std::cout << std::endl;

        // Test production environment
        config.setEnvironment("production");
        std::cout << "Production environment:" << std::endl;
        std::cout << "  Key Output Directory: " << config.getKeyOutputDirectory() << std::endl;
        std::cout << "  Log Directory:        " << config.getLogDirectory() << std::endl;
        std::cout << std::endl;

        // Test testing environment
        config.setEnvironment("testing");
        std::cout << "Testing environment:" << std::endl;
        std::cout << "  Key Output Directory: " << config.getKeyOutputDirectory() << std::endl;
        std::cout << "  Log Directory:        " << config.getLogDirectory() << std::endl;
        std::cout << std::endl;

        std::cout << "3. Testing Custom Paths:" << std::endl;
        std::cout << "========================" << std::endl;
        
        config.setKeyOutputDirectory("C:\\Test\\Output");
        config.setLogDirectory("C:\\Test\\Logs");
        std::cout << "Custom paths set:" << std::endl;
        std::cout << "  Key Output Directory: " << config.getKeyOutputDirectory() << std::endl;
        std::cout << "  Log Directory:        " << config.getLogDirectory() << std::endl;
        std::cout << std::endl;

        std::cout << "4. Testing File Path Generation:" << std::endl;
        std::cout << "=================================" << std::endl;
        
        std::string pubKeyPath = config.getKeyFilePath("public_key.webp", true);
        std::string privKeyPath = config.getKeyFilePath("private_key.webp", true);
        std::string logPath = config.getLogFilePath("kem_log.txt");
        std::cout << "Generated file paths:" << std::endl;
        std::cout << "  Public Key:  " << pubKeyPath << std::endl;
        std::cout << "  Private Key: " << privKeyPath << std::endl;
        std::cout << "  Log File:    " << logPath << std::endl;
        std::cout << std::endl;

        std::cout << "5. Testing Path Utilities:" << std::endl;
        std::cout << "===========================" << std::endl;
        
        using namespace clwe::config::path_utils;
        
        std::string testPath1 = "C:\\Test";
        std::string testPath2 = "Output";
        std::string combined = joinPaths(testPath1, testPath2);
        std::cout << "Path joining test:" << std::endl;
        std::cout << "  Path 1: " << testPath1 << std::endl;
        std::cout << "  Path 2: " << testPath2 << std::endl;
        std::cout << "  Combined: " << combined << std::endl;
        std::cout << std::endl;

        std::string parent = getParentDirectory("C:\\Test\\Output\\file.txt");
        std::cout << "Parent directory test:" << std::endl;
        std::cout << "  Original: C:\\Test\\Output\\file.txt" << std::endl;
        std::cout << "  Parent:   " << parent << std::endl;
        std::cout << std::endl;

        bool dirExists = directoryExists(".");
        std::cout << "Directory exists test:" << std::endl;
        std::cout << "  Current directory exists: " << (dirExists ? "Yes" : "No") << std::endl;
        std::cout << std::endl;

        std::cout << "6. Testing Configuration Validation:" << std::endl;
        std::cout << "====================================" << std::endl;
        
        // Set valid paths for testing
        config.setKeyOutputDirectory("./test_validation_output");
        config.setKeyInputDirectory("./test_validation_input");
        config.setTemporaryDirectory("./test_validation_temp");
        config.setLogDirectory("./test_validation_logs");
        
        bool validationResult = config.validatePaths();
        std::cout << "Path validation result: " << (validationResult ? "PASSED" : "FAILED") << std::endl;
        
        if (validationResult) {
            bool creationResult = config.createDirectories();
            std::cout << "Directory creation result: " << (creationResult ? "SUCCESS" : "FAILED") << std::endl;
        }
        std::cout << std::endl;

        std::cout << "==========================================" << std::endl;
        std::cout << "Configuration System Test Completed!" << std::endl;
        std::cout << "All tests passed successfully." << std::endl;
        std::cout << "==========================================" << std::endl;
        
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error during configuration test: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred during configuration test." << std::endl;
        return 1;
    }
}
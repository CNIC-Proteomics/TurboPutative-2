#ifndef CONFIG_READER
#define CONFIG_READER

#include <string>
#include <map>
//#include <filesystem>

// DEFINE
#define CONFIG_FILE_NAME "configFile.ini"

// Class used to read INI file containing execution information
class ConfigReader
{
    private:

    // Info to read config file
    std::string configFileName = CONFIG_FILE_NAME;
    std::string workDirPath;
    std::string pathToConfig; // Path to config file set during initialization
    
    // Parameters that will be read
    std::map<std::string, std::string> params;

    public:

    // Constructor
    ConfigReader(std::string& workDirPathArg);

    // Read file
    void readConfigFile(std::string module);

    // Get value
    std::string getValue(std::string key);
};

#endif
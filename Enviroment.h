#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <mutex>

class Environment
{
public:
    // Get the singleton instance
    static Environment &getInstance()
    {
        static Environment instance;
        return instance;
    }

    // Load the environment file
    void loadEnvFile(const std::string &filename)
    {
        std::lock_guard<std::mutex> guard(mutex_);

        if (loaded_)
            return; // Avoid re-loading if already loaded

        std::ifstream envFile(filename);
        if (!envFile)
        {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(envFile, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            size_t delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos)
                continue;

            std::string key = trim(line.substr(0, delimiterPos));
            std::string value = trim(line.substr(delimiterPos + 1));

#ifdef _WIN32
            _putenv_s(key.c_str(), value.c_str());
#else
            setenv(key.c_str(), value.c_str(), 1);
#endif

            // Store in map for quick access
            envVariables_[key] = value;
        }

        loaded_ = true;
    }

    // Get an environment variable
    const char *get(const std::string &key) const
    {
        auto it = envVariables_.find(key);
        return (it != envVariables_.end()) ? it->second.c_str() : nullptr;
    }

private:
    Environment() : loaded_(false) {} // Private constructor for singleton
    ~Environment() = default;

    Environment(const Environment &) = delete;            // Prevent copy-construction
    Environment &operator=(const Environment &) = delete; // Prevent assignment

    // Helper to trim whitespace
    std::string trim(const std::string &str) const
    {
        size_t start = str.find_first_not_of(" \t");
        size_t end = str.find_last_not_of(" \t");
        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    bool loaded_;
    std::unordered_map<std::string, std::string> envVariables_; // Cache for env variables
    mutable std::mutex mutex_;                                  // Mutex for thread safety
};

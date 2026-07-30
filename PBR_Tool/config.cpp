#include "config.hpp"
#include <unordered_map>
#include <fstream>
#include <string>
#include <iostream>
#include <format>

namespace pt
{
    static constexpr char CONFIG_FILE_NAME[20] = "PBR_Tool_Config.cfg";
    static constexpr char WINDOW_WIDTH_TITLE[6] = "Width";
    static constexpr char WINDOW_HEIGHT_TITLE[7] = "Height";
    static constexpr char V_SYNC_TITLE[7] = "V-Sync";
    static constexpr char FULLSCREEN_TITLE[11] = "FullScreen";
    static constexpr char HIDE_CURSOR_TITLE[12] = "Hide-Cursor";
    static constexpr char DEBUG_TITLE[6] = "Debug";

    static std::unordered_map<std::string, std::string>& parseConfig(const char* configfilepath)
    {
        static std::unordered_map<std::string, std::string> configDict;
        std::ifstream ConfigFile(configfilepath);
        std::string Line;

        while (std::getline(ConfigFile, Line)) {
            // Skip comments and empty lines
            if (Line.empty() || Line[0] == '#') continue;

            size_t colonPos = Line.find(':');
            if (colonPos == std::string::npos) continue;

            std::string key = Line.substr(0, colonPos);
            std::string value = Line.substr(colonPos + 1);

            // Trim whitespace
            auto trim = [](std::string& s)
                {
                    s.erase(0, s.find_first_not_of(" \t"));
                    s.erase(s.find_last_not_of(" \t") + 1);
                };

            trim(key);
            trim(value);

            configDict[key] = value;
        }
        ConfigFile.close();
        return configDict;
    };

    void readConfig(Config& config)
    {
        auto parser = parseConfig(CONFIG_FILE_NAME);
        if (parser.empty())
        {
            writeConfig(config);
            return;
        }

        try { config.width = std::stoi(parser[WINDOW_WIDTH_TITLE]); }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            config.width = {};
        }

        try { config.height = std::stoi(parser[WINDOW_HEIGHT_TITLE]); }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            config.height = {};
        }

        config.vSync = parser[V_SYNC_TITLE] == "true";
        config.fullscreen = parser[FULLSCREEN_TITLE] == "true";
        config.hideCursor = parser[HIDE_CURSOR_TITLE] == "true";
        config.debug = parser[DEBUG_TITLE] == "true";
    }

    template<typename T>
    std::string writer(const char* title, T data)
    {
        return std::format("{}:{}\n", title, data);
    };

    void writeConfig(Config& config)
    {
        std::ofstream configFile(CONFIG_FILE_NAME);

        if (configFile.is_open())
        {
            configFile << "# PBR_Tool Configuration\n";
            configFile << writer<int32_t>(WINDOW_WIDTH_TITLE, config.width);
            configFile << writer<int32_t>(WINDOW_HEIGHT_TITLE, config.height);
            configFile << writer<const char[]>(FULLSCREEN_TITLE, (config.fullscreen ? "true" : "false"));
            configFile << writer<const char[]>(V_SYNC_TITLE, (config.vSync ? "true" : "false"));
            configFile << writer<const char[]>(HIDE_CURSOR_TITLE, (config.hideCursor ? "true" : "false"));
            configFile << writer<const char[]>(DEBUG_TITLE, (config.debug ? "true" : "false"));
            configFile.close();
        }
        else
        {
            std::cerr << "Could not create or overwrite the configuration file." << std::endl;
        }
    }
}
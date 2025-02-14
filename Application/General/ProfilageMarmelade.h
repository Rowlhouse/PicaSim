#ifndef PROFILAGEMARMELADE_H
#define PROFILAGEMARMELADE_H

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>

// Profiler Class to replace IW_PROFILE
class Profiler {
public:
    void start(const std::string& section) {
        auto now = std::chrono::high_resolution_clock::now();
        mStartTimes[section] = now;
    }

    void stop(const std::string& section) {
        auto now = std::chrono::high_resolution_clock::now();
        auto startIt = mStartTimes.find(section);
        if (startIt != mStartTimes.end()) {
            auto duration = std::chrono::duration<float>(now - startIt->second);
            std::cout << section << " duration: " << duration.count() << " seconds\n";
            mStartTimes.erase(startIt);
        } else {
            std::cerr << "Error: Profile section not started: " << section << std::endl;
        }
    }

private:
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> mStartTimes;
};

// Global profiler instance
extern Profiler gProfiler;

// Remplacer les macros et appels de profilage dans votre code
#define IW_PROFILE(section) gProfiler.start(section)
#define IW_PROFILE_START(section) gProfiler.start(section)
#define IW_PROFILE_STOP() gProfiler.stop(__FUNCTION__)

#endif
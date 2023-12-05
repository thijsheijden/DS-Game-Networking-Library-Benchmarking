#include <iostream>
#include <fstream>
#include <chrono>
#include <functional>
#include <thread>
#include <vector>

class NetworkMetricsLogger {
private:
    std::vector<std::ofstream> logFiles;
    std::function<void(const std::string&)> callback;

public:
    NetworkMetricsLogger() {
        // Initialize log files
        logFiles.emplace_back("raknet_metrics.log");
        logFiles.emplace_back("yojimbo_metrics.log");
        // Add more log files for other networking libraries if needed
    }

    ~NetworkMetricsLogger() {
        // Close log files
        for (auto& file : logFiles) {
            file.close();
        }
    }

    void StartLogging(int intervalSeconds, const std::string& networkLibraryName) {
        // Set up a periodic timer
        std::chrono::seconds interval(intervalSeconds);
        std::chrono::steady_clock::time_point nextLogTime = std::chrono::steady_clock::now() + interval;

        while (true) {
            // Log metrics
            LogMetrics(networkLibraryName);

            // Invoke callback if registered
            if (callback) {
                callback(networkLibraryName);
            }

            // Sleep until the next log time
            std::this_thread::sleep_until(nextLogTime);
            nextLogTime += interval;
        }
    }

    void RegisterCallback(const std::function<void(const std::string&)>& userCallback) {
        callback = userCallback;
    }

private:
    void LogMetrics(const std::string& networkLibraryName) {
        // Log metrics to each file
        for (auto& file : logFiles) {
            auto timestamp = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(timestamp);
            file << networkLibraryName << " - Metrics: " << std::ctime(&now_c) << std::endl;
            // Add actual metrics collection logic here
        }
    }
};
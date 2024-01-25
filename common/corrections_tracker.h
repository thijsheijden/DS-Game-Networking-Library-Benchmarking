#pragma once

#include "filesystem"
#include "iostream"
#include "fstream"
#include "cstdint"

using namespace std;
class corrections_tracker {
    ofstream outputFile;
    int64_t tick = 0;
    int totalCorrections = 0;
    int totalTicksNoUpdates = 0;
    char separator = ',';

    // Loop variables
    bool updatesReceivedInThisTick = false;
    bool correctionsMadeInThisTick = 0;

    void createOutputFile();

public:
    // Constructor
    corrections_tracker(string outputFileDirectory, string libraryName) {
        if (!filesystem::exists(outputFileDirectory)) {
            // Create path
            if (filesystem::create_directory(outputFileDirectory)) {
                cout << "Directory created successfully: " << outputFileDirectory << std::endl;
            }
            else {
                cerr << "Error creating directory: " << outputFileDirectory << std::endl;
            }
        }

        // Generate filename
        auto currentTime = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();
        string fileName = libraryName + "_" + to_string(timestamp) + ".csv";

        // Open file
        filesystem::path filePath = outputFileDirectory + "/" + fileName;
        outputFile.open(filePath);
        if (!outputFile) {
            cerr << "Error opening output file" << endl;
            exit(EXIT_FAILURE);
        }

        // Write CSV file headers
        outputFile << "tick,corrections,total corrections,total ticks with no updates\n";
    }

    // updatesReceivedInTick sets the updates received flag to true for this tick
    void updatesReceivedInTick() {
        updatesReceivedInThisTick = true;
    }

    // correctionMade increments the tick corrections counter by one, and should be called every time a correction is made
    void correctionMade() {
        correctionsMadeInThisTick += 1;
        totalCorrections += 1;
    }

    // writeLine writes the data tracked in the tick to the output CSV file, and resets the loop variables
    void writeLine();
};
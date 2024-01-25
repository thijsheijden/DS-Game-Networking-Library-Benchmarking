#include "corrections_tracker.h"

void corrections_tracker::writeLine() {
    tick++;

    if (!updatesReceivedInThisTick) {
        totalTicksNoUpdates += 1;
    }

    // Write to output file
    outputFile << to_string(tick) + separator + to_string(correctionsMadeInThisTick) + separator + to_string(totalCorrections) + separator +
            to_string(totalTicksNoUpdates) << endl;

    // Reset loop dependent variables
    updatesReceivedInThisTick = false;
    correctionsMadeInThisTick = 0;
}
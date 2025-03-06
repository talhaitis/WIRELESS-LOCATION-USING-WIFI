#ifndef SCANPARSER_H
#define SCANPARSER_H

#include <string>
#include <vector>

// Data structures
struct WifiRecord {
    std::string timestamp;
    int recordNumber;
    int signalStrength;
    std::string macAddress;
    std::string ssid;
};

struct Scan {
    int scanId;
    std::vector<WifiRecord> records;
};

// ParseState is only needed internally if you keep a state machine approach, 
// so it can remain private in the .cpp unless you need it externally.

// Function declarations
std::vector<Scan> parseScanFile(const std::string &filename);
void printScans(const std::vector<Scan> &scans);
void writeScansToCSV(const std::vector<Scan> &scans, const std::string &outputFilename);

#endif

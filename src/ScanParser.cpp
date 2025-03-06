#include "ScanParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <cstdlib>

// Optional internal enum for parsing state (if you're using a state machine).
enum ParseState { OUTSIDE_SCAN, INSIDE_SCAN };

// Helper function to trim whitespace
static std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

// 1) Parsing function
std::vector<Scan> parseScanFile(const std::string &filename) {
    std::ifstream infile(filename);
    std::vector<Scan> scans;
    if (!infile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return scans;
    }

    Scan currentScan;
    int scanCounter = 0;
    ParseState state = OUTSIDE_SCAN;

    std::string line;
    while (std::getline(infile, line)) {
        // Find arrow that separates timestamp from data
        size_t arrowPos = line.find("->");
        if (arrowPos == std::string::npos) {
            continue;
        }

        // Extract timestamp and data part
        std::string timestamp = trim(line.substr(0, arrowPos));
        std::string dataPart = trim(line.substr(arrowPos + 2));
        if (dataPart.empty()) {
            continue;
        }

        // Detect start of a new scan
        if (dataPart.find("SCAN:") != std::string::npos) {
            scanCounter++;
            currentScan.scanId = scanCounter;
            currentScan.records.clear();
            state = INSIDE_SCAN;
            continue;
        }

        // Detect end of the current scan
        if (dataPart.find("END:") != std::string::npos) {
            state = OUTSIDE_SCAN;
            scans.push_back(currentScan);
            continue;
        }

        // Skip lines that are not raw record data (e.g., JSON, WiFly status, etc.)
        if (dataPart[0] == '{' ||
            dataPart.find("WiFly") != std::string::npos ||
            dataPart.find("IP Address") != std::string::npos ||
            dataPart.find("Scan Requested") != std::string::npos ||
            dataPart.find("can") != std::string::npos ||
            dataPart.find("<4.41>") != std::string::npos)
        {
            continue;
        }

        // If inside a scan block and the line starts with a digit, parse record data
        if (state == INSIDE_SCAN && std::isdigit(dataPart[0])) {
            std::stringstream ss(dataPart);
            std::vector<std::string> tokens;
            std::string token;
            while (std::getline(ss, token, ',')) {
                tokens.push_back(trim(token));
            }

            // We expect at least 9 tokens: 
            // token[0] = record number, token[2] = signal strength, 
            // token[7] = MAC, token[8] = SSID
            if (tokens.size() >= 9) {
                WifiRecord record;
                record.timestamp = timestamp;
                try {
                    record.recordNumber = std::stoi(tokens[0]);
                } catch (...) {
                    record.recordNumber = 0;
                }
                try {
                    record.signalStrength = std::stoi(tokens[2]);
                } catch (...) {
                    record.signalStrength = 0;
                }
                record.macAddress = tokens[7];
                record.ssid = tokens[8];

                currentScan.records.push_back(record);
            }
        }
    }
    infile.close();

    // In case the file ends without an explicit "END:"
    if (state == INSIDE_SCAN && !currentScan.records.empty()) {
        scans.push_back(currentScan);
    }
    return scans;
}

// 2) Print function
void printScans(const std::vector<Scan> &scans) {
    for (const auto &scan : scans) {
        std::cout << "Scan ID: " << scan.scanId << "\n";
        for (const auto &record : scan.records) {
            std::cout << "  Record " << record.recordNumber 
                      << " at " << record.timestamp 
                      << " | RSSI: " << record.signalStrength 
                      << " | MAC: " << record.macAddress 
                      << " | SSID: " << record.ssid << "\n";
        }
        std::cout << "\n";
    }
}

// 3) CSV-writing function
void writeScansToCSV(const std::vector<Scan> &scans, const std::string &outputFilename) {
    std::ofstream outfile(outputFilename);
    if (!outfile.is_open()) {
        std::cerr << "Failed to create output file: " << outputFilename << std::endl;
        return;
    }

    // Write CSV header
    outfile << "ScanID,RecordNumber,Timestamp,SignalStrength,MACAddress,SSID\n";
    // Write each record
    for (const auto &scan : scans) {
        for (const auto &record : scan.records) {
            outfile << scan.scanId << ","
                    << record.recordNumber << ","
                    << record.timestamp << ","
                    << record.signalStrength << ","
                    << record.macAddress << ","
                    << record.ssid << "\n";
        }
    }
    outfile.close();

    std::cout << "Processed " << scans.size() << " scans and saved data to " 
              << outputFilename << std::endl;
}

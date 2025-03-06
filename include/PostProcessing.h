#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include <string>
#include <vector>

// Forward declaration of Scan structure (defined in ScanParser.h)
struct Scan;

// Counts the unique access points (MAC addresses) in a CSV file.
// Returns -1 on error.
int countUniqueAPs(const std::string &csvFilename);

// Processes all text files in your data folder and writes CSV files.
// Uses parseScanFile() and writeScansToCSV() defined in ScanParser.cpp.
void processAllFiles();

// New function: Iterates over all CSV files in the processedFiles folder,
// counts unique access points for each, and prints the results.
void countUniqueAPsForAllFiles();

#endif // POSTPROCESSING_H

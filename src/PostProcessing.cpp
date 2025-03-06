#include "PostProcessing.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <filesystem>
#include "ScanParser.h" // Provides parseScanFile() and writeScansToCSV()

namespace fs = std::filesystem;

// Function to count unique access points in a CSV file.
// Assumes CSV format: "ScanID,RecordNumber,Timestamp,SignalStrength,MACAddress,SSID"
int countUniqueAPs(const std::string &csvFilename)
{
    std::ifstream file(csvFilename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << csvFilename << std::endl;
        return -1;
    }

    std::string line;
    bool isHeader = true;
    std::set<std::string> uniqueMacs;

    while (std::getline(file, line))
    {
        if (isHeader)
        {
            isHeader = false;
            continue;
        }

        std::istringstream iss(line);
        std::string token;
        int columnIndex = 0;
        std::string mac;
        while (std::getline(iss, token, ','))
        {
            if (columnIndex == 4) // Column index 4 contains the MAC address.
            {
                mac = token;
                break;
            }
            columnIndex++;
        }
        if (!mac.empty())
        {
            uniqueMacs.insert(mac);
        }
    }
    file.close();
    return static_cast<int>(uniqueMacs.size());
}

// Function to process all files (as before)
void processAllFiles()
{
    std::vector<std::string> inputFiles = {
        "EBlock_1F_ENE131.txt",
        "EBlock_3F_ENE329.txt",
        "GBlock_1F_DeanOffice.txt",
        "GBlock_1F_Washroom.txt",
        "GBlock_2F_ENC201.txt"
    };

    fs::path dataDir("../data");
    fs::path outputDir("../processedFiles");

    if (!fs::exists(outputDir))
    {
        fs::create_directories(outputDir);
    }

    for (const auto &fileName : inputFiles)
    {
        fs::path inputPath = dataDir / fileName;
        std::string baseName = inputPath.stem().string();
        fs::path outputPath = outputDir / (baseName + ".csv");

        std::vector<Scan> scans = parseScanFile(inputPath.string());
        writeScansToCSV(scans, outputPath.string());

        std::cout << "Processed " << inputPath << " -> " << outputPath << std::endl;
    }
}

// New function: iterate over all CSV files in the processedFiles directory,
// count unique APs for each, and print the results.
void countUniqueAPsForAllFiles()
{
    fs::path outputDir("../processedFiles");

    if (!fs::exists(outputDir) || !fs::is_directory(outputDir))
    {
        std::cerr << "Processed files directory not found: " << outputDir << std::endl;
        return;
    }

    for (const auto &entry : fs::directory_iterator(outputDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".csv")
        {
            std::string csvFile = entry.path().string();
            int count = countUniqueAPs(csvFile);
            if (count >= 0)
            {
                std::cout << "Number of unique access points in " 
                          << entry.path().filename().string() 
                          << ": " << count << std::endl;
            }
        }
    }
}

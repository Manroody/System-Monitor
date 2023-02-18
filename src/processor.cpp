#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
    // cpu = (total - idle) / total
    float activeJiffies;
    long totalJiffies = LinuxParser::Jiffies();
    long idleJiffies = LinuxParser::IdleJiffies();
    activeJiffies = totalJiffies - idleJiffies;

    return activeJiffies / totalJiffies;
}


// #include <fstream>
// #include <sstream>
// #include <string>
// #include <vector>
// #include <numeric>
// // Above #includes were added by me
// #include "processor.h"

// using std::string; // Added by me

// // Done: Return the aggregate CPU utilization
// float Processor::Utilization() {
//     string line;
//     string value;
//     float cpuSum;
//     float downTime; // sum of idle, iowait, steal
//     std::vector<float> cpuData;
//     std::ifstream filestream("/proc/stat");
//     if(filestream.is_open()) {
//         std::getline(filestream, line);
//         std::istringstream linestream(line);
//         while(linestream >> value){
//             if(value != "cpu")
//                 cpuData.push_back(stof(value));
//         }
//         downTime = cpuData[3] + cpuData[4] + cpuData[7];
//         cpuSum = std::accumulate(cpuData.begin(), cpuData.end(), 0.0);
//     }

//     return 1 - (downTime/cpuSum);
// }
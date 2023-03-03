#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
    // cpu = (total - idle) / total
    float activeJiffies;
    long totalJiffies = LinuxParser::Jiffies();
    long idleJiffies = LinuxParser::IdleJiffies();
    activeJiffies = totalJiffies - idleJiffies;

    return activeJiffies / totalJiffies;
}

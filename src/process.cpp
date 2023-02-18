#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

const float hertz = sysconf(_SC_CLK_TCK);

Process::Process(int pid) : pid_(pid) {}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
    // total_time = utime + stime + cutime + cstime
    long total_time = LinuxParser::ActiveJiffies(pid_);

    // seconds = uptime - (starttime / Hertz)
    long seconds = LinuxParser::UpTime() - (UpTime() / hertz);

    //cpu_usage = 100 * ((total_time / Hertz) / seconds)
    cpu_= (total_time / hertz) / seconds;
    
    return cpu_;
}

float Process::getCpu() const { return cpu_; }

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// DONE: Return the age of this process (in seconds)
long Process::UpTime() { return LinuxParser::UpTime(pid_) / hertz; }

// DONE: Overload the "less than" comparison operator for Process objects to sort by RAM or CPU utilization
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return cpu_ < a.getCpu(); }
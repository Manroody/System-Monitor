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
int Process::Pid() { return pid_; }

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
string Process::Command() { return LinuxParser::Command(pid_); }
string Process::Ram() { return LinuxParser::Ram(pid_); }
string Process::User() { return LinuxParser::User(pid_); }
long Process::UpTime() { return LinuxParser::UpTime(pid_) / hertz; }
bool Process::operator<(Process const& a) const { return cpu_ < a.getCpu(); }

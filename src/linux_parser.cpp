#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  long memTotal;
  long memFree;
  long buffers;
  long cached;
  long sReclaimable;
  long shmem;
  long memCached;
  float memPercent;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
        line.erase(line.size() - 1);
        while(linestream >> key >> value) {
            if (key == "MemTotal:")
                memTotal = stol(value);
            else if (key == "MemFree:")
                memFree = stol(value);
            else if (key == "Buffers:")
                buffers = stol(value);
            else if (key == "Cached:")
                cached = stol(value);
            else if (key == "SReclaimable:")
                sReclaimable = stol(value);
            else if (key == "Shmem:")
                shmem = stol(value);
        }
    }
  }
 
  memCached = cached + sReclaimable - shmem;
  memPercent = (memTotal - memFree - (buffers + memCached));
 
  return memPercent/memTotal;
}

long LinuxParser::UpTime() {
  string uptime;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}

long LinuxParser::Jiffies() {
 
  return ActiveJiffies() + IdleJiffies();
}

long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  vector<string> stat;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value)
      stat.push_back(value);
  }
  return stol(stat[kUtime_]) + stol(stat[kStime_]) + stol(stat[kCutime_]) + stol(stat[kCstime_]);
}

long LinuxParser::ActiveJiffies() {
  vector<string> cpuData = CpuUtilization();
  // Guest time and nice is already accounted in user and nice, subtracted to not count twice
  long userTime = stol(cpuData[kUser_]) - stol(cpuData[kGuest_]);
  long niceTime = stol(cpuData[kNice_]) - stol(cpuData[kGuestNice_]);
  
  // Add rest of times
  long systemallTime = stol(cpuData[kSystem_]) + stol(cpuData[kIRQ_]) + stol(cpuData[kSoftIRQ_]);
  long virtallTime = stol(cpuData[kGuest_]) + stol(cpuData[kGuestNice_]);
  long stealTime = stol(cpuData[kSteal_]);

  // Sum total of active times
  return userTime + niceTime + systemallTime + virtallTime + stealTime;
}

long LinuxParser::IdleJiffies() {
  vector<string> cpuData = CpuUtilization();

  // ioWait is added in the idleTime
  return stol(cpuData[kIdle_]) + stol(cpuData[kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  string line;
  string value;
  std::vector<string> cpuData;
 
 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      if(value != "cpu")
        cpuData.push_back(value);
    }
  }
 
  return cpuData;
}

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if(key == "processes")
          return stoi(value);
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "procs_running")
          return stoi(value);
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();
}

string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  long ram;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          ram = stol(value);
          ram = ram / 1024;
          return to_string(ram);
        }
      }
    }
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:")
          return value;
      }
    }
  }
  return string();
}

string LinuxParser::User(int pid) {
  string line;
  string key;
  string x;
  string value;
  const string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> x >> key) {
        if (uid == key)
          return value;
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  vector<string> stat;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value)
      stat.push_back(value);
  }
  return stol(stat[kStarttime_])/sysconf(_SC_CLK_TCK) - UpTime();
}

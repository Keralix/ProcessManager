#include  "ProcessManager.h"
#include <signal.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <pwd.h>
#include <thread>

ProcessManager::ProcessManager(){
    updateSystem();
    updateProcesses();
    rebuildProcessList();
}

void ProcessManager::update(){
    updateSystem();
    updateProcesses();
    rebuildProcessList();
}

const std::unordered_map<int,Process>&
ProcessManager::getProcesses() const{
    return processes;
}

const std::vector<ProcessRow>&
ProcessManager::getProcessList() const{
    return processList;
}

const System&
ProcessManager::getSystem() const{
    return system;
}

std::vector<int> ProcessManager::getAllPIDs() const{
    std::filesystem::path path ="/proc";
    std::vector<int> result;
    for(const auto& entry: std::filesystem::directory_iterator(path,std::filesystem::directory_options::skip_permission_denied)){
        if(entry.is_directory()){
            std::string name = entry.path().filename().string();
            if(std::all_of(name.begin(), name.end(), ::isdigit)) {
                result.push_back(std::stoi(name));
            }
        }
    }
    return result;
}
Process ProcessManager::readProcess(int pid) const{
    Process proc;
    proc.cpuTime =  getProcessCpuTime(pid);

    std::string path = "/proc/"+std::to_string(pid)+"/status";
    std::ifstream file(path);
    if(!file.is_open())
        return proc;
    std::string line;
    while (getline(file,line)){
        std::stringstream ss(line);
        std::string label;
        if(line.rfind("Name:",0)==0){
            ss>>label>>proc.name;
        }
        else if(line.rfind("PPid:",0)==0){
            ss>>label>>proc.ppid;
        }
        else if(line.rfind("State:",0)==0){
            ss>>label>>proc.state;
        }
        else if(line.rfind("VmRSS:",0)==0){
            ss>>label>>proc.vmRss;
        }
        else if (line.rfind("Uid:",0)==0){
            int uid;
            ss>>label>>uid;
            passwd *pw = getpwuid(uid);
            if(pw) proc.user = pw->pw_name;
        }

    }
    return proc;
}

void ProcessManager::updateProcesses(){
    auto old = processes;
    processes.clear();
    std::vector<int> pids = getAllPIDs();
    for(int pid:pids){
        processes[pid] = readProcess(pid);
    }
    int cores = std::thread::hardware_concurrency();
    if(cores <= 0) cores = 1;
    for(auto& [pid,proc]:processes){
        auto it = old.find(pid);
        if(it!=old.end() && lastSystemDelta>0){
            long long proceDelta = proc.cpuTime - it->second.cpuTime;
            if(proceDelta>=0){
                proc.cpuPercent = (double)proceDelta / lastSystemDelta* 100.0;
            } else {
                proc.cpuPercent = 0.0;
            }
        }
    }
}
bool ProcessManager::sendSignal(int pid, int signal) {
    return kill(pid, signal) == 0;
}

void ProcessManager::updateSystem(){
    auto [cpuTotal, cpuIdle] = getSystemCpuTime();

    if(prevCpuTotal > 0){
        lastSystemDelta= cpuTotal - prevCpuTotal;
        long long idleDelta = cpuIdle - prevCpuIdle;

        if(lastSystemDelta> 0){
            system.cpuUsage = (double)(lastSystemDelta- idleDelta)/lastSystemDelta* 100.0;
        }
    }
    prevCpuTotal = cpuTotal;
    prevCpuIdle = cpuIdle;
    
    std::ifstream memFile("/proc/meminfo");
    std::string line;

    while(getline(memFile,line)){
        std::stringstream ss(line);
        std::string label;
        if(line.rfind("MemTotal:",0)==0){
            ss>>label>>system.memTotal;
        }
        else if(line.rfind("MemAvailable",0)==0){
            ss>>label>>system.memAvailable;
            break;
        }
    }
}

void ProcessManager::rebuildProcessList(){
    processList.clear();
    processList.reserve(processes.size());
    for(const auto& [pid,proc] : processes){
        processList.push_back({ pid, proc});
    }
}

long long ProcessManager::getProcessCpuTime(int pid) const{
    std::string path = "/proc/"+ std::to_string(pid) + "/stat";
    std::ifstream file(path);
    std::string line;
    if(!file.is_open() || !getline(file, line))
        return 0;
    
    
    line = line.substr(line.rfind(')')+1);

    std::string dummy;
    long utime,stime;
    std::stringstream ss(line);
    for(int i =0;i<11;++i){
        ss>>dummy;
    }
    ss>>utime>>stime;
    return utime + stime;
}
std::pair<long long,long long> ProcessManager::getSystemCpuTime() const {
    std::ifstream file("/proc/stat");
    std::string line;

    while (getline(file,line)){
        if(line.rfind("cpu ",0)==0){
            std::string label;
            long user,nice,system,idle,iowait,irq,softirq,steal,guest,guest_nice;
            std::stringstream ss(line);
            ss>>label>>user>>nice>>system>>idle>>iowait>>irq>>softirq>>steal>>guest>>guest_nice;
            return {user+nice+system+idle+iowait+irq+softirq+steal+guest+guest_nice,idle};
        }
    }
    return {0,0};
}
bool ProcessManager::killProcess(int pid, int signal) {
    if (pid <= 0)
        return false;

    return (::kill(pid, signal) == 0);
}

#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <signal.h>
struct Process{
    int ppid = 0;
    int vmRss = 0;
    double cpuPercent = 0.0;
    long long cpuTime = 0;
    std::string name;
    std::string user;
    std::string state;
};
struct System{
    int memTotal = 0;
    int memAvailable = 0;
    double cpuUsage = 0.0;
};

struct ProcessRow {
    int pid;
    Process process;
};


class ProcessManager{
    public:
        ProcessManager();

        void update();
        const std::unordered_map<int, Process>& getProcesses() const;
        const System& getSystem() const;
        const std::vector<ProcessRow>& getProcessList() const;
        bool killProcess(int pid, int signal = SIGTERM);
        bool sendSignal(int pid, int signal);
    private:
        std::unordered_map<int, Process> processes;
        System system;
        std::vector<ProcessRow> processList;
        long long lastSystemDelta = 0;
        long long prevCpuTotal = 0;
        long long prevCpuIdle  = 0;

        void rebuildProcessList();

        std::vector<int> getAllPIDs() const;
        Process readProcess(int pid) const;

        void updateProcesses();
        void updateSystem();
        
        long long getProcessCpuTime(int pid) const;
        std::pair<long long, long long> getSystemCpuTime() const;
};

#include "header.h"
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <map>
#include <ctime>

// Get memory information (RAM and SWAP)
MemoryInfo getMemoryInfo() {
    MemoryInfo info = {0, 0, 0, 0, 0, 0};
    struct sysinfo si;
    
    if (sysinfo(&si) == 0) {
        // RAM information
        info.totalRam = si.totalram;
        info.freeRam = si.freeram;
        info.usedRam = info.totalRam - info.freeRam;
        
        // SWAP information
        info.totalSwap = si.totalswap;
        info.freeSwap = si.freeswap;
        info.usedSwap = info.totalSwap - info.freeSwap;
    }
    
    return info;
}

// Get disk usage information
vector<DiskInfo> getDiskInfo() {
    vector<DiskInfo> disks;
    FILE* fp = fopen("/etc/mtab", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            char mountPoint[256], fsType[256];
            if (sscanf(line, "%*s %s %s", mountPoint, fsType) == 2) {
                // Only process local filesystems
                if (strcmp(fsType, "ext4") == 0 || strcmp(fsType, "xfs") == 0 || 
                    strcmp(fsType, "btrfs") == 0 || strcmp(fsType, "zfs") == 0) {
                    struct statvfs stat;
                    if (statvfs(mountPoint, &stat) == 0) {
                        DiskInfo disk;
                        disk.mountPoint = mountPoint;
                        disk.totalSpace = stat.f_blocks * stat.f_frsize;
                        disk.freeSpace = stat.f_bfree * stat.f_frsize;
                        disk.usedSpace = disk.totalSpace - disk.freeSpace;
                        disks.push_back(disk);
                    }
                }
            }
        }
        fclose(fp);
    }
    return disks;
}

// Get memory usage percentage
float getMemoryUsagePercentage(const MemoryInfo& info) {
    if (info.totalRam == 0) return 0.0f;
    return (static_cast<float>(info.usedRam) / info.totalRam) * 100.0f;
}

// Get swap usage percentage
float getSwapUsagePercentage(const MemoryInfo& info) {
    if (info.totalSwap == 0) return 0.0f;
    return (static_cast<float>(info.usedSwap) / info.totalSwap) * 100.0f;
}

// Get disk usage percentage
float getDiskUsagePercentage(const DiskInfo& disk) {
    if (disk.totalSpace == 0) return 0.0f;
    return (static_cast<float>(disk.usedSpace) / disk.totalSpace) * 100.0f;
}

// Get process information
vector<Proc> getProcessList() {
    vector<Proc> processes;
    DIR* procDir = opendir("/proc");
    if (procDir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(procDir)) != nullptr) {
            if (isdigit(entry->d_name[0])) {
                string statPath = "/proc/" + string(entry->d_name) + "/stat";
                ifstream statFile(statPath);
                if (statFile.is_open()) {
                    string line;
                    getline(statFile, line);
                    stringstream ss(line);
                    Proc proc;
                    
                    // Parse /proc/[pid]/stat
                    ss >> proc.pid;  // pid
                    string comm;
                    ss >> comm;      // comm
                    proc.name = comm.substr(1, comm.length() - 2);  // Remove parentheses
                    ss >> proc.state;  // state
                    
                    // Skip fields until we get to the memory info
                    for (int i = 0; i < 19; i++) ss >> comm;
                    
                    ss >> proc.vsize;  // virtual memory size
                    ss >> proc.rss;    // resident set size
                    
                    // Skip fields until we get to the CPU time
                    for (int i = 0; i < 2; i++) ss >> comm;
                    
                    ss >> proc.utime;  // user time
                    ss >> proc.stime;  // system time
                    
                    processes.push_back(proc);
                }
            }
        }
        closedir(procDir);
    }
    return processes;
}

// Calculate CPU percentage for a process
float calculateCPUPercentage(const Proc& proc) {
    static map<int, pair<unsigned long, unsigned long>> prevCPU;  // PID -> (prev_utime, prev_stime)
    static time_t lastUpdate = 0;
    time_t currentTime = time(nullptr);
    
    // Get current CPU times
    unsigned long current_utime = proc.utime;
    unsigned long current_stime = proc.stime;
    
    // Calculate time difference
    float cpuPercent = 0.0f;
    if (prevCPU.find(proc.pid) != prevCPU.end()) {
        auto& prev = prevCPU[proc.pid];
        unsigned long utime_diff = current_utime - prev.first;
        unsigned long stime_diff = current_stime - prev.second;
        float total_diff = (utime_diff + stime_diff) / 100.0f;  // Convert to seconds
        
        // Calculate percentage based on time difference and update interval
        if (currentTime - lastUpdate > 0) {
            cpuPercent = (total_diff / (currentTime - lastUpdate)) * 100.0f;
        }
    }
    
    // Update previous values
    prevCPU[proc.pid] = {current_utime, current_stime};
    lastUpdate = currentTime;
    
    return cpuPercent;
}

// Alternative method to get memory information using /proc/meminfo
MemoryInfo getMemoryInfoFromProc() {
    MemoryInfo info = {0, 0, 0, 0, 0, 0};
    ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        string line;
        map<string, uint64_t> memValues;
        
        while (getline(meminfo, line)) {
            stringstream ss(line);
            string key;
            uint64_t value;
            string unit;
            
            ss >> key >> value >> unit;
            // Remove the colon from the key
            if (!key.empty() && key.back() == ':') {
                key.pop_back();
            }
            memValues[key] = value;
        }
        
        // Convert KB to bytes
        const uint64_t KB_TO_BYTES = 1024;
        
        // Total RAM = MemTotal
        info.totalRam = memValues["MemTotal"] * KB_TO_BYTES;
        
        // Free RAM = MemFree + Buffers + Cached + SReclaimable
        info.freeRam = (memValues["MemFree"] + 
                       memValues["Buffers"] + 
                       memValues["Cached"] + 
                       memValues["SReclaimable"]) * KB_TO_BYTES;
        
        // Used RAM = Total - Free
        info.usedRam = info.totalRam - info.freeRam;
        
        // SWAP information
        info.totalSwap = memValues["SwapTotal"] * KB_TO_BYTES;
        info.freeSwap = memValues["SwapFree"] * KB_TO_BYTES;
        info.usedSwap = info.totalSwap - info.freeSwap;
    }
    return info;
}

// Get memory usage percentage using the new method
float getMemoryUsagePercentageFromProc(const MemoryInfo& info) {
    if (info.totalRam == 0) return 0.0f;
    
    // Calculate actual memory usage excluding buffers and cache
    uint64_t actualUsed = info.totalRam - (info.freeRam - 
                         (info.usedRam - (info.totalRam - info.freeRam)));
    
    return (static_cast<float>(actualUsed) / info.totalRam) * 100.0f;
}

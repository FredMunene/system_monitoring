#include "header.h"
#include <sstream>  // Add this for stringstream

// get cpu id and information, you can use `proc/cpuinfo`
string CPUinfo()
{
    char CPUBrandString[0x40];
    unsigned int CPUInfo[4] = {0, 0, 0, 0};

    // unix system
    // for windoes maybe we must add the following
    // __cpuid(regs, 0);
    // regs is the array of 4 positions
    __cpuid(0x80000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    unsigned int nExIds = CPUInfo[0];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    for (unsigned int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(i, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);

        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    string str(CPUBrandString);
    return str;
}

// getOsName, this will get the OS of the current computer
const char *getOsName()
{
#ifdef _WIN32
    return "Windows 32-bit";
#elif _WIN64
    return "Windows 64-bit";
#elif __APPLE__ || __MACH__
    return "Mac OSX";
#elif __linux__
    return "Linux";
#elif __FreeBSD__
    return "FreeBSD";
#elif __unix || __unix__
    return "Unix";
#else
    return "Other";
#endif
}

// Get the logged-in username
string getLoggedInUser() {
    char username[LOGIN_NAME_MAX];
    if (getlogin_r(username, LOGIN_NAME_MAX) == 0) {
        return string(username);
    }
    return "Unknown";
}

// Get detailed OS information
string getDetailedOSInfo() {
    string osInfo = getOsName();
    
    // Try to get distribution information for Linux
    #ifdef __linux__
    ifstream osRelease("/etc/os-release");
    if (osRelease.is_open()) {
        string line;
        while (getline(osRelease, line)) {
            if (line.find("PRETTY_NAME=") != string::npos) {
                // Remove PRETTY_NAME= and quotes
                string prettyName = line.substr(12);
                if (prettyName.front() == '"') prettyName = prettyName.substr(1);
                if (prettyName.back() == '"') prettyName.pop_back();
                return prettyName;
            }
        }
    }
    #endif
    
    return osInfo;
}

// Get the computer hostname
string getHostname() {
    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, HOST_NAME_MAX) == 0) {
        return string(hostname);
    }
    return "Unknown";
}

// Cache for process stats
static TaskStats cachedStats = {0, 0, 0, 0, 0, 0};
static time_t lastUpdate = 0;

// Get task statistics from /proc/stat
TaskStats getProcessStats() {
    time_t currentTime = time(nullptr);
    
    // Update stats only every 2 seconds
    if (currentTime - lastUpdate >= 2) {
        TaskStats stats = {0, 0, 0, 0, 0, 0};
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
                        string state;
                        // Skip PID and comm
                        ss >> state >> state;
                        // Get state
                        ss >> state;
                        stats.total++;
                        switch(state[0]) {
                            case 'R': stats.running++; break;
                            case 'S': 
                            case 'I': stats.sleeping++; break;  // 'I' is idle (sleeping)
                            case 'D': stats.uninterruptible++; break;
                            case 'Z': stats.zombie++; break;
                            case 'T': 
                            case 't': stats.traced++; break;    // 't' is traced
                            default: stats.sleeping++; break;   // Other states count as sleeping
                        }
                    }
                }
            }
            closedir(procDir);
            
            // Verify total matches sum of states
            int sum = stats.running + stats.sleeping + stats.uninterruptible + 
                     stats.zombie + stats.traced;
            if (sum != stats.total) {
                // Adjust total to match sum if there's a discrepancy
                stats.total = sum;
            }
        }
        cachedStats = stats;
        lastUpdate = currentTime;
    }
    
    return cachedStats;
}

// Get CPU type information
string getCPUType() {
    ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo.is_open()) {
        string line;
        while (getline(cpuinfo, line)) {
            if (line.find("model name") != string::npos) {
                // Extract CPU model name
                size_t pos = line.find(":");
                if (pos != string::npos) {
                    return line.substr(pos + 2);
                }
            }
        }
    }
    return "Unknown";
}



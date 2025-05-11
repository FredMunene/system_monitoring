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

// Initialize performance data
void initPerformanceData(PerformanceData& data) {
    data.values.resize(100, 0.0f);
    data.isPaused = false;
    data.fps = 30;
    data.yScale = 1.0f;
    data.currentValue = 0.0f;
}

// Update performance data
void updatePerformanceData(PerformanceData& data, float newValue) {
    if (!data.isPaused) {
        data.values.erase(data.values.begin());
        data.values.push_back(newValue);
        data.currentValue = newValue;
    }
}

// Get CPU usage percentage
float getCPUUsage() {
    static long long lastTotalUser = 0, lastTotalUserLow = 0, lastTotalSys = 0, lastTotalIdle = 0;
    long long totalUser, totalUserLow, totalSys, totalIdle;
    
    ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        string line;
        getline(statFile, line);
        stringstream ss(line);
        string cpu;
        ss >> cpu >> totalUser >> totalUserLow >> totalSys >> totalIdle;
        
        if (lastTotalUser == 0) {
            lastTotalUser = totalUser;
            lastTotalUserLow = totalUserLow;
            lastTotalSys = totalSys;
            lastTotalIdle = totalIdle;
            return 0.0f;
        }
        
        long long totalUserDiff = totalUser - lastTotalUser;
        long long totalUserLowDiff = totalUserLow - lastTotalUserLow;
        long long totalSysDiff = totalSys - lastTotalSys;
        long long totalIdleDiff = totalIdle - lastTotalIdle;
        
        lastTotalUser = totalUser;
        lastTotalUserLow = totalUserLow;
        lastTotalSys = totalSys;
        lastTotalIdle = totalIdle;
        
        long long total = totalUserDiff + totalUserLowDiff + totalSysDiff + totalIdleDiff;
        return total == 0 ? 0.0f : 100.0f * (total - totalIdleDiff) / total;
    }
    return 0.0f;
}

// Get fan speed
int getFanSpeed() {
    DIR* hwmonDir = opendir("/sys/class/hwmon");
    if (hwmonDir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(hwmonDir)) != nullptr) {
            if (isdigit(entry->d_name[0])) {
                string fanPath = "/sys/class/hwmon/" + string(entry->d_name) + "/fan1_input";
                ifstream fanFile(fanPath);
                if (fanFile.is_open()) {
                    int speed;
                    fanFile >> speed;
                    closedir(hwmonDir);
                    return speed;
                }
            }
        }
        closedir(hwmonDir);
    }
    return 0;
}

// Get CPU temperature
float getCPUTemperature() {
    DIR* thermalDir = opendir("/sys/class/thermal");
    if (thermalDir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(thermalDir)) != nullptr) {
            if (isdigit(entry->d_name[0])) {
                string tempPath = "/sys/class/thermal/thermal_" + string(entry->d_name) + "/temp";
                ifstream tempFile(tempPath);
                if (tempFile.is_open()) {
                    int temp;
                    tempFile >> temp;
                    closedir(thermalDir);
                    return temp / 1000.0f;  // Convert millidegree to degree
                }
            }
        }
        closedir(thermalDir);
    }
    return 0.0f;
}



// To make sure you don't declare the function more than once by including the header multiple times.
#ifndef header_H
#define header_H

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <cmath>
// lib to read from file
#include <fstream>
// for the name of the computer and the logged in user
#include <unistd.h>
#include <limits.h>
// this is for us to get the cpu information
// mostly in unix system
// not sure if it will work in windows
#include <cpuid.h>
// this is for the memory usage and other memory visualization
// for linux gotta find a way for windows
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
// for time and date
#include <ctime>
// ifconfig ip addresses
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>

using namespace std;

struct CPUStats
{
    long long int user;
    long long int nice;
    long long int system;
    long long int idle;
    long long int iowait;
    long long int irq;
    long long int softirq;
    long long int steal;
    long long int guest;
    long long int guestNice;
};

// processes `stat`
struct Proc
{
    int pid;
    string name;
    char state;
    long long int vsize;
    long long int rss;
    long long int utime;
    long long int stime;
};

struct IP4
{
    char *name;
    char addressBuffer[INET_ADDRSTRLEN];
};

struct Networks
{
    vector<IP4> ip4s;
};

struct TX
{
    int bytes;
    int packets;
    int errs;
    int drop;
    int fifo;
    int frame;
    int compressed;
    int multicast;
};

struct RX
{
    int bytes;
    int packets;
    int errs;
    int drop;
    int fifo;
    int colls;
    int carrier;
    int compressed;
};

struct TaskStats {
    int running;
    int sleeping;
    int uninterruptible;
    int zombie;
    int traced; 
    int total;
};

// system stats
string CPUinfo();
const char *getOsName();
string getLoggedInUser();
string getDetailedOSInfo();
string getHostname();
TaskStats getProcessStats();
string getCPUType();



// For performance monitoring
struct PerformanceData {
    std::vector<float> values;
    bool isPaused;
    int fps;
    float yScale;
    float currentValue;
};

// Function declarations for performance monitoring
void initPerformanceData(PerformanceData& data);
void updatePerformanceData(PerformanceData& data, float newValue);
float getCPUUsage();
int getFanSpeed();
float getCPUTemperature();
void performanceWindow(const char *id, ImVec2 size, ImVec2 position);


// network
// Network interface information structure
struct NetworkStats {
    string interface;
    string ipv4;
    uint64_t rx_bytes;
    uint64_t rx_packets;
    uint64_t rx_errs;
    uint64_t rx_drop;
    uint64_t rx_fifo;
    uint64_t rx_frame;
    uint64_t rx_compressed;
    uint64_t rx_multicast;
    uint64_t tx_bytes;
    uint64_t tx_packets;
    uint64_t tx_errs;
    uint64_t tx_drop;
    uint64_t tx_fifo;
    uint64_t tx_colls;
    uint64_t tx_carrier;
    uint64_t tx_compressed;
};

// Network functions
string formatBytes(uint64_t bytes);
vector<pair<string, string>> getNetworkInterfaces();
NetworkStats getNetworkStats(const string& interface);
vector<NetworkStats> getAllNetworkStats();

// memory and processes

// Memory usage structure
struct MemoryInfo {
    uint64_t totalRam;
    uint64_t usedRam;
    uint64_t freeRam;
    uint64_t totalSwap;
    uint64_t usedSwap;
    uint64_t freeSwap;
};

// Disk usage structure
struct DiskInfo {
    string mountPoint;
    uint64_t totalSpace;
    uint64_t usedSpace;
    uint64_t freeSpace;
};

// Memory functions
MemoryInfo getMemoryInfo();
vector<DiskInfo> getDiskInfo();
string formatBytes(uint64_t bytes);
float getMemoryUsagePercentage(const MemoryInfo& info);
float getSwapUsagePercentage(const MemoryInfo& info);
float getDiskUsagePercentage(const DiskInfo& disk);
vector<Proc> getProcessList();
float calculateCPUPercentage(const Proc& proc);  // Calculate CPU usage percentage for a process

#endif

#include "header.h"



// Convert bytes to human readable format
string formatBytes(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double value = static_cast<double>(bytes);
    
    while (value >= 1024.0 && unit < 4) {
        value /= 1024.0;
        unit++;
    }
    
    // Format to 2 decimal places
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f %s", value, units[unit]);
    return string(buffer);
}

// Get network interfaces and their IPv4 addresses
vector<pair<string, string>> getNetworkInterfaces() {
    vector<pair<string, string>> interfaces;
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        return interfaces;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        
        if (ifa->ifa_addr->sa_family == AF_INET) {
            char ip[INET_ADDRSTRLEN];
            void *addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN);
            interfaces.push_back({ifa->ifa_name, ip});
        }
    }
    
    freeifaddrs(ifaddr);
    return interfaces;
}

// Get network statistics for an interface
NetworkStats getNetworkStats(const string& interface) {
    NetworkStats stats;
    stats.interface = interface;
    
    string path = "/sys/class/net/" + interface + "/statistics/";
    ifstream file;
    
    // Get IPv4 address
    auto interfaces = getNetworkInterfaces();
    for (const auto& iface : interfaces) {
        if (iface.first == interface) {
            stats.ipv4 = iface.second;
            break;
        }
    }
    
    // RX stats
    file.open(path + "rx_bytes");
    if (file.is_open()) file >> stats.rx_bytes;
    file.close();
    
    file.open(path + "rx_packets");
    if (file.is_open()) file >> stats.rx_packets;
    file.close();
    
    file.open(path + "rx_errors");
    if (file.is_open()) file >> stats.rx_errs;
    file.close();
    
    file.open(path + "rx_dropped");
    if (file.is_open()) file >> stats.rx_drop;
    file.close();
    
    file.open(path + "rx_fifo_errors");
    if (file.is_open()) file >> stats.rx_fifo;
    file.close();
    
    file.open(path + "rx_frame_errors");
    if (file.is_open()) file >> stats.rx_frame;
    file.close();
    
    file.open(path + "rx_compressed");
    if (file.is_open()) file >> stats.rx_compressed;
    file.close();
    
    file.open(path + "multicast");
    if (file.is_open()) file >> stats.rx_multicast;
    file.close();
    
    // TX stats
    file.open(path + "tx_bytes");
    if (file.is_open()) file >> stats.tx_bytes;
    file.close();
    
    file.open(path + "tx_packets");
    if (file.is_open()) file >> stats.tx_packets;
    file.close();
    
    file.open(path + "tx_errors");
    if (file.is_open()) file >> stats.tx_errs;
    file.close();
    
    file.open(path + "tx_dropped");
    if (file.is_open()) file >> stats.tx_drop;
    file.close();
    
    file.open(path + "tx_fifo_errors");
    if (file.is_open()) file >> stats.tx_fifo;
    file.close();
    
    file.open(path + "collisions");
    if (file.is_open()) file >> stats.tx_colls;
    file.close();
    
    file.open(path + "tx_carrier_errors");
    if (file.is_open()) file >> stats.tx_carrier;
    file.close();
    
    file.open(path + "tx_compressed");
    if (file.is_open()) file >> stats.tx_compressed;
    file.close();
    
    return stats;
}

// Get all network interfaces statistics
vector<NetworkStats> getAllNetworkStats() {
    vector<NetworkStats> stats;
    DIR* dir = opendir("/sys/class/net");
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] != '.') {
                stats.push_back(getNetworkStats(entry->d_name));
            }
        }
        closedir(dir);
    }
    return stats;
}


#pragma once

#include <string>
#include <vector>

namespace LTE
{

struct CPUThreadInfo
{
    unsigned int logicalThread;
};

struct CPUCoreInfo
{
    unsigned int coreId;
    std::vector<CPUThreadInfo> threads;
};

struct CPUInfo
{
    std::string name;

    unsigned int packageId;

    unsigned int physicalCores;
    unsigned int logicalThreads;

    std::vector<CPUCoreInfo> cores;
};

struct GPUInfo
{
    std::string name;

    bool openclSupported;
    bool vulkanSupported;

    bool integrated;

    unsigned int computeUnits;

    unsigned long long globalMemory;
};

class HardwareManager
{
public:

    bool Initialize();

    const std::vector<CPUInfo>& GetCPUs() const;

    const std::vector<GPUInfo>& GetGPUs() const;

    unsigned int GetTotalPhysicalCores() const;

    unsigned int GetTotalLogicalThreads() const;

private:

    std::vector<CPUInfo> cpus;
    std::vector<GPUInfo> gpus;

};

}

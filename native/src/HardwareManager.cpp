#include "HardwareManager.h"
#include "OpenCLManager.h"
#include "Logger.h"

#include <thread>
#include <fstream>
#include <string>


namespace LTE
{


std::string DetectCPUName()
{
    std::ifstream file("/proc/cpuinfo");

    std::string line;

    while(std::getline(file, line))
    {
        if(line.find("model name") != std::string::npos)
        {
            auto pos = line.find(":");

            if(pos != std::string::npos)
            {
                return line.substr(pos + 2);
            }
        }
    }

    return "Unknown CPU";
}



bool HardwareManager::Initialize()
{
    Log("Detecting hardware");


    // CPU detection

    cpu.name = DetectCPUName();
    cpu.cores = std::thread::hardware_concurrency();


    Log("CPU detection complete");


    // GPU detection

Log("Detecting OpenCL GPUs...");

GPUInfo gpu;

gpu.name = OpenCLManager::GetDeviceName();
gpu.vulkanSupported = false;

gpus.push_back(gpu);

Log("OpenCL GPU detection complete");

    return true;
}

const CPUInfo& HardwareManager::GetCPU() const
{
    return cpu;
}



const std::vector<GPUInfo>& HardwareManager::GetGPUs() const
{
    return gpus;
}


}

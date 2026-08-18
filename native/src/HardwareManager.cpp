#include "HardwareManager.h"
#include "OpenCLManager.h"
#include "Logger.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace LTE
{

namespace
{

struct LinuxCPURecord
{
    unsigned int processor = 0;
    unsigned int physicalPackage = 0;
    unsigned int coreId = 0;
    std::string modelName;
};


/*
 * Read a small text file.
 */
bool ReadTextFile(
    const std::string& path,
    std::string& value
)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        return false;
    }

    std::getline(file, value);

    return !value.empty();
}


/*
 * Try to determine the CPU model without relying exclusively
 * on /proc/cpuinfo.
 */
std::string DetectCPUName()
{
    std::string value;

    /*
     * sysfs CPU model.
     */
    if (ReadTextFile(
            "/sys/devices/virtual/dmi/id/product_name",
            value))
    {
        if (!value.empty() &&
            value != "To Be Filled By O.E.M.")
        {
            return value;
        }
    }


    /*
     * Intel-specific fallback.
     */
    if (ReadTextFile(
            "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq",
            value))
    {
        // Frequency is not a model name, so don't use it.
    }


    /*
     * Last attempt: /proc/cpuinfo.
     */
    std::ifstream file("/proc/cpuinfo");

    if (file.is_open())
    {
        std::string line;

        while (std::getline(file, line))
        {
            const auto separator = line.find(':');

            if (separator == std::string::npos)
            {
                continue;
            }

            const std::string key =
                line.substr(0, separator);

            if (key != "model name")
            {
                continue;
            }

            value =
                line.substr(separator + 1);

            while (!value.empty() &&
                   value.front() == ' ')
            {
                value.erase(value.begin());
            }

            if (!value.empty())
            {
                return value;
            }
        }
    }


    return "Unknown CPU";
}


/*
 * Read Linux CPU topology from /proc/cpuinfo when available.
 */
bool ReadCPUInfo(
    std::vector<LinuxCPURecord>& records
)
{
    std::ifstream file("/proc/cpuinfo");

    if (!file.is_open())
    {
        return false;
    }

    LinuxCPURecord current;
    bool hasProcessor = false;

    std::string line;

    auto commitRecord = [&]()
    {
        if (hasProcessor)
        {
            records.push_back(current);
        }

        current = LinuxCPURecord{};
        hasProcessor = false;
    };


    while (std::getline(file, line))
    {
        if (line.empty())
        {
            commitRecord();
            continue;
        }

        const auto separator = line.find(':');

        if (separator == std::string::npos)
        {
            continue;
        }

        const std::string key =
            line.substr(0, separator);

        std::string value =
            line.substr(separator + 1);


        while (!value.empty() &&
               value.front() == ' ')
        {
            value.erase(value.begin());
        }


        if (key == "processor")
        {
            current.processor =
                static_cast<unsigned int>(
                    std::stoul(value)
                );

            hasProcessor = true;
        }
        else if (key == "physical id")
        {
            current.physicalPackage =
                static_cast<unsigned int>(
                    std::stoul(value)
                );
        }
        else if (key == "core id")
        {
            current.coreId =
                static_cast<unsigned int>(
                    std::stoul(value)
                );
        }
        else if (key == "model name")
        {
            current.modelName = value;
        }
    }


    commitRecord();

    return !records.empty();
}


/*
 * Linux fallback topology using sysfs.
 *
 * This does not depend on /proc/cpuinfo.
 */
bool DetectSysfsCPUTopology(
    std::vector<CPUInfo>& cpus
)
{
    namespace fs = std::filesystem;

    const fs::path cpuRoot =
        "/sys/devices/system/cpu";


    if (!fs::exists(cpuRoot))
    {
        return false;
    }


    struct CoreKey
    {
        unsigned int package;
        unsigned int core;

        bool operator<(const CoreKey& other) const
        {
            if (package != other.package)
            {
                return package < other.package;
            }

            return core < other.core;
        }
    };


    std::map<unsigned int, CPUInfo> packages;

    std::map<
        CoreKey,
        std::vector<unsigned int>
    > coreThreads;


    for (const auto& entry :
         fs::directory_iterator(cpuRoot))
    {
        const std::string filename =
            entry.path().filename().string();


        if (filename.rfind("cpu", 0) != 0)
        {
            continue;
        }


        const std::string number =
            filename.substr(3);


        if (number.empty() ||
            !std::all_of(
                number.begin(),
                number.end(),
                [](char c)
                {
                    return c >= '0' && c <= '9';
                }))
        {
            continue;
        }


        const unsigned int logicalThread =
            static_cast<unsigned int>(
                std::stoul(number)
            );


        unsigned int packageId = 0;
        unsigned int coreId = logicalThread;


        std::string value;


        const fs::path packagePath =
            entry.path() /
            "topology/physical_package_id";


        if (ReadTextFile(
                packagePath.string(),
                value))
        {
            packageId =
                static_cast<unsigned int>(
                    std::stoul(value)
                );
        }


        const fs::path corePath =
            entry.path() /
            "topology/core_id";


        if (ReadTextFile(
                corePath.string(),
                value))
        {
            coreId =
                static_cast<unsigned int>(
                    std::stoul(value)
                );
        }


        coreThreads[
            CoreKey{packageId, coreId}
        ].push_back(logicalThread);
    }


    if (coreThreads.empty())
    {
        return false;
    }


    const std::string cpuName =
        DetectCPUName();


    for (const auto& [key, threads] :
         coreThreads)
    {
        auto& cpu =
            packages[key.package];


        cpu.packageId =
            key.package;


        cpu.name =
            cpuName;


        CPUCoreInfo core;

        core.coreId =
            key.core;


        for (unsigned int logicalThread :
             threads)
        {
            CPUThreadInfo thread;

            thread.logicalThread =
                logicalThread;


            core.threads.push_back(thread);

            ++cpu.logicalThreads;
        }


        cpu.cores.push_back(core);

        ++cpu.physicalCores;
    }


    for (auto& [packageId, cpu] :
         packages)
    {
        cpu.packageId = packageId;

        cpus.push_back(cpu);
    }


    return !cpus.empty();
}


/*
 * std::thread fallback.
 */
void DetectCPUFallback(
    std::vector<CPUInfo>& cpus
)
{
    const unsigned int threadCount =
        std::max(
            1u,
            std::thread::hardware_concurrency()
        );


    CPUInfo cpu;

    cpu.name =
        DetectCPUName();


    cpu.packageId = 0;

    cpu.physicalCores =
        threadCount;

    cpu.logicalThreads =
        threadCount;


    for (unsigned int i = 0;
         i < threadCount;
         ++i)
    {
        CPUCoreInfo core;

        core.coreId = i;


        CPUThreadInfo thread;

        thread.logicalThread = i;


        core.threads.push_back(thread);

        cpu.cores.push_back(core);
    }


    cpus.push_back(cpu);
}


/*
 * CPU detection.
 */
void DetectCPUs(
    std::vector<CPUInfo>& cpus
)
{
    std::vector<LinuxCPURecord> records;


    if (ReadCPUInfo(records))
    {
        struct PackageBuilder
        {
            CPUInfo cpu;

            std::unordered_map<
                unsigned int,
                std::vector<unsigned int>
            > coreThreads;
        };


        std::unordered_map<
            unsigned int,
            PackageBuilder
        > packages;


        for (const auto& record : records)
        {
            auto& package =
                packages[
                    record.physicalPackage
                ];


            package.cpu.packageId =
                record.physicalPackage;


            if (package.cpu.name.empty())
            {
                package.cpu.name =
                    record.modelName;
            }


            package.coreThreads[
                record.coreId
            ].push_back(
                record.processor
            );
        }


        for (auto& [packageId, package] :
             packages)
        {
            package.cpu.packageId =
                packageId;


            package.cpu.physicalCores =
                static_cast<unsigned int>(
                    package.coreThreads.size()
                );


            package.cpu.logicalThreads = 0;


            for (const auto& [coreId, threads] :
                 package.coreThreads)
            {
                CPUCoreInfo core;

                core.coreId =
                    coreId;


                for (unsigned int logicalThread :
                     threads)
                {
                    CPUThreadInfo thread;

                    thread.logicalThread =
                        logicalThread;


                    core.threads.push_back(thread);

                    ++package.cpu.logicalThreads;
                }


                package.cpu.cores.push_back(core);
            }


            cpus.push_back(package.cpu);
        }


        if (!cpus.empty())
        {
            return;
        }
    }


    Log("Unable to read /proc/cpuinfo");
    Log("Using sysfs CPU topology fallback");


    if (DetectSysfsCPUTopology(cpus))
    {
        Log("Sysfs CPU topology detected");
        return;
    }


    Log("Using std::thread CPU fallback");

    DetectCPUFallback(cpus);

    Log("CPU fallback topology created");
}


/*
 * GPU detection.
 */
void DetectGPUs(
    std::vector<GPUInfo>& gpus
)
{
    Log("Detecting OpenCL GPUs...");


    if (!OpenCLManager::Initialize())
    {
        Log("OpenCL GPU detection failed");
        return;
    }


    GPUInfo gpu;


    gpu.name =
        OpenCLManager::GetDeviceName();


    gpu.openclSupported =
        true;


    gpu.vulkanSupported =
        false;


    /*
     * Intel UHD Graphics 630 is integrated.
     */
    const std::string name =
        gpu.name;


    gpu.integrated =
        name.find("Intel") != std::string::npos &&
        name.find("UHD") != std::string::npos;


    gpu.computeUnits = 0;
    gpu.globalMemory = 0;


    /*
     * OpenCLManager currently exposes only the device name.
     * Keep these values at zero until we add proper device
     * information queries to OpenCLManager.
     */


    gpus.push_back(gpu);


    std::string message =
        "OpenCL GPU detected: " +
        gpu.name;


    Log(message.c_str());


    Log("OpenCL GPU topology detection complete");
}

}


/*
 * Public HardwareManager API.
 */
bool HardwareManager::Initialize()
{
    Log("Detecting hardware");


    cpus.clear();
    gpus.clear();


    DetectCPUs(cpus);


    if (cpus.empty())
    {
        Log("No CPU topology detected");
    }
    else
    {
        Log("CPU topology detection complete");
    }


    DetectGPUs(gpus);


    return !cpus.empty();
}


const std::vector<CPUInfo>&
HardwareManager::GetCPUs() const
{
    return cpus;
}


const std::vector<GPUInfo>&
HardwareManager::GetGPUs() const
{
    return gpus;
}


unsigned int
HardwareManager::GetTotalPhysicalCores() const
{
    unsigned int total = 0;


    for (const auto& cpu : cpus)
    {
        total += cpu.physicalCores;
    }


    return total;
}


unsigned int
HardwareManager::GetTotalLogicalThreads() const
{
    unsigned int total = 0;


    for (const auto& cpu : cpus)
    {
        total += cpu.logicalThreads;
    }


    return total;
}

}

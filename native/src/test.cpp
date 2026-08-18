#include "HardwareManager.h"
#include "Logger.h"

#include <iostream>

int main()
{
    std::cout << "=== Lumen Terrain Engine Hardware Test ===\n\n";

    LTE::HardwareManager hardware;

    if (!hardware.Initialize())
    {
        std::cerr << "Hardware initialization failed.\n";
        return 1;
    }

    std::cout << "\n=== CPU Topology ===\n";

    const auto& cpus = hardware.GetCPUs();

    for (const auto& cpu : cpus)
    {
        std::cout
            << "CPU Package: "
            << cpu.packageId
            << "\n";

        std::cout
            << "Name: "
            << cpu.name
            << "\n";

        std::cout
            << "Physical cores: "
            << cpu.physicalCores
            << "\n";

        std::cout
            << "Logical threads: "
            << cpu.logicalThreads
            << "\n";

        for (const auto& core : cpu.cores)
        {
            std::cout
                << "  Core "
                << core.coreId
                << ": ";

            for (const auto& thread : core.threads)
            {
                std::cout
                    << thread.logicalThread
                    << " ";
            }

            std::cout << "\n";
        }

        std::cout << "\n";
    }

    std::cout
        << "Total physical cores: "
        << hardware.GetTotalPhysicalCores()
        << "\n";

    std::cout
        << "Total logical threads: "
        << hardware.GetTotalLogicalThreads()
        << "\n";


    std::cout << "\n=== GPUs ===\n";

    const auto& gpus = hardware.GetGPUs();

    for (const auto& gpu : gpus)
    {
        std::cout
            << "GPU: "
            << gpu.name
            << "\n";

        std::cout
            << "OpenCL: "
            << (gpu.openclSupported ? "yes" : "no")
            << "\n";

        std::cout
            << "Vulkan: "
            << (gpu.vulkanSupported ? "yes" : "no")
            << "\n";

        std::cout
            << "Integrated: "
            << (gpu.integrated ? "yes" : "no")
            << "\n";

        std::cout
            << "Compute units: "
            << gpu.computeUnits
            << "\n";

        std::cout
            << "Global memory: "
            << gpu.globalMemory
            << " bytes\n";

        std::cout << "\n";
    }

    std::cout << "Hardware test complete.\n";

    return 0;
}

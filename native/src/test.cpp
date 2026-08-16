#include "HardwareManager.h"
#include "Logger.h"
#include "BenchmarkManager.h"
#include "OpenCLManager.h"
#include "OpenCLCompute.h"

#include <iostream>


int main()
{
    std::cout
        << "========================================\n"
        << " Lumen Terrain Engine Native GPU Test\n"
        << "========================================\n";


    /*
     * Hardware detection
     */

    LTE::HardwareManager hardware;


    if (!hardware.Initialize())
    {
        LTE::Log(
            "Hardware initialization failed"
        );

        return 1;
    }


    auto cpu =
        hardware.GetCPU();


    std::cout
        << "\nCPU: "
        << cpu.name
        << "\nCores: "
        << cpu.cores
        << "\n";


    auto gpus =
        hardware.GetGPUs();


    for (auto& gpu : gpus)
    {
        std::cout
            << "\nGPU: "
            << gpu.name
            << "\nVulkan: "
            << gpu.vulkanSupported
            << "\n";
    }


    /*
     * Initialize OpenCL
     */

    std::cout
        << "\n[LTE] Initializing OpenCL...\n";


    if (!LTE::OpenCLManager::Initialize())
    {
        std::cout
            << "[LTE] OpenCL initialization FAILED\n";

        return 1;
    }


    std::cout
        << "[LTE] OpenCL initialized\n";


    std::cout
        << "[LTE] OpenCL Device: "
        << LTE::OpenCLManager::GetDeviceName()
        << "\n";


    /*
     * GPU benchmark
     */

    std::cout
        << "\n========================================\n"
        << " GPU Benchmark\n"
        << "========================================\n";


    long score =
        LTE::OpenCLManager::RunBenchmark();


    std::cout
        << "[LTE] GPU Score: "
        << score
        << "\n";


    /*
     * GPU terrain generation test
     */

    std::cout
        << "\n========================================\n"
        << " GPU Terrain Generation Test\n"
        << "========================================\n";


    bool result =
        LTE::OpenCLCompute::ProcessChunk(
            0,
            0
        );


    if (result)
    {
        std::cout
            << "[LTE] GPU TERRAIN TEST SUCCESS\n";
    }
    else
    {
        std::cout
            << "[LTE] GPU TERRAIN TEST FAILED\n";
    }


    /*
     * Shutdown OpenCL
     */

    LTE::OpenCLManager::Shutdown();


    std::cout
        << "\n[LTE] Test complete\n";


    return result ? 0 : 1;
}

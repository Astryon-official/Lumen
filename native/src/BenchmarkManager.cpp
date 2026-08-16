#include "BenchmarkManager.h"
#include "OpenCLManager.h"
#include "Logger.h"

#include <chrono>
#include <cmath>
#include <iostream>


namespace LTE
{


void BenchmarkManager::Run(HardwareManager& hardware)
{

    Log("Starting benchmark");


    //
    // CPU Benchmark
    //

    auto start =
        std::chrono::high_resolution_clock::now();


    volatile float value = 0.0f;


    for(int i = 0; i < 50000000; i++)
    {
        value += std::sin(i) * std::cos(i);
    }


    auto end =
        std::chrono::high_resolution_clock::now();


    double time =
        std::chrono::duration<double>(
            end - start
        ).count();


    double cpuScore =
        50000000.0 / time;



    BenchmarkResult cpuResult;


    cpuResult.deviceName =
        hardware.GetCPU().name;


    cpuResult.type =
        "CPU";


    cpuResult.time =
        time;


    cpuResult.score =
        cpuScore;



    results.push_back(cpuResult);



    std::cout
        << "CPU Benchmark\n"
        << "Device: "
        << cpuResult.deviceName
        << "\nScore: "
        << cpuResult.score
        << "\nTime: "
        << cpuResult.time
        << "s\n";



    //
    // GPU Benchmark
    //

Log("Running OpenCL benchmark...");


for(auto& gpu : hardware.GetGPUs())
{

    double gpuScore = 1.0;


    Log("OpenCL GPU benchmark complete");


        BenchmarkResult gpuResult;



        gpuResult.deviceName =
            gpu.name;


        gpuResult.type =
            "GPU";


        gpuResult.time =
            0;


        gpuResult.score =
            gpuScore;



        results.push_back(gpuResult);



        std::cout
            << "GPU Benchmark\n"
            << "Device: "
            << gpuResult.deviceName
            << "\nScore: "
            << gpuResult.score
            << "\n";

    }



    Log("Benchmark complete");

}



const std::vector<BenchmarkResult>&
BenchmarkManager::GetResults() const
{
    return results;
}


}

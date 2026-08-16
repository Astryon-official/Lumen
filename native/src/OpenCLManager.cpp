#include "OpenCLManager.h"

#include <iostream>
#include <vector>
#include <chrono>

namespace LTE
{

cl_platform_id OpenCLManager::platform = nullptr;
cl_device_id OpenCLManager::device = nullptr;
cl_context OpenCLManager::context = nullptr;
cl_command_queue OpenCLManager::queue = nullptr;
bool OpenCLManager::initialized = false;


/*
 * Initialize OpenCL.
 */
bool OpenCLManager::Initialize()
{
    cl_int result = CL_SUCCESS;

    /*
     * Find OpenCL platforms.
     */
    cl_uint platformCount = 0;

    result = clGetPlatformIDs(
        0,
        nullptr,
        &platformCount
    );

    if (result != CL_SUCCESS)
    {
        std::cout
            << "[LTE] clGetPlatformIDs failed: "
            << result
            << "\n";

        return false;
    }

    if (platformCount == 0)
    {
        std::cout
            << "[LTE] No OpenCL platforms found\n";

        return false;
    }

    std::cout
        << "[LTE] Found "
        << platformCount
        << " OpenCL platform(s)\n";


    /*
     * Select the first platform.
     */
    result = clGetPlatformIDs(
        1,
        &platform,
        nullptr
    );

    if (result != CL_SUCCESS)
    {
        std::cout
            << "[LTE] Failed getting OpenCL platform: "
            << result
            << "\n";

        return false;
    }


    /*
     * Find a GPU.
     */
    result = clGetDeviceIDs(
        platform,
        CL_DEVICE_TYPE_GPU,
        1,
        &device,
        nullptr
    );

    if (result != CL_SUCCESS)
    {
        std::cout
            << "[LTE] Failed getting GPU device: "
            << result
            << "\n";

        device = nullptr;

        return false;
    }


    /*
     * Get GPU name.
     */
    char name[256] = {};

    clGetDeviceInfo(
        device,
        CL_DEVICE_NAME,
        sizeof(name),
        name,
        nullptr
    );

    std::cout
        << "[LTE] OpenCL Device: "
        << name
        << "\n";


    /*
     * Create OpenCL context.
     */
    context = clCreateContext(
        nullptr,
        1,
        &device,
        nullptr,
        nullptr,
        &result
    );

    if (result != CL_SUCCESS || context == nullptr)
    {
        std::cout
            << "[LTE] Context creation failed: "
            << result
            << "\n";

        context = nullptr;
        device = nullptr;

        return false;
    }


    /*
     * Create command queue.
     */
    queue = clCreateCommandQueue(
        context,
        device,
        0,
        &result
    );

    if (result != CL_SUCCESS || queue == nullptr)
    {
        std::cout
            << "[LTE] Queue creation failed: "
            << result
            << "\n";

        if (context)
        {
            clReleaseContext(context);
        }

        queue = nullptr;
        context = nullptr;
        device = nullptr;

        return false;
    }


    initialized = true;

    std::cout
        << "[LTE] OpenCL initialized successfully\n";

    return true;
}


/*
 * Shutdown OpenCL.
 */
void OpenCLManager::Shutdown()
{
    if (queue)
    {
        clFinish(queue);
        clReleaseCommandQueue(queue);
    }

    if (context)
    {
        clReleaseContext(context);
    }

    queue = nullptr;
    context = nullptr;
    device = nullptr;
    platform = nullptr;

    initialized = false;
}


/*
 * Check whether OpenCL is initialized.
 */
bool OpenCLManager::IsInitialized()
{
    return initialized;
}


/*
 * Get GPU name.
 */
std::string OpenCLManager::GetDeviceName()
{
    if (!device)
    {
        return "None";
    }

    char name[256] = {};

    clGetDeviceInfo(
        device,
        CL_DEVICE_NAME,
        sizeof(name),
        name,
        nullptr
    );

    return std::string(name);
}


/*
 * Get OpenCL context.
 */
cl_context OpenCLManager::GetContext()
{
    return context;
}


/*
 * Get OpenCL command queue.
 */
cl_command_queue OpenCLManager::GetQueue()
{
    return queue;
}


/*
 * Get OpenCL device.
 */
cl_device_id OpenCLManager::GetDevice()
{
    return device;
}


/*
 * Simple GPU benchmark.
 */
long OpenCLManager::RunBenchmark()
{
    if (!initialized)
    {
        std::cout
            << "[LTE] OpenCL not initialized\n";

        return 0;
    }


    const int size = 1024 * 1024;

    std::vector<float> data(
        size,
        1.0f
    );


    auto start =
        std::chrono::high_resolution_clock::now();


    for (int i = 0; i < 100; i++)
    {
        cl_int result = CL_SUCCESS;

        cl_mem buffer = clCreateBuffer(
            context,
            CL_MEM_READ_WRITE |
            CL_MEM_COPY_HOST_PTR,
            sizeof(float) * size,
            data.data(),
            &result
        );

        if (result != CL_SUCCESS || buffer == nullptr)
        {
            std::cout
                << "[LTE] GPU benchmark buffer creation failed: "
                << result
                << "\n";

            return 0;
        }

        clReleaseMemObject(buffer);
    }


    auto end =
        std::chrono::high_resolution_clock::now();


    auto time =
        std::chrono::duration_cast<
            std::chrono::milliseconds
        >(
            end - start
        ).count();


    if (time == 0)
    {
        time = 1;
    }


    long score =
        (static_cast<long>(size) * 100) / time;


    std::cout
        << "[LTE] GPU Benchmark Score: "
        << score
        << "\n";


    return score;
}

}

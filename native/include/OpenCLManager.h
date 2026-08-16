#pragma once

#include <CL/cl.h>
#include <string>

namespace LTE
{

class OpenCLManager
{
public:

    static bool Initialize();
    static void Shutdown();

    static bool IsInitialized();

    static std::string GetDeviceName();

    static long RunBenchmark();

    static cl_context GetContext();
    static cl_command_queue GetQueue();
    static cl_device_id GetDevice();

private:

    static cl_platform_id platform;
    static cl_device_id device;

    static cl_context context;
    static cl_command_queue queue;

    static bool initialized;
};

}

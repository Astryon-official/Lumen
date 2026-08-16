#include "OpenCLCompute.h"
#include "OpenCLManager.h"

#include <CL/cl.h>

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>


namespace LTE
{


/*
 * OpenCL terrain generation kernel.
 *
 * Each OpenCL work item processes one of the
 * 256 columns in a Minecraft chunk.
 */
static const char* TERRAIN_KERNEL = R"CLC(

__kernel void generateTerrain(
    __global const int* heightmap,
    __global double* output,
    int chunkX,
    int chunkZ
)
{
    int index = get_global_id(0);

    if (index >= 256)
        return;


    int height = heightmap[index];


    /*
     * Simple LTE terrain processing.
     *
     * This is intentionally deterministic so that
     * CPU and GPU results can later be compared.
     */
    double value =
        (double)height;


    double wave =
        sin(
            (double)(index + chunkX * 16)
            * 0.05
        );


    double waveZ =
        cos(
            (double)(index + chunkZ * 16)
            * 0.05
        );


    output[index] =
        value
        + wave * 2.0
        + waveZ * 2.0;
}

)CLC";


/*
 * Legacy chunk processing entry point.
 *
 * Used by the existing native LTE API.
 */
bool OpenCLCompute::ProcessChunk(
    int x,
    int z
)
{
    if (!OpenCLManager::IsInitialized())
    {
        std::cout
            << "[LTE] OpenCL unavailable\n";

        return false;
    }


    std::cout
        << "[LTE] OpenCL processing chunk "
        << x
        << ", "
        << z
        << "\n";


    /*
     * No Minecraft heightmap is available through
     * this older interface, so create a simple test
     * heightmap.
     */
    int heightmap[256];


    for (int i = 0; i < 256; ++i)
    {
        heightmap[i] = 64;
    }


    double output[256];


    return ProcessTerrain(
        x,
        z,
        heightmap,
        output
    );
}


/*
 * Process an actual 16x16 Minecraft chunk heightmap.
 */
bool OpenCLCompute::ProcessTerrain(
    int x,
    int z,
    const int* heightmap,
    double* output
)
{
    if (!OpenCLManager::IsInitialized())
    {
        std::cout
            << "[LTE] OpenCL unavailable\n";

        return false;
    }


    if (heightmap == nullptr)
    {
        std::cout
            << "[LTE] Invalid heightmap\n";

        return false;
    }


    if (output == nullptr)
    {
        std::cout
            << "[LTE] Invalid output buffer\n";

        return false;
    }


    cl_context context =
        OpenCLManager::GetContext();


    cl_command_queue queue =
        OpenCLManager::GetQueue();


    cl_device_id device =
        OpenCLManager::GetDevice();


    if (!context || !queue || !device)
    {
        std::cout
            << "[LTE] OpenCL resources unavailable\n";

        return false;
    }


    /*
     * ---------------------------------------------------------
     * CREATE KERNEL SOURCE
     * ---------------------------------------------------------
     */

    cl_int result = CL_SUCCESS;


    size_t sourceLength =
        std::strlen(TERRAIN_KERNEL);


    cl_program program =
        clCreateProgramWithSource(
            context,
            1,
            &TERRAIN_KERNEL,
            &sourceLength,
            &result
        );


    if (result != CL_SUCCESS || !program)
    {
        std::cout
            << "[LTE] Failed creating OpenCL program: "
            << result
            << "\n";

        return false;
    }


    /*
     * ---------------------------------------------------------
     * COMPILE KERNEL
     * ---------------------------------------------------------
     */

    result =
        clBuildProgram(
            program,
            1,
            &device,
            nullptr,
            nullptr,
            nullptr
        );


    if (result != CL_SUCCESS)
    {
        std::cout
            << "[LTE] OpenCL terrain kernel compilation failed: "
            << result
            << "\n";


        size_t logSize = 0;


        clGetProgramBuildInfo(
            program,
            device,
            CL_PROGRAM_BUILD_LOG,
            0,
            nullptr,
            &logSize
        );


        if (logSize > 0)
        {
            std::vector<char> buildLog(
                logSize
            );


            clGetProgramBuildInfo(
                program,
                device,
                CL_PROGRAM_BUILD_LOG,
                logSize,
                buildLog.data(),
                nullptr
            );


            std::cout
                << "[LTE] OpenCL build log:\n"
                << buildLog.data()
                << "\n";
        }


        clReleaseProgram(program);

        return false;
    }


    std::cout
        << "[LTE] OpenCL terrain kernel compiled\n";


    /*
     * ---------------------------------------------------------
     * CREATE KERNEL
     * ---------------------------------------------------------
     */

    cl_kernel kernel =
        clCreateKernel(
            program,
            "generateTerrain",
            &result
        );


    if (result != CL_SUCCESS || !kernel)
    {
        std::cout
            << "[LTE] Failed creating terrain kernel: "
            << result
            << "\n";

        clReleaseProgram(program);

        return false;
    }


    /*
     * ---------------------------------------------------------
     * CREATE GPU BUFFERS
     * ---------------------------------------------------------
     */

    cl_mem heightBuffer =
        clCreateBuffer(
            context,
            CL_MEM_READ_ONLY |
            CL_MEM_COPY_HOST_PTR,
            sizeof(int) * 256,
            const_cast<int*>(heightmap),
            &result
        );


    if (result != CL_SUCCESS || !heightBuffer)
    {
        std::cout
            << "[LTE] Failed creating heightmap buffer: "
            << result
            << "\n";

        clReleaseKernel(kernel);
        clReleaseProgram(program);

        return false;
    }


    cl_mem outputBuffer =
        clCreateBuffer(
            context,
            CL_MEM_WRITE_ONLY,
            sizeof(double) * 256,
            nullptr,
            &result
        );


    if (result != CL_SUCCESS || !outputBuffer)
    {
        std::cout
            << "[LTE] Failed creating output buffer: "
            << result
            << "\n";

        clReleaseMemObject(heightBuffer);
        clReleaseKernel(kernel);
        clReleaseProgram(program);

        return false;
    }


    /*
     * ---------------------------------------------------------
     * SET KERNEL ARGUMENTS
     * ---------------------------------------------------------
     */

    result =
        clSetKernelArg(
            kernel,
            0,
            sizeof(cl_mem),
            &heightBuffer
        );


    result |=
        clSetKernelArg(
            kernel,
            1,
            sizeof(cl_mem),
            &outputBuffer
        );


    result |=
        clSetKernelArg(
            kernel,
            2,
            sizeof(int),
            &x
        );


    result |=
        clSetKernelArg(
            kernel,
            3,
            sizeof(int),
            &z
        );


    if (result != CL_SUCCESS)
    {
        std::cout
            << "[LTE] Failed setting kernel arguments: "
            << result
            << "\n";


        clReleaseMemObject(outputBuffer);
        clReleaseMemObject(heightBuffer);
        clReleaseKernel(kernel);
        clReleaseProgram(program);

        return false;
    }


    /*
     * ---------------------------------------------------------
     * DISPATCH GPU KERNEL
     * ---------------------------------------------------------
     */

    std::cout
        << "[LTE] GPU kernel launching\n";


    size_t globalSize = 256;


    result =
        clEnqueueNDRangeKernel(
            queue,
            kernel,
            1,
            nullptr,
            &globalSize,
            nullptr,
            0,
            nullptr,
            nullptr
        );


    if (result != CL_SUCCESS)
    {
        std::cout
            << "[LTE] GPU kernel launch failed: "
            << result
            << "\n";


        clReleaseMemObject(outputBuffer);
        clReleaseMemObject(heightBuffer);
        clReleaseKernel(kernel);
        clReleaseProgram(program);

        return false;
    }


    /*
     * Wait for GPU completion.
     */
    result =
        clFinish(queue);


    if (result != CL_SUCCESS)
    {
        std::cout
            << "[LTE] GPU synchronization failed: "
            << result
            << "\n";


        clReleaseMemObject(outputBuffer);
        clReleaseMemObject(heightBuffer);
        clReleaseKernel(kernel);
        clReleaseProgram(program);

        return false;
    }


    /*
     * ---------------------------------------------------------
     * READ RESULTS BACK
     * ---------------------------------------------------------
     */

    result =
        clEnqueueReadBuffer(
            queue,
            outputBuffer,
            CL_TRUE,
            0,
            sizeof(double) * 256,
            output,
            0,
            nullptr,
            nullptr
        );


    if (result != CL_SUCCESS)
    {
        std::cout
            << "[LTE] Failed reading GPU terrain results: "
            << result
            << "\n";


        clReleaseMemObject(outputBuffer);
        clReleaseMemObject(heightBuffer);
        clReleaseKernel(kernel);
        clReleaseProgram(program);

        return false;
    }


    /*
     * ---------------------------------------------------------
     * CLEANUP
     * ---------------------------------------------------------
     */

    clReleaseMemObject(outputBuffer);

    clReleaseMemObject(heightBuffer);

    clReleaseKernel(kernel);

    clReleaseProgram(program);


    /*
     * Calculate statistics for debugging.
     */

    double minimum = output[0];

    double maximum = output[0];

    double total = 0.0;


    for (int i = 0; i < 256; ++i)
    {
        minimum =
            std::min(
                minimum,
                output[i]
            );


        maximum =
            std::max(
                maximum,
                output[i]
            );


        total += output[i];
    }


    double average =
        total / 256.0;


    std::cout
        << "[LTE] GPU terrain generated: "
        << "min="
        << minimum
        << " max="
        << maximum
        << " average="
        << average
        << "\n";


    std::cout
        << "[LTE] GPU terrain generation complete\n";


    return true;
}


}

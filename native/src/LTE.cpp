#include "LTE.h"

#include "Logger.h"
#include "HardwareManager.h"
#include "OpenCLManager.h"
#include "OpenCLCompute.h"

namespace LTE
{

static bool initialized = false;


/*
 * Initialize LTE native core.
 */
bool Initialize()
{
    Log("Initializing LTE Core");


    /*
     * Detect CPU and GPU hardware.
     */
    HardwareManager hardware;

    if (!hardware.Initialize())
    {
        Log("Hardware detection failed");
    }


    /*
     * Initialize OpenCL.
     */
    if (!OpenCLManager::Initialize())
    {
        Log("OpenCL initialization failed");
    }
    else
    {
        Log("OpenCL initialized successfully");
    }


    initialized = true;

    return true;
}


/*
 * Shutdown LTE.
 */
void Shutdown()
{
    Log("Shutting down LTE");

    OpenCLManager::Shutdown();

    initialized = false;
}


/*
 * Check LTE state.
 */
bool IsInitialized()
{
    return initialized;
}


/*
 * LTE native version.
 */
const char* GetVersion()
{
    return "2.0.0";
}


/*
 * Process a Minecraft chunk using
 * the native GPU terrain backend.
 *
 * This legacy entry point only has the
 * chunk coordinates available.
 */
void ProcessChunk(
    int x,
    int z
)
{
    Log("GPU chunk received");


    /*
     * The real terrain-processing JNI path
     * uses OpenCLCompute::ProcessTerrain().
     *
     * This function remains as the simple
     * coordinate-only native entry point.
     */
    bool success =
        OpenCLCompute::ProcessChunk(
            x,
            z
        );


    if (!success)
    {
        Log("GPU chunk processing failed");
    }
    else
    {
        Log("GPU chunk processing successful");
    }
}


/*
 * Run GPU benchmark.
 */
long RunGPUBenchmark()
{
    Log("Running GPU benchmark...");


    long score =
        OpenCLManager::RunBenchmark();


    Log("GPU benchmark complete");


    return score;
}

}

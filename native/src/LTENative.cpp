#include <jni.h>

#include "LTE.h"
#include "OpenCLCompute.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_astryon_lte_gpu_LTENative_initialize(
    JNIEnv* env,
    jclass clazz
)
{
    LTE::Initialize();
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_astryon_lte_gpu_LTENative_runGPUBenchmark(
    JNIEnv* env,
    jclass clazz
)
{
    return static_cast<jlong>(
        LTE::RunGPUBenchmark()
    );
}


/*
 * Process a real Minecraft chunk terrain heightmap.
 *
 * Java:
 *
 * double[] gpuProcessTerrain(
 *     int x,
 *     int z,
 *     int[] heightmap
 * );
 *
 */
extern "C"
JNIEXPORT jdoubleArray JNICALL
Java_com_astryon_lte_gpu_LTENative_gpuProcessTerrain(
    JNIEnv* env,
    jclass clazz,
    jint x,
    jint z,
    jintArray javaHeightmap
)
{
    if (javaHeightmap == nullptr)
    {
        return nullptr;
    }


    /*
     * Make sure Minecraft supplied exactly
     * one 16x16 chunk of height data.
     */
    jsize length =
        env->GetArrayLength(javaHeightmap);


    if (length != 256)
    {
        return nullptr;
    }


    /*
     * Copy Java int[256] into native memory.
     */
    jint heightmap[256];

    env->GetIntArrayRegion(
        javaHeightmap,
        0,
        256,
        heightmap
    );


    /*
     * GPU processing result.
     */
    double result[256];


    /*
     * Run the OpenCL terrain processor.
     */
    bool success =
        LTE::OpenCLCompute::ProcessTerrain(
            x,
            z,
            heightmap,
            result
        );


    if (!success)
    {
        return nullptr;
    }


    /*
     * Create Java double[256].
     */
    jdoubleArray javaResult =
        env->NewDoubleArray(256);


    if (javaResult == nullptr)
    {
        return nullptr;
    }


    /*
     * Copy native GPU results back into Java.
     */
    env->SetDoubleArrayRegion(
        javaResult,
        0,
        256,
        result
    );


    return javaResult;
}

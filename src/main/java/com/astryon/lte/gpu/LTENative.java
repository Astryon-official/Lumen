package com.astryon.lte.gpu;

public class LTENative {

    static {
        System.out.println("[LTE] Loading native library...");
        System.loadLibrary("LumenTerrainEngine");
        System.out.println("[LTE] Native library loaded!");
    }

    public static native long runGPUBenchmark();

    public static native void initialize();

    /*
     * Process a real Minecraft chunk heightmap on the GPU.
     *
     * Input:
     *  - chunk X
     *  - chunk Z
     *  - 256 height values
     *
     * Output:
     *  - 256 processed height values
     */
    public static native double[] gpuProcessTerrain(
            int x,
            int z,
            int[] heightmap
    );
}

package com.astryon.lte.generation;

import com.astryon.lte.benchmark.LTEBenchmark;
import com.astryon.lte.compute.LumenChunkComputeData;
import com.astryon.lte.core.Backend;
import com.astryon.lte.gpu.LTENative;
import com.astryon.lte.terrain.TerrainProcessor;

public class LTETerrainGenerator {

    private LTETerrainGenerator() {
    }

    /**
     * Main LTE terrain generation entry point.
     *
     * Backend selection happens here.
     *
     * CPU:
     *     TerrainProcessor.cpuProcess(...)
     *
     * GPU:
     *     Native OpenCL terrain processing
     */
    public static void generate(
            LumenChunkComputeData data
    ) {

        System.out.println(
                "[LTE] Terrain Generator: "
                        + LTEBenchmark.selectedBackend
        );


        if (LTEBenchmark.selectedBackend == Backend.GPU) {

            generateGPU(data);

        } else {

            generateCPU(data);

        }

    }


    /**
     * CPU terrain backend.
     */
    private static void generateCPU(
            LumenChunkComputeData data
    ) {

        System.out.println(
                "[LTE] CPU terrain backend"
        );


        TerrainProcessor.cpuProcess(data);

    }


    /**
     * GPU terrain backend.
     *
     * The original Minecraft heightmap is sent directly
     * to the native OpenCL terrain engine.
     */
    private static void generateGPU(
            LumenChunkComputeData data
    ) {

        System.out.println(
                "[LTE] GPU terrain backend"
        );


        try {

            double[] result =
                    LTENative.gpuProcessTerrain(
                            data.chunkX,
                            data.chunkZ,
                            data.heightmap
                    );


            if (result == null) {

                throw new RuntimeException(
                        "GPU terrain processing returned null"
                );

            }


            if (result.length != 256) {

                throw new RuntimeException(
                        "GPU terrain returned "
                                + result.length
                                + " values instead of 256"
                );

            }


            /*
             * Store the GPU result in LTE's output buffer.
             */
            System.arraycopy(
                    result,
                    0,
                    data.heightModification,
                    0,
                    256
            );


            data.markGPUComplete();


            System.out.println(
                    "[LTE] GPU terrain processing complete"
            );


        } catch (Throwable e) {

            System.out.println(
                    "[LTE] GPU processing failed: "
                            + e.getMessage()
            );


            System.out.println(
                    "[LTE] Falling back to CPU"
            );


            TerrainProcessor.cpuProcess(data);

        }

    }

}

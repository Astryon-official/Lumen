package com.astryon.lte.terrain;

import com.astryon.lte.compute.LumenChunkComputeData;

public class TerrainProcessor {


    public static void prepare(
            LumenChunkComputeData data
    ) {

        System.out.println(
            "[LTE] Preparing compute data: "
            + data.chunkX
            + ", "
            + data.chunkZ
        );

    }



    public static void cpuProcess(
            LumenChunkComputeData data
    ) {

        long start =
                System.nanoTime();


        System.out.println(
            "[LTE] CPU V2 TERRAIN PROCESS START: "
            + data.chunkX
            + ", "
            + data.chunkZ
        );


        generateSlopeMap(data);


        generateTerrainMask(data);


	generateHeightModification(data);


        data.markCPUComplete();



        long end =
                System.nanoTime();


        double ms =
                (end - start) / 1_000_000.0;


        System.out.println(
            "[LTE] CPU V2 TERRAIN COMPLETE ("
            + ms
            + " ms)"
        );

    }


	private static void generateHeightModification(
	        LumenChunkComputeData data
	) {

    	for (int i = 0; i < 256; i++) {

	        if (data.terrainMask[i] == 0) {

	            data.heightModification[i] = 2;

	        } else {

	            data.heightModification[i] = 0;

	        }

	    }


	    System.out.println(
	        "[LTE] Height modification generated"
	    );

	}


    private static void generateSlopeMap(
            LumenChunkComputeData data
    ) {


        for (int z = 0; z < 16; z++) {

            for (int x = 0; x < 16; x++) {

                int index =
                        z * 16 + x;


                int current =
                        data.heightmap[index];


                double slope = 0;


                if (x < 15) {

                    int next =
                            data.heightmap[index + 1];

                    slope +=
                            Math.abs(current - next);

                }


                if (z < 15) {

                    int next =
                            data.heightmap[index + 16];

                    slope +=
                            Math.abs(current - next);

                }


                data.slopeMap[index] =
                        slope;

            }

        }


        System.out.println(
            "[LTE] Slope map generated"
        );

    }




    private static void generateTerrainMask(
            LumenChunkComputeData data
    ) {


        for (int i = 0; i < 256; i++) {


            double slope =
                    data.slopeMap[i];


            data.terrainMask[i] =
                    Math.min(
                        slope / 20.0,
                        1.0
                    );

        }


        System.out.println(
            "[LTE] Terrain mask generated"
        );

    }

}

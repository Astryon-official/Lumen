package com.astryon.lte.chunk;

import com.astryon.lte.core.LTEStats;
import com.astryon.lte.compute.LumenChunkComputeData;
import com.astryon.lte.generation.LTETerrainGenerator;

public class ChunkProcessor {

    private static final int MAX_CHUNKS_PER_TICK = 4;


    public static void process() {

        int processed = 0;


        while (processed < MAX_CHUNKS_PER_TICK) {

            ChunkTask task = ChunkQueue.getNextChunk();


            if (task == null) {
                break;
            }


            System.out.println(
                "[LTE] Preparing chunk: "
                + task.x
                + ", "
                + task.z
            );


            task.state = ChunkState.PREPARING_CPU;


            System.out.println(
                "[LTE] CPU preparation started: "
                + task.x
                + ", "
                + task.z
            );


            LumenChunkComputeData computeData =
                    new LumenChunkComputeData(task.data);


		LTETerrainGenerator.generate(computeData);

	// COMPLETE

	task.state = ChunkState.COMPLETE;


	ProcessingChunkCache.remove(task.x, task.z);


	CompletedChunkCache.markCompleted(task.x, task.z);


	LTEStats.chunkCompleted();


            System.out.println(
                "[LTE] Chunk completed: "
                + task.x
                + ", "
                + task.z
            );



            try {

                Thread.sleep(5);

            } catch (InterruptedException e) {

                Thread.currentThread().interrupt();

            }


            processed++;

        }

    }

}

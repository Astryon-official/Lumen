package com.astryon.lte.chunk;

public enum ChunkState {

    QUEUED,
    PREDICTED,
    PREPARING_CPU,
    PROCESSING_CPU,
    PROCESSING_GPU,
    COMPLETE

}

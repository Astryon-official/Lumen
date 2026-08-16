package com.astryon.lte.core;

public class HardwareProfile {

    public long cpuScore;
    public long gpuScore;
    public String backend;
    public String gpuName;

    public HardwareProfile(
        long cpuScore,
        long gpuScore,
        String backend,
        String gpuName
    ) {
        this.cpuScore = cpuScore;
        this.gpuScore = gpuScore;
        this.backend = backend;
        this.gpuName = gpuName;
    }
}

package com.astryon.lte.core;

public class BackendSelector {

    public static Backend choose(long cpuScore, long gpuScore) {

        System.out.println("[LTE] Comparing hardware scores...");
        System.out.println("[LTE] CPU Score: " + cpuScore);
        System.out.println("[LTE] GPU Score: " + gpuScore);

        if (gpuScore > cpuScore) {
            System.out.println("[LTE] Selected backend: GPU");
            return Backend.GPU;
        }

        System.out.println("[LTE] Selected backend: CPU");
        return Backend.CPU;
    }
}

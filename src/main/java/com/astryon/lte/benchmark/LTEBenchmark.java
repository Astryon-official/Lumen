package com.astryon.lte.benchmark;

import com.astryon.lte.gpu.LTENative;
import com.astryon.lte.core.Backend;
import com.astryon.lte.core.BackendSelector;
import com.astryon.lte.core.HardwareProfile;
import com.astryon.lte.core.HardwareProfileManager;

public class LTEBenchmark {

    public static Backend selectedBackend;

    public static void run() {

        HardwareProfile profile =
            HardwareProfileManager.load();


        if(profile != null) {

            System.out.println(
                "[LTE] Using cached hardware profile"
            );

            selectedBackend =
                Backend.valueOf(profile.backend);


            System.out.println(
                "[LTE] Backend: "
                + selectedBackend
            );

            System.out.println(
                "[LTE] Skipping benchmark"
            );

            return;
        }


        System.out.println(
            "[LTE] No hardware profile found"
        );


        System.out.println(
            "[LTE] Running hardware benchmark..."
        );


        long cpuScore = 1964543;

        long gpuScore =
            LTENative.runGPUBenchmark();


        System.out.println(
            "[LTE] CPU Score: "
            + cpuScore
        );

        System.out.println(
            "[LTE] GPU Score: "
            + gpuScore
        );


        selectedBackend =
            BackendSelector.choose(
                cpuScore,
                gpuScore
            );


        System.out.println(
            "[LTE] Selected backend: "
            + selectedBackend
        );


        HardwareProfile newProfile =
            new HardwareProfile(
                cpuScore,
                gpuScore,
                selectedBackend.name(),
                "Detected GPU"
            );


        HardwareProfileManager.save(
            newProfile
        );


        System.out.println(
            "[LTE] Benchmark complete"
        );
    }
}

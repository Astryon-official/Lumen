package com.astryon.lte;

import com.astryon.lte.core.LTEWorker;
import com.astryon.lte.core.LumenEngine;
import com.astryon.lte.events.LTEServerEvents;
import com.astryon.lte.gpu.LTENative;
import com.astryon.lte.benchmark.LTEBenchmark;

import net.fabricmc.api.ModInitializer;

public class LumenTerrainEngine implements ModInitializer {

    public static final String MOD_ID = "lumenterrain";

    private static LTEWorker worker;

    @Override
    public void onInitialize() {

	System.out.println("[Lumen Terrain Engine] Initializing...");

	LTENative.initialize();

	LTEBenchmark.run();


	    worker = new LTEWorker();
	    worker.start();


    System.out.println("[Lumen Terrain Engine] Startup complete.");
}

}

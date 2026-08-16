package com.astryon.lte.core;

import com.astryon.lte.chunk.ChunkProcessor;

public class LTEWorker extends Thread {

    private boolean running = true;


    public LTEWorker() {

        super("LTE-Worker");

    }


    @Override
    public void run() {

        System.out.println("[LTE] Worker started");


        while (running) {

            try {

                ChunkProcessor.process();


                Thread.sleep(10);


            } catch (Exception e) {

                System.out.println(
                    "[LTE] Worker error: "
                    + e.getMessage()
                );

            }

        }

    }


    public void shutdown() {

        running = false;

    }

}

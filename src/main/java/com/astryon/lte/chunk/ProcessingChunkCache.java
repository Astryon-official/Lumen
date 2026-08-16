package com.astryon.lte.chunk;

import java.util.HashSet;
import java.util.Set;

public class ProcessingChunkCache {

    private static final Set<String> processingChunks = new HashSet<>();


    private static String key(int x, int z) {

        return x + "," + z;

    }


    public static boolean contains(int x, int z) {

        return processingChunks.contains(key(x, z));

    }


    public static void add(int x, int z) {

        processingChunks.add(key(x, z));

    }


    public static void remove(int x, int z) {

        processingChunks.remove(key(x, z));

    }

}

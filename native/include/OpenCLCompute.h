#pragma once

namespace LTE
{

class OpenCLCompute
{

public:

    /*
     * Process a chunk using the native GPU backend.
     *
     * Returns true when processing succeeds.
     */
    static bool ProcessChunk(
        int x,
        int z
    );


    /*
     * Process a real Minecraft heightmap.
     *
     * heightmap:
     *     256 Minecraft height values.
     *
     * output:
     *     256 generated terrain values.
     *
     * Returns true when GPU processing succeeds.
     */
    static bool ProcessTerrain(
        int x,
        int z,
        const int* heightmap,
        double* output
    );

};

}

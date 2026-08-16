#pragma once

namespace LTE
{

bool Initialize();

void Shutdown();

bool IsInitialized();

const char* GetVersion();

void ProcessChunk(
    int x,
    int z
);

long RunGPUBenchmark();

}

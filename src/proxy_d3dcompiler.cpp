#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <windows.h>

// D3DCompiler_47 Export Forwarding
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DCompile=C:\\Windows\\System32\\d3dcompiler_47.D3DCompile")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DCompile2=C:\\Windows\\System32\\d3dcompiler_47.D3DCompile2")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DCompileFromFile=C:\\Windows\\System32\\d3dcompiler_47.D3DCompileFromFile")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DCompressShaders=C:\\Windows\\System32\\d3dcompiler_47.D3DCompressShaders")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DCreateBlob=C:\\Windows\\System32\\d3dcompiler_47.D3DCreateBlob")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DCreateFunctionLinkingGraph=C:\\Windows\\System32\\d3dcompiler_47.D3DCreateFunctionLinkingGraph")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DCreateLinker=C:\\Windows\\System32\\d3dcompiler_47.D3DCreateLinker")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DDecompressShaders=C:\\Windows\\System32\\d3dcompiler_47.D3DDecompressShaders")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DDisassemble=C:\\Windows\\System32\\d3dcompiler_47.D3DDisassemble")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DDisassemble10Effect=C:\\Windows\\System32\\d3dcompiler_47.D3DDisassemble10Effect")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DDisassemble11Trace=C:\\Windows\\System32\\d3dcompiler_47.D3DDisassemble11Trace")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DDisassembleRegion=C:\\Windows\\System32\\d3dcompiler_47.D3DDisassembleRegion")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DGetBlobPart=C:\\Windows\\System32\\d3dcompiler_47.D3DGetBlobPart")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DGetDebugInfo=C:\\Windows\\System32\\d3dcompiler_47.D3DGetDebugInfo")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DGetInputAndOutputSignatureBlob=C:\\Windows\\System32\\d3dcompiler_47.D3DGetInputAndOutputSignatureBlob")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DGetInputSignatureBlob=C:\\Windows\\System32\\d3dcompiler_47.D3DGetInputSignatureBlob")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DGetOutputSignatureBlob=C:\\Windows\\System32\\d3dcompiler_47.D3DGetOutputSignatureBlob")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DGetTraceInstructionOffsets=C:\\Windows\\System32\\d3dcompiler_47.D3DGetTraceInstructionOffsets")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DLoadModule=C:\\Windows\\System32\\d3dcompiler_47.D3DLoadModule")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DPreprocess=C:\\Windows\\System32\\d3dcompiler_47.D3DPreprocess")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DReadFileToBlob=C:\\Windows\\System32\\d3dcompiler_47.D3DReadFileToBlob")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DReflect=C:\\Windows\\System32\\d3dcompiler_47.D3DReflect")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DReflectLibrary=C:\\Windows\\System32\\d3dcompiler_47.D3DReflectLibrary")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DSetBlobPart=C:\\Windows\\System32\\d3dcompiler_47.D3DSetBlobPart")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DStripShader=C:\\Windows\\System32\\d3dcompiler_47.D3DStripShader")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:D3DWriteBlobToFile=C:\\Windows\\System32\\d3dcompiler_47.D3DWriteBlobToFile")
#pragma comment(                                                               \
    linker,                                                                    \
    "/export:DebugSetMute=C:\\Windows\\System32\\d3dcompiler_47.DebugSetMute")

// --- Payload ---
void LoadBypass() {
  HMODULE hBypass = LoadLibraryA("ace_inject.dll");
  HMODULE hGfx = LoadLibraryA("ace_inject-1.dll");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    // Create a thread to load the bypass DLL to prevent blocking the DllMain
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LoadBypass, NULL, 0, NULL);
  }
  return TRUE;
}

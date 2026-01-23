#define _CRT_SECURE_NO_WARNINGS
#include "MinHook.h"
#include <fstream>
#include <string>
#include <windows.h>

// --- Configuration ---
int g_targetFPS = 300; // Default, can be overridden by fps_config.txt

void LoadConfig() {
  char path[MAX_PATH];
  GetModuleFileNameA(NULL, path, MAX_PATH);

  // Get directory of exe
  std::string dir(path);
  size_t pos = dir.find_last_of("\\/");
  if (pos != std::string::npos) {
    dir = dir.substr(0, pos + 1);
  }

  std::string configPath = dir + "fps_config.txt";
  std::ifstream config(configPath);
  if (config.is_open()) {
    config >> g_targetFPS;
    config.close();
    // Allow -1 (unlimited) or 30-1000
    if (g_targetFPS != -1 && (g_targetFPS < 30 || g_targetFPS > 1000)) {
      g_targetFPS = 300;
    }
  }
}

// --- Il2Cpp API Definitions ---
typedef void *(*Il2CppDomainGet)();
typedef void *(*Il2CppThreadAttach)(void *domain);
typedef void *(*Il2CppGetImage)(void *assembly);
typedef void *(*Il2CppDomainGetAssemblies)(void *domain, size_t *size);
typedef void *(*Il2CppClassFromName)(void *image, const char *namespaze,
                                     const char *name);
typedef void *(*Il2CppClassGetMethodFromName)(void *klass, const char *name,
                                              int argsCount);
typedef void *(*Il2CppRuntimeInvoke)(void *method, void *obj, void **params,
                                     void **exc);

Il2CppDomainGet il2cpp_domain_get;
Il2CppThreadAttach il2cpp_thread_attach;
Il2CppDomainGetAssemblies il2cpp_domain_get_assemblies;
Il2CppClassFromName il2cpp_class_from_name;
Il2CppClassGetMethodFromName il2cpp_class_get_method_from_name;
Il2CppRuntimeInvoke il2cpp_runtime_invoke;
Il2CppGetImage il2cpp_assembly_get_image;

HMODULE hGameAssembly = NULL;
void *g_setTargetFrameRateMethod = NULL;
void *g_setVSyncCountMethod = NULL;
bool g_initialized = false;

// --- Il2Cpp MethodInfo Structure ---
struct Il2CppMethodInfo {
  void *methodPointer;
};

// --- Hook for set_targetFrameRate ---
typedef void (*tSetTargetFrameRate)(int fps);
tSetTargetFrameRate oSetTargetFrameRate = NULL;

void hkSetTargetFrameRate(int fps) {
  oSetTargetFrameRate(g_targetFPS); // Use configured FPS
}

void ResolveIl2Cpp() {
  hGameAssembly = GetModuleHandleW(L"GameAssembly.dll");
  if (!hGameAssembly)
    return;

  il2cpp_domain_get =
      (Il2CppDomainGet)GetProcAddress(hGameAssembly, "il2cpp_domain_get");
  il2cpp_thread_attach =
      (Il2CppThreadAttach)GetProcAddress(hGameAssembly, "il2cpp_thread_attach");
  il2cpp_domain_get_assemblies = (Il2CppDomainGetAssemblies)GetProcAddress(
      hGameAssembly, "il2cpp_domain_get_assemblies");
  il2cpp_class_from_name = (Il2CppClassFromName)GetProcAddress(
      hGameAssembly, "il2cpp_class_from_name");
  il2cpp_class_get_method_from_name =
      (Il2CppClassGetMethodFromName)GetProcAddress(
          hGameAssembly, "il2cpp_class_get_method_from_name");
  il2cpp_runtime_invoke = (Il2CppRuntimeInvoke)GetProcAddress(
      hGameAssembly, "il2cpp_runtime_invoke");
  il2cpp_assembly_get_image = (Il2CppGetImage)GetProcAddress(
      hGameAssembly, "il2cpp_assembly_get_image");
}

void ApplyFPSSettings() {
  if (!g_initialized || !il2cpp_runtime_invoke)
    return;

  // Force configured FPS
  if (g_setTargetFrameRateMethod) {
    void *params[] = {&g_targetFPS};
    il2cpp_runtime_invoke(g_setTargetFrameRateMethod, NULL, params, NULL);
  }

  // Force VSync off
  if (g_setVSyncCountMethod) {
    int vsync = 0;
    void *params[] = {&vsync};
    il2cpp_runtime_invoke(g_setVSyncCountMethod, NULL, params, NULL);
  }
}

// Background thread that continuously enforces FPS
DWORD WINAPI FPSEnforcerThread(LPVOID lpParam) {
  while (true) {
    Sleep(1000); // Check every second
    ApplyFPSSettings();
  }
  return 0;
}

void RunIl2CppHook() {
  if (!hGameAssembly)
    return;
  if (!il2cpp_domain_get || !il2cpp_runtime_invoke)
    return;

  void *domain = il2cpp_domain_get();
  il2cpp_thread_attach(domain);

  size_t size = 0;
  void **assemblies = (void **)il2cpp_domain_get_assemblies(domain, &size);
  void *unityEngineImage = NULL;

  for (size_t i = 0; i < size; i++) {
    void *image = il2cpp_assembly_get_image(assemblies[i]);
    if (il2cpp_class_from_name(image, "UnityEngine", "Application")) {
      unityEngineImage = image;
      break;
    }
  }

  if (!unityEngineImage)
    return;

  // Get Application.set_targetFrameRate
  void *appClass =
      il2cpp_class_from_name(unityEngineImage, "UnityEngine", "Application");
  Il2CppMethodInfo *methodInfo =
      (Il2CppMethodInfo *)il2cpp_class_get_method_from_name(
          appClass, "set_targetFrameRate", 1);

  if (methodInfo && methodInfo->methodPointer) {
    g_setTargetFrameRateMethod = (void *)methodInfo;

    // Hook it
    if (MH_CreateHook(methodInfo->methodPointer, &hkSetTargetFrameRate,
                      (LPVOID *)&oSetTargetFrameRate) == MH_OK) {
      MH_EnableHook(methodInfo->methodPointer);
    }
  }

  // Get QualitySettings.set_vSyncCount
  void *qualityClass = il2cpp_class_from_name(unityEngineImage, "UnityEngine",
                                              "QualitySettings");
  if (qualityClass) {
    Il2CppMethodInfo *vsyncMethod =
        (Il2CppMethodInfo *)il2cpp_class_get_method_from_name(
            qualityClass, "set_vSyncCount", 1);
    if (vsyncMethod) {
      g_setVSyncCountMethod = (void *)vsyncMethod;
    }
  }

  g_initialized = true;

  // Apply settings immediately
  ApplyFPSSettings();

  // Start background enforcer thread
  CreateThread(NULL, 0, FPSEnforcerThread, NULL, 0, NULL);
}

// --- Stealth setup ---
typedef HANDLE(WINAPI *CREATE_FILE_W)(LPCWSTR, DWORD, DWORD,
                                      LPSECURITY_ATTRIBUTES, DWORD, DWORD,
                                      HANDLE);
static CREATE_FILE_W p_CreateFileW = NULL;
typedef DWORD(WINAPI *GET_FILE_ATTRIBUTES_W)(LPCWSTR);
static GET_FILE_ATTRIBUTES_W p_GetFileAttributesW = NULL;

bool ShouldHide(LPCWSTR name) {
  if (!name)
    return false;
  std::wstring n(name);
  for (auto &c : n)
    c = towlower(c);
  return (n.find(L"ace_inject.dll") != std::wstring::npos);
}
HANDLE WINAPI h_CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess,
                            DWORD dwShareMode,
                            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                            DWORD dwCreationDisposition,
                            DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
  if (ShouldHide(lpFileName)) {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return INVALID_HANDLE_VALUE;
  }
  return p_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                       lpSecurityAttributes, dwCreationDisposition,
                       dwFlagsAndAttributes, hTemplateFile);
}
DWORD WINAPI h_GetFileAttributesW(LPCWSTR lpFileName) {
  if (ShouldHide(lpFileName)) {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return INVALID_FILE_ATTRIBUTES;
  }
  return p_GetFileAttributesW(lpFileName);
}

void Setup() {
  // Load config first
  LoadConfig();

  MH_Initialize();

  HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
  if (hKernelBase) {
    MH_CreateHookApi(L"kernelbase.dll", "CreateFileW", (LPVOID)h_CreateFileW,
                     (LPVOID *)&p_CreateFileW);
    MH_CreateHookApi(L"kernelbase.dll", "GetFileAttributesW",
                     (LPVOID)h_GetFileAttributesW,
                     (LPVOID *)&p_GetFileAttributesW);
  }
  MH_EnableHook(MH_ALL_HOOKS);

  // Wait for game to fully initialize
  Sleep(15000);

  ResolveIl2Cpp();
  RunIl2CppHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Setup, NULL, 0, NULL);
  }
  return TRUE;
}

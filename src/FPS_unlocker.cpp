#include "resource.h"
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

bool ExtractResource(int resourceId, const wchar_t *outputPath) {
  HMODULE hModule = GetModuleHandle(NULL);
  HRSRC hRes =
      FindResourceW(hModule, MAKEINTRESOURCEW(resourceId), (LPCWSTR)RT_RCDATA);
  if (!hRes)
    return false;

  HGLOBAL hData = LoadResource(hModule, hRes);
  if (!hData)
    return false;

  void *pData = LockResource(hData);
  DWORD size = SizeofResource(hModule, hRes);
  if (!pData || size == 0)
    return false;

  HANDLE hFile = CreateFileW(outputPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;

  DWORD written;
  WriteFile(hFile, pData, size, &written, NULL);
  CloseHandle(hFile);

  return written == size;
}

int main() {
  std::cout << "==================================" << std::endl;
  std::cout << "  Endfield FPS Unlocker Installer" << std::endl;
  std::cout << "==================================" << std::endl;
  std::cout << std::endl;

  if (GetFileAttributesW(L"Endfield_Data") == INVALID_FILE_ATTRIBUTES) {
    std::cout << "[ERROR] Please run this from the Endfield game folder!"
              << std::endl;
    std::cout << "        (The folder containing Arknights_Endfield.exe)"
              << std::endl;
    std::cout << std::endl;
    system("pause");
    return 1;
  }

  std::cout << "[OK] Game folder detected." << std::endl;
  std::cout << std::endl;

  int targetFPS = 300;
  std::cout << "Enter your desired FPS cap (-1 for unlimited, default: 300): ";
  std::string input;
  std::getline(std::cin, input);

  if (!input.empty()) {
    try {
      targetFPS = std::stoi(input);
      if (targetFPS != -1 && (targetFPS < 30 || targetFPS > 1000)) {
        std::cout << "[WARN] Invalid FPS value. Using 300." << std::endl;
        targetFPS = 300;
      }
    } catch (...) {
      std::cout << "[WARN] Invalid input. Using 300." << std::endl;
      targetFPS = 300;
    }
  }

  std::cout << "[OK] Target FPS set to: " << targetFPS << std::endl;
  std::cout << std::endl;

  std::ofstream configFile("fps_config.txt");
  if (configFile.is_open()) {
    configFile << targetFPS;
    configFile.close();
    std::cout << "[OK] Created fps_config.txt" << std::endl;
  }

  if (ExtractResource(IDR_ACE_INJECT_DLL, L"ace_inject.dll")) {
    std::cout << "[OK] Installed ace_inject.dll" << std::endl;
  } else {
    std::cout << "[ERROR] Failed to extract ace_inject.dll!" << std::endl;
  }

  if (ExtractResource(IDR_D3DCOMPILER_DLL, L"d3dcompiler_47.dll")) {
    std::cout << "[OK] Installed d3dcompiler_47.dll" << std::endl;
  } else {
    std::cout << "[WARN] Failed to extract d3dcompiler_47.dll!" << std::endl;
  }

  if (ExtractResource(IDR_VULKAN_DLL, L"vulkan-1.dll")) {
    std::cout << "[OK] Installed vulkan-1.dll" << std::endl;
  } else {
    std::cout << "[ERROR] Failed to extract vulkan-1.dll!" << std::endl;
  }

  std::fstream bootConfig("Endfield_Data\\boot.config", std::ios::in);
  if (bootConfig.is_open()) {
    std::string content((std::istreambuf_iterator<char>(bootConfig)),
                        std::istreambuf_iterator<char>());
    bootConfig.close();

    // Replace target-frame-rate value
    size_t pos = content.find("target-frame-rate=");
    if (pos != std::string::npos) {
      size_t endPos = content.find('\n', pos);
      if (endPos == std::string::npos)
        endPos = content.length();
      content.replace(pos, endPos - pos,
                      "target-frame-rate=" + std::to_string(targetFPS));

      std::ofstream out("Endfield_Data\\boot.config");
      out << content;
      out.close();
      std::cout << "[OK] Patched boot.config (target-frame-rate=" << targetFPS
                << ")" << std::endl;
    }
  }

  std::cout << std::endl;
  std::cout << "==================================" << std::endl;
  std::cout << "  Installation Complete!" << std::endl;
  std::cout << "==================================" << std::endl;
  std::cout << std::endl;
  std::cout << "FPS Target: " << targetFPS << std::endl;
  std::cout << std::endl;
  std::cout << "You can now launch the game normally." << std::endl;
  std::cout << "To change FPS later, just run this installer again."
            << std::endl;
  std::cout << std::endl;

  system("pause");
  return 0;
}

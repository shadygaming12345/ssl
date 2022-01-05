// Maybe add XOR or a different string encrypter.

#include <Windows.h>
#include <iostream>
#include <string>

#include "MinHook/MinHook.h"
#include "cURL.h"

uintptr_t PatternFind(std::string pattern) // Signature scanning by someone who is not m.
{
    uintptr_t MemoryBase = (uintptr_t)GetModuleHandleA(0);

    static auto patternToByte = [](const char* pattern)
    {
        auto       bytes = std::vector<int>{};
        const auto start = const_cast<char*>(pattern);
        const auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else { bytes.push_back(strtoul(current, &current, 16)); }
        }
        return bytes;
    };

    const auto dosHeader = (PIMAGE_DOS_HEADER)MemoryBase;
    const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)MemoryBase + dosHeader->e_lfanew);

    const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto       patternBytes = patternToByte(pattern.c_str());
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(MemoryBase);

    const auto s = patternBytes.size();
    const auto d = patternBytes.data();

    size_t nFoundResults = 0;

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return (uintptr_t)(&scanBytes[i]);      // Default/First Result.
        }
    }
    return NULL;
}

DWORD WINAPI Startup(LPVOID) // Made by Yani
{
    AllocConsole();
    
    FILE* fptr;
    freopen_s(&fptr, "CONOUT$", "w", stdout);

    // I got this idea from someone, I forgot who though. https://www.coolgenerator.com/ascii-text-generator Font: Block
    const char* ASCII = R"(
                                                           
 _|      _|                _|_|_|                          
 _|_|  _|_|  _|    _|      _|    _|    _|_|    _|      _|  
 _|  _|  _|  _|    _|      _|    _|  _|_|_|_|  _|      _|  
 _|      _|  _|    _|      _|    _|  _|          _|  _|    
 _|      _|    _|_|_|  _|  _|_|_|      _|_|_|      _|      
                   _|                                      
               _|_|            
                            
)";
    std::cout << ASCII << "Welcome to My.Dev!\n";

    auto CurlEasyPattern = PatternFind("89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 48 83 EC 28 48 85 C9");

    MH_CreateHook((PVOID)CurlEasyPattern, CurlDetour, (PVOID*)curl_original);
    MH_EnableHook((PVOID)CurlEasyPattern);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Startup, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


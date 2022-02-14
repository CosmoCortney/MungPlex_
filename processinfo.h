#ifndef PROCESSINFO_H
#define PROCESSINFO_H
#include<windows.h>
#include<TlHelp32.h>
#include<codecvt>
#include<QString>
#include<QDebug>
#include<Dbghelp.h>
#include <locale>
#include <codecvt>
#define MODULE_PAIR std::pair<std::string, unsigned long long>

//manages all required information of a running process
class ProcessInfo
{
private:
    DWORD pid;
    std::wstring processName;
    BOOL is_wowx64;
    HANDLE handle;
    SYSTEM_INFO lpSystemInfo;
    DWORD apiError;
    std::vector<MODULE_PAIR> moduleTable;

public:
    ProcessInfo();
    ProcessInfo(DWORD pid, std::wstring processName)
    {
        this->pid = pid;
        this->processName = processName;

        handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);
        if(handle){ IsWow64Process(handle, &is_wowx64); }

        this->apiError = GetLastError();

    }

    void set_pid();
    void set_processName();
    void set_handle();
    void set_wowx64();

    DWORD get_pid(){ return this->pid; }
    std::wstring get_processName(){ return this->processName; }
    BOOL get_is_wowx64(){ return this->is_wowx64; }
    DWORD get_apiError(){ return this->apiError; }
    HANDLE get_handle(){ return this->handle; }
    std::vector<MODULE_PAIR>& get_modulePairsInfo()
    {
        handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(moduleEntry);

        if (Module32First(handle, &moduleEntry))
        {
            std::string moduleName;
            unsigned long long moduleAddress = 0;
            do
            {
                moduleName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(moduleEntry.szModule);
                moduleAddress = (unsigned long long)moduleEntry.modBaseAddr;
                moduleTable.push_back(MODULE_PAIR(moduleName, moduleAddress));
            } while (Module32Next(handle, &moduleEntry));
        }

        return moduleTable;
    }
};

#endif // PROCESSINFO_H

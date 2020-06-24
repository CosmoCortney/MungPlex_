#ifndef PROCESSINFO_H
#define PROCESSINFO_H
#include<windows.h>
#include<TlHelp32.h>
#include<codecvt>
#include<QString>
#include<QDebug>
#include<Dbghelp.h>

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
};

#endif // PROCESSINFO_H

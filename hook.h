#ifndef HOOK_H
#define HOOK_H
#include<windows.h>
#include<tlhelp32.h>
#include<vector>
#include<codecvt>

//todo: make this a singleton
class Hook      //carries all information required to access a foreign process
{
private:
    DWORD pid;
    HANDLE hProcess;
    HMODULE hModule;
    LPCSTR processName;
    bool is_wowx64;

public:
    Hook(DWORD pid, HANDLE hProcess, LPCSTR processName)
    {
        this->pid = pid;
        this->hProcess = hProcess;
        this->processName = processName;
    }
    Hook(){}
    Hook(DWORD pid){this->pid = pid;}

    DWORD getPid(){return this->pid;}
    HANDLE getHandle(){return this->hProcess;}
    HMODULE getHModule(){return this->hModule;}
    LPCSTR getProcessName(){return this->processName;}
    bool get_is_wowx64(){ return this->is_wowx64; }

    void setPid(DWORD pid){this->pid = pid;}
    void setHandle(HANDLE hProcess){this->hProcess = hProcess;}
    void setHModule(HMODULE hModule){this->hModule = hModule;}
    void setProcessName(LPCSTR processName){this->processName = processName;}
    void set_is_wowx64(bool x){ this->is_wowx64 = x; }
};



#endif // HOOK_H

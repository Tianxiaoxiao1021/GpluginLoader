#ifndef DYNLOAD_H
#define DYNLOAD_H

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE dll_handle;
#else
    #include <dlfcn.h>
    typedef void* dll_handle;
#endif

// 打开动态库
dll_handle dll_open(const char* path);

// 获取函数符号
void* dll_sym(dll_handle handle, const char* symbol);

// 关闭动态库
int dll_close(dll_handle handle);

// 获取错误信息
const char* dll_error(void);

#endif 
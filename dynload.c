#include "dynload.h"
#include <stdio.h>

dll_handle dll_open(const char* path) {
#ifdef _WIN32
    return LoadLibraryA(path);
#else
    return dlopen(path, RTLD_NOW);
#endif
}

void* dll_sym(dll_handle handle, const char* symbol) {
#ifdef _WIN32
    return (void*)GetProcAddress(handle, symbol);
#else
    return dlsym(handle, symbol);
#endif
}

int dll_close(dll_handle handle) {
#ifdef _WIN32
    return FreeLibrary(handle) ? 0 : -1;
#else
    return dlclose(handle);
#endif
}

const char* dll_error(void) {
#ifdef _WIN32
    DWORD error = GetLastError();
    static char buf[128];
    sprintf(buf, "Error code: %lu", error);
    return buf;
#else
    return dlerror();
#endif
} 
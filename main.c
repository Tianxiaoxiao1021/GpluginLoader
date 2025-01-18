#define VERSION 1.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynload.h"
#include "dirent.h"

// 插件API结构体声明
struct git_plugin_api;

// 插件信息结构体
struct plugin_info {
    void* handle;              // 动态库句柄
    char* name;               // 插件名称
    int enabled;             // 是否启用
    char* path;              // 插件路径
};

// 插件列表
static struct plugin_info* plugins = NULL;
static int plugin_count = 0;

int load_plugin(const char* git_path, const char* plugin_path) {
    dll_handle handle;
    char lib_path[1024];

#ifdef _WIN32
    snprintf(lib_path, sizeof(lib_path), "%s/%s.dll", plugin_path,
        strrchr(plugin_path, '/') ? strrchr(plugin_path, '/') + 1 : plugin_path);
#else
    snprintf(lib_path, sizeof(lib_path), "%s/lib%s.so", plugin_path,
        strrchr(plugin_path, '/') ? strrchr(plugin_path, '/') + 1 : plugin_path);
#endif

    handle = dll_open(lib_path);
    if (!handle) {
        fprintf(stderr, "Failed to load plugin: %s\n", dll_error());
        return -1;
    }

    // 获取插件初始化函数
    int (*init_plugin)(struct git_plugin_api*) = dll_sym(handle, "init_plugin");
    if (!init_plugin) {
        fprintf(stderr, "Plugin does not have init_plugin function\n");
        dll_close(handle);
        return -1;
    }

    // 调用插件初始化
    dll_handle git_handle = dll_open(git_path);
    if (!git_handle) {
        fprintf(stderr, "Failed to open Git library: %s\n", dll_error());
        dll_close(handle);
        return -1;
    }

    struct git_plugin_api* api = dll_sym(git_handle, "get_git_plugin_api");
    if (!api) {
        fprintf(stderr, "Failed to get Git plugin API\n");
        dll_close(handle);
        dll_close(git_handle);
        return -1;
    }

    if (init_plugin(api) != 0) {
        fprintf(stderr, "Plugin initialization failed\n");
        dll_close(handle);
        dll_close(git_handle);
        return -1;
    }

    // 保存插件信息
    plugins = realloc(plugins, sizeof(struct plugin_info) * (plugin_count + 1));
    plugins[plugin_count].handle = handle;
    plugins[plugin_count].name = _strdup(strrchr(plugin_path, '/') ?
        strrchr(plugin_path, '/') + 1 : plugin_path);
    plugins[plugin_count].enabled = 1;
    plugins[plugin_count].path = _strdup(plugin_path);
    plugin_count++;

    dll_close(git_handle);
    return 0;
}

int unload_plugin(const char* plugin_name) {
    for (int i = 0; i < plugin_count; i++) {
        if (strcmp(plugins[i].name, plugin_name) == 0) {
            // 调用插件清理函数
            void (*cleanup_plugin)() = dll_sym(plugins[i].handle, "cleanup_plugin");
            if (cleanup_plugin)
                cleanup_plugin();

            dll_close(plugins[i].handle);
            free(plugins[i].name);
            free(plugins[i].path);

            // 移除插件信息
            memmove(&plugins[i], &plugins[i + 1],
                sizeof(struct plugin_info) * (plugin_count - i - 1));
            plugin_count--;
            plugins = realloc(plugins, sizeof(struct plugin_info) * plugin_count);
            return 0;
        }
    }
    return -1;
}

int toggle_plugin(const char* plugin_name, int enable) {
    for (int i = 0; i < plugin_count; i++) {
        if (strcmp(plugins[i].name, plugin_name) == 0) {
            plugins[i].enabled = enable;
            return 0;
        }
    }
    return -1;
}

int main(int argc, char* argv[]) {
    char* git_path = NULL;
    char* plugin_path = NULL;
    char* mode = NULL;

    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-git=", 5) == 0)
            git_path = argv[i] + 5;
        else if (strncmp(argv[i], "-plugin=", 8) == 0)
            plugin_path = argv[i] + 8;
        else if (strncmp(argv[i], "-mode=", 6) == 0)
            mode = argv[i] + 6;
    }

    if (!git_path || !plugin_path || !mode) {
        if (argc <= 1) {
            fprintf(stderr, "GpluginLoader Version %.1f\n", VERSION);
            fprintf(stderr, "copyright(c) 2025 tianxiaoxiao developer team\n");
            fprintf(stderr, "Usage: %s -git=path/to/git -plugin=path/to/plugin -mode=load/unload/enable/disable\n",
                argv[0]);
        }
        else {
            if (argv[1] == "-h" || argv[1] == "--help") {
                fprintf(stderr, "Usage: %s -git=path/to/git -plugin=path/to/plugin -mode=load/unload/enable/disable\n",
                    argv[0]);
            }
            if (argv[1] == "-v" || argv[1] == "--version") {
                fprintf(stderr, "GpluginLoader Version %.1f\n", VERSION);
            }
        }
        return 1;
    }

    if (strcmp(mode, "load") == 0) {
        return load_plugin(git_path, plugin_path);
    }
    else if (strcmp(mode, "unload") == 0) {
        return unload_plugin(plugin_path);
    }
    else if (strcmp(mode, "enable") == 0) {
        return toggle_plugin(plugin_path, 1);
    }
    else if (strcmp(mode, "disable") == 0) {
        return toggle_plugin(plugin_path, 0);
    }
    else {
        fprintf(stderr, "Invalid mode: %s\n", mode);
        return 1;
    }

    return 0;
}
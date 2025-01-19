#ifndef GIT_PLUGIN_API_H
#define GIT_PLUGIN_API_H

#include "repository.h"

struct git_plugin_api {
    // 命令注册相关API
    int (*register_command)(const char *name, int (*fn)(int, const char **, const char *));
    int (*unregister_command)(const char *name);
    struct repository* (*get_repository)(void);
    
    // 仓库信息API
    const char* (*get_repo_name)(void);
    const char* (*get_repo_owner)(void);
    const char* (*get_repo_description)(void);
    char* (*get_repo_readme)(void);

    // 新增API：添加和修改命令
    int (*add_command)(const char *name, int (*fn)(int, const char **, const char *));
    int (*modify_command)(const char *name, int (*fn)(int, const char **, const char *));
};

// 获取插件API实例
struct git_plugin_api* get_git_plugin_api(void);

#endif 
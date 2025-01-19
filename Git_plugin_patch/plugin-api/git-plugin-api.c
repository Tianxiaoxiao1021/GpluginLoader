/*
* GpluginLoader:git-plugin-api.c
* 
* 插件可调用的API实现
*/


#include "git-plugin-api.h"
#include "builtin.h"
#include "repository.h"
#include "strbuf.h"
#include "config.h"

static const char* git_get_repo_name(void) {
    const char *gitdir = repo_git_path(the_repository, "");
    if (!gitdir)
        return NULL;
    const char *last_slash = strrchr(gitdir, '/');
    if (last_slash && strcmp(last_slash + 1, ".git") == 0) {
        const char *parent_dir = last_slash - 1;
        while (parent_dir > gitdir && *parent_dir != '/')
            parent_dir--;
        return parent_dir + 1;
    }
    return NULL;
}

static const char* git_get_repo_owner(void) {
    const char *owner = NULL;
    if (!git_config_get_string("user.name", &owner))
        return owner;
    return NULL;
}

static const char* git_get_repo_description(void) {
    static struct strbuf buf = STRBUF_INIT;
    strbuf_reset(&buf);
    
    if (strbuf_read_file(&buf, repo_git_path(the_repository, "description"), 0) > 0)
        return buf.buf;
        
    return NULL;
}

static char* git_get_repo_readme(void) {
    static const char *readme_names[] = {
        "README.md",
        "README",
        "README.txt",
        NULL
    };
    
    static struct strbuf buf = STRBUF_INIT;
    const char **name;
    
    strbuf_reset(&buf);
    
    for (name = readme_names; *name; name++) {
        if (strbuf_read_file(&buf, *name, 0) > 0)
            return strbuf_detach(&buf, NULL);
    }
    
    return NULL;
}

static int git_add_command(const char *name, int (*fn)(int, const char **, const char *)) {
    return register_builtin(name, fn);
}

static int git_modify_command(const char *name, int (*fn)(int, const char **, const char *)) {
    if (unregister_builtin(name) != 0) {
        return -1;
    }
    return register_builtin(name, fn);
}

static struct git_plugin_api plugin_api = {
    .register_command = register_builtin,
    .unregister_command = unregister_builtin,
    .get_repository = the_repository,
    .get_repo_name = git_get_repo_name,
    .get_repo_owner = git_get_repo_owner,
    .get_repo_description = git_get_repo_description,
    .get_repo_readme = git_get_repo_readme,
    .add_command = git_add_command,
    .modify_command = git_modify_command
};

struct git_plugin_api* get_git_plugin_api(void) {
    return &plugin_api;
}
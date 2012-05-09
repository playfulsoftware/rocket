#ifndef LUAVM_H
#define LUAVM_H

#include <string>

#include <lua.hpp>


class LuaVM {
    public:

        LuaVM();

        ~LuaVM();

        bool runScriptFile(const char* file);

        bool runScriptString(const char* buffer);

        bool runStartupScript(const char* file);

        std::string getConfigString(const char* name);

        double getConfigNumber(const char* name);

    private:
        lua_State* vmState;

        static LuaVM* getVMSelf(lua_State* L);

        // builtin lua functions

        static int luaTrace(lua_State* L);

        static int luaRequire(lua_State* L);

        static int luaModule(lua_State* L);

};

#endif //LUAVM_H

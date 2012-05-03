#ifndef LUAVM_H
#define LUAVM_H

#include <lua.hpp>

#include "FastDelegate.h"

class LuaVM {
    public:

        LuaVM();

        ~LuaVM();

        bool runScriptFile(const char* file);

        bool runScriptString(const char* buffer);



    private:
        lua_State* vmState;

        static LuaVM* getVMSelf(lua_State* L);

        static int luaTrace(lua_State* L);

};

#endif //LUAVM_H

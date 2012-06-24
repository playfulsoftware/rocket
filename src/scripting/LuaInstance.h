// LuaInstance.h
// Copyright 2012 PlayfulSoftware

#ifndef ROCKET_SCRIPTING_LUAINSTANCE_H
#define ROCKET_SCRIPTING_LUAINSTANCE_H

#include <map>
#include <string>

#include <lua.hpp>

namespace Rocket {
namespace Scripting {

class LuaInstance {

    public:

        typedef int LuaObjectRef;

        typedef std::map< const char*, lua_CFunction > LuaModuleMap;

        typedef enum LuaObjectType {
            LuaNil = LUA_TNIL,
            LuaNumber = LUA_TNUMBER,
            LuaBool = LUA_TBOOLEAN,
            LuaString = LUA_TSTRING,
            LuaTable = LUA_TTABLE,
            LuaFunction = LUA_TFUNCTION,
            LuaUserData = LUA_TUSERDATA,
            LuaThread = LUA_TTHREAD,
            LuaPointer = LUA_TLIGHTUSERDATA
        };

        const static LuaObjectRef INVALID_REF = LUA_NOREF;

        LuaInstance();

        ~LuaInstance();

        bool runBuffer(const char* buffer);

        bool getGlobalValue(const char* name, int* value);
        bool getGlobalValue(const char* name, long* value);
        bool getGlobalValue(const char* name, float* value);
        bool getGlobalValue(const char* name, double* value);
        bool getGlobalValue(const char* name, bool* value);
        bool getGlobalValue(const char* name, char** value);

        void setGlobalValue(const char* name, int value);
        void setGlobalValue(const char* name, long value);
        void setGlobalValue(const char* name, float value);
        void setGlobalValue(const char* name, double value);
        void setGlobalValue(const char* name, bool value);
        void setGlobalValue(const char* name, const char* value);

        void DeleteGlobal(const char* name);


        //void addModule(const char* name, LuaModuleMap& map);

    private:
        // make instances non-copyable
        LuaInstance(const LuaInstance& other);
        LuaInstance& operator=(const LuaInstance& other);

        lua_State* vmState;

        // private function to get a reference to the class instance via the lua environment.
        static LuaInstance* getVMSelf(lua_State* L);

        // builtin lua functions

        static int luaTrace(lua_State* L);

        static int luaRequire(lua_State* L);

        static int luaModule(lua_State* L);

};

} // Scripting
} // Rocket

#endif //ROCKET_SCRIPTING_LUAINSTANCE_H

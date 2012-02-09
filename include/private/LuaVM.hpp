#ifndef LUAVM_HPP
#define LUAVM_HPP

#include <lua.hpp>

#include <public/EventListener.hpp>
#include <public/EventSource.hpp>

#include <private/SimpleEventSource.hpp>

class LuaVM : public EventListener, public EventSource {

    private:
        lua_State* vmState;
        SimpleEventSource eventSrc;

        static LuaVM* getVMSelf(lua_State* L);

        static int luaTrace(lua_State* L);

        static int luaRegisterEvent(lua_State* L);

        static int luaEmitEvent(lua_State* L);

        void emitLuaEvent(const char* e);

    public:
        LuaVM();

        ~LuaVM();

        void runScriptFile(const char* file);

        void runScriptString(const char* buffer);

        virtual bool addListener(EventListener* listener);

        virtual bool removeListener(EventListener* listener);

        virtual void emit(const char* e);

        virtual void onEvent(const EventSource* src, const char* e);
};

#endif //LUAVM_HPP

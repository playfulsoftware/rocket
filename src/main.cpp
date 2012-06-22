#include <deque>
#include <iostream>
#include <map>
#include <string>

#include <SDL/SDL.h>

#include "FastDelegate.h"
#include "LuaVM.h"
#include "utils.h"

class EventArgs { 
    public:
        virtual void* getEventData() const = 0;
};

typedef fastdelegate::FastDelegate<void (const EventArgs &)> EventHandler;

class IEventEmitter {
    public:
        virtual void on(const char* eventName, const EventHandler& handler) = 0;
};

class SDLEventArgs : public EventArgs {
    public:
        virtual void* getEventData() const { return NULL; }
};

class SDLEngineLoop : public IEventEmitter {

    private:

        bool shouldRun;
        std::map< std::string, std::deque<EventHandler> > events;

        void emit(const char* name, const EventArgs &e) {
            if (events.find(name) != events.end()) {
                std::deque<EventHandler>::iterator iter = events[name].begin();
                for (iter; iter != events[name].end(); ++iter) {
                    (*iter)(e);
                }
            }
        }

    public:

        virtual void on(const char* eventName, const EventHandler& handler) {
            if (events.find(eventName) == events.end()) {
                std::deque<EventHandler> queue;
                events[eventName] = queue;
            }
            events[eventName].push_back(handler);
        }


        SDLEngineLoop() : shouldRun(true) {
            if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
                std::cerr << "fatal error: unable to initialize SDL" << std::endl;
                exit(1);
            } else {
                std::cerr << "SDL initialized." << std::endl;
            }
        }

        ~SDLEngineLoop() { 
            std::cerr << "shutting down SDL." << std::endl;
            SDL_Quit(); 
        }

        void createWindow(int width, int height) {
            // define the GL settings
            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            int flags = SDL_OPENGL;

            // initialize the window
            const SDL_VideoInfo* info = SDL_GetVideoInfo();

            if (SDL_SetVideoMode(width, height, info->vfmt->BitsPerPixel, flags) == 0)
            {
              // bail
              exit(1);
            }
        }

        int start() {
            std::cerr << "sending start loop message" << std::endl;
            SDLEventArgs start;
            emit("SDLLoopStart", start);
            std::cerr << "message sent" << std::endl;



            SDL_Event evt;

            while (shouldRun) {
                //handle SDL events. 
                while (SDL_PollEvent(&evt)) {
                    switch (evt.type) {
                        case SDL_QUIT:
                            shouldRun = false;
                            break;
                        default:
                            break;
                    }
                }
            }

            return 0;
        }

};

void test(const EventArgs& e) {
    std::cerr << "in event handler func" << std::endl;
}

class TestClass {
    public:
        void func(const EventArgs& e) {
            std::cerr << "in member event handler func" << std::endl;
        }
};

int main(int argc, char** argv) {

    const char* envPath = getenv("PSENGINE_CONFIG");
    std::string configPath;

    if (envPath != NULL) { 
        configPath = envPath;
    } else if (argc > 1) {
        configPath =  argv[1];
    } else {
        configPath = getBinPath(argv[0]) + "config.lua";
    }

    std::cerr << "engine path root: " << configPath << std::endl;


    SDLEngineLoop main;

    TestClass t;

    main.on("SDLLoopStart", EventHandler(test));
    main.on("SDLLoopStart", EventHandler(&t, &TestClass::func));
    LuaVM vm;

    if (!vm.runStartupScript(configPath.c_str())) {
        std::cerr << "an error occurred while loading the lua startup script" << std::endl;
        return 1;
    }

    std::cerr << "screen is " << vm.getConfigNumber("width") << "x" << vm.getConfigNumber("height") << std::endl;

    main.createWindow((int)vm.getConfigNumber("width"), (int)vm.getConfigNumber("height"));

    return main.start();
}


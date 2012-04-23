#!/usr/bin/env python

top = "."
build = "build"

from waflib import TaskGen

@TaskGen.extension(".m")
def m_hook(self, node):
    return self.create_compiled_task("c", node)

ENGINE_SRC = "src"
BUILD_DIR = "build"
SUPPORT_DIR = "support"

LUA_DIR = "%s/lua-5.2.0" % SUPPORT_DIR
SDL_OSX_DIR = "%s/sdl_osx" % SUPPORT_DIR

sdl_main = "%s/SDLMain.m" % SDL_OSX_DIR

INCLUDE = ["include", "%s/src" % LUA_DIR]

def options(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")
    ctx.recurse(LUA_DIR)

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")
    ctx.recurse(LUA_DIR)

    #ctx.check_cc(lib="SDL")
    # OSX-specific SDL config.. this allows us to use the canonical sdl osx 
    # install without having to do re-install it for building this specifically.
    if ctx.env["DEST_OS"] == "darwin":
        ctx.env.FRAMEWORK_SDL = "SDL"
        ctx.env.FRAMEWORK_COCOA = "Cocoa"


def build(ctx):
    
    ctx.recurse(LUA_DIR)

    srcs = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC)
    libs = ["static-lua"]

    if ctx.env["DEST_OS"] == "darwin":
        ctx.objects(
                features = "c",
                source = sdl_main,
                target="sdlmain",
                use=["SDL", "COCOA"]
                )


        libs += ["SDL", "COCOA", "sdlmain"]

    ctx.program(
            source = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC),
            target = "engine",
            includes = INCLUDE,
            use = libs,
            )

    ctx(rule="cp ${SRC} ${TGT}", source="%s/test.lua" % ENGINE_SRC, target="test.lua", always=True)

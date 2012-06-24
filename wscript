#!/usr/bin/env python

top = "."
build = "build"

from waflib import Scripting
from waflib import TaskGen

@TaskGen.extension(".m")
def m_hook(self, node):
    return self.create_compiled_task("c", node)

@TaskGen.extension(".mm")
def mm_hook(self, node):
    return self.create_compiled_task("cxx", node)

ENGINE_SRC = "src"
BUILD_DIR = "build"
DEPS_DIR = "deps"
SUPPORT_DIR = "support"

LUA_DIR = "%s/lua-5.2.0" % DEPS_DIR
LUA_SRC = "%s/src" % LUA_DIR


SDL_OSX_DIR = "%s/sdl1.3_osx" % SUPPORT_DIR

sdl_main = "%s/SDLMain.m" % SDL_OSX_DIR

INCLUDE = [ENGINE_SRC, LUA_SRC]

def options(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")
    ctx.check(features="c cxx", cflags=["-Wall", "-Werror"])

    ctx.check_cc(lib=["m","readline"], uselib_store="lua_deps")

    # OSX-specific SDL config.. this allows us to use the canonical sdl osx 
    # install without having to do re-install it for building this specifically.
    if ctx.env["DEST_OS"] == "darwin":
        ctx.env.FRAMEWORK_SDL = "SDL"
        ctx.env.FRAMEWORK_COCOA = "Cocoa"
        ctx.env.FRAMEWORK_OPENGL = "OpenGL"
        ctx.env.FRAMEWORK_CARBON = "Carbon"
        ctx.env.FRAMEWORK_IOKIT = "IOKit"
    else:
        ctx.check_cfg(path="sdl-config", package="", args="--cflags --libs", uselib_store="SDL")

def build(ctx):

    incl = INCLUDE
    libs = []

    srcs = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC)

    lua_srcs = ctx.path.ant_glob("%s/*.c" % LUA_SRC, excl=map(lambda x: "%s/%s" % (LUA_SRC, x), ["lua.c", "luac.c"]))
    
    ctx.stlib(features = "c", source = lua_srcs, target="lua", use=["lua_deps"])

    libs += ["lua"]

    if ctx.env["DEST_OS"] == "darwin":
        ctx.objects(
                features = "c",
                source = sdl_main,
                target="sdlmain",
                use=["SDL", "COCOA"]
                )

        libs += ["COCOA", "sdlmain"]

    ctx.program(
            source = ctx.path.ant_glob("%s/**/*.cpp" % ENGINE_SRC),
            target = "rocketdemo",
            includes = INCLUDE,
            use = libs,
            )

    ctx(rule="cp ${SRC} ${TGT}", source="%s/test.lua" % ENGINE_SRC, target="test.lua", always=True)

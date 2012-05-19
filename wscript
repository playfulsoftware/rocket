#!/usr/bin/env python

top = "."
build = "build"

from waflib import TaskGen
from waflib import Scripting

from waflib.Build import BuildContext
from waflib.Configure import ConfigurationContext

@TaskGen.extension(".m")
def m_hook(self, node):
    return self.create_compiled_task("c", node)

ENGINE_SRC = "src"
BUILD_DIR = "build"
DEPS_DIR = "deps"

LUA_DIR = "%s/lua-5.2.0" % DEPS_DIR
SDL_OSX_DIR = "%s/sdl_osx" % DEPS_DIR

sdl_main = "%s/SDLMain.m" % SDL_OSX_DIR

INCLUDE = ["include", "%s/src" % LUA_DIR]

class ModuleFinderContext(ConfigurationContext):
    cmd = "find_deps"
    fun = "find_deps"
    

def options(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")
    ctx.recurse(LUA_DIR)

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")
    ctx.check(features="c cxx", cflags=["-Wall", "-Werror"])
    ctx.recurse(LUA_DIR)

    # OSX-specific SDL config.. this allows us to use the canonical sdl osx 
    # install without having to do re-install it for building this specifically.
    if ctx.env["DEST_OS"] == "darwin":
        ctx.env.FRAMEWORK_SDL = "SDL"
        ctx.env.FRAMEWORK_COCOA = "Cocoa"
    else:
        ctx.check_cfg(path="sdl-config", package="", args="--cflags --libs", uselib_store="SDL")

    Scripting.run_command("find_deps")

def find_deps(ctx):
    print ("resolving module dependencies.")
    ctx.env.deps = []

def build(ctx):

    print ("building dependencies")
    for dep in ctx.env.deps:
        pass
    
    ctx.recurse(LUA_DIR)

    srcs = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC)
    libs = ["static-lua", "SDL"]

    if ctx.env["DEST_OS"] == "darwin":
        ctx.objects(
                features = "c",
                source = sdl_main,
                target="sdlmain",
                use=["SDL", "COCOA"]
                )

        libs += ["COCOA", "sdlmain"]

    ctx.program(
            source = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC),
            target = "engine",
            includes = INCLUDE,
            use = libs,
            )

    ctx(rule="cp ${SRC} ${TGT}", source="%s/test.lua" % ENGINE_SRC, target="test.lua", always=True)

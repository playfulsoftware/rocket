#!/usr/bin/env python

top = "."
build = "build"

from waflib import TaskGen

@TaskGen.extension(".m")
def m_hook(self, node):
    return self.create_compiled_task("c", node)

ENGINE_SRC = "src"
BUILD_DIR = "build"
DEPS_DIR = "deps"
SUPPORT_DIR = "support"

LUA_DIR = "%s/lua-5.2.0" % DEPS_DIR
SDL_OSX_DIR = "%s/sdl_osx" % SUPPORT_DIR

sdl_main = "%s/SDLMain.m" % SDL_OSX_DIR

INCLUDE = ["include", "%s/src" % LUA_DIR]


def options(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")
    ctx.load("module")

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")
    ctx.load("module")
    ctx.check(features="c cxx", cflags=["-Wall", "-Werror"])

    # OSX-specific SDL config.. this allows us to use the canonical sdl osx 
    # install without having to do re-install it for building this specifically.
    if ctx.env["DEST_OS"] == "darwin":
        ctx.env.FRAMEWORK_SDL = "SDL"
        ctx.env.FRAMEWORK_COCOA = "Cocoa"
    else:
        ctx.check_cfg(path="sdl-config", package="", args="--cflags --libs", uselib_store="SDL")

    ctx.find_dependencies(DEPS_DIR)

    for dep in ctx.load_dependencies():
        if dep.ctype() is "cxx":
            ctx.check_cxx(cxxflags=dep.cxxflags(), uselib_store=dep.lib_name())
        else:
            ctx.check_cc(cflags=dep.cflags(), uselib_store=dep.lib_name())
        ctx.check(lib=dep.libs(), defines=dep.defines(), uselib_store=dep.lib_name())

    ctx.env.DEP_TYPE = "static"


def build(ctx):

    for dep in ctx.load_dependencies():
        print "building %s" % dep.lib_name()
        dep.build(ctx)


    srcs = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC)
    libs = ["lua-static", "SDL"]

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

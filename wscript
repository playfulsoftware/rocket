#!/usr/bin/env python

top = "."
build = "build"

import os.path
import subprocess

from waflib.Configure import conf
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

LUA_DIR = "%s/lua-5.2.1" % DEPS_DIR
LUA_SRC = "%s/src" % LUA_DIR

SDL_OSX_DIR = "%s/sdl1.3_osx" % SUPPORT_DIR

sdl_main = "%s/SDLMain.m" % SDL_OSX_DIR

INCLUDE = [ENGINE_SRC, LUA_SRC]

REPOS = {
        "sdl2": {
            "type": "hg",
            "url": "http://hg.libsdl.org/SDL"
        }
}

@conf
def clone_hg_repo(ctx, name, url):
    dest_dir = os.path.join(DEPS_DIR, name)
    ctx.start_msg("Downloading %s" % name)
    ctx.cmd_and_log([ctx.env.HG, "clone", url, dest_dir])
    ctx.end_msg("OK")



def options(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")

    ## search for tools needed to grab the external dependencies.
    ctx.find_program("git")
    ctx.find_program("hg")

    # get repo checkouts of dependencies
    for repo, repo_data in REPOS.items():
        ctx.start_msg("Checking for %s repository" % repo)
        if os.path.exists(os.path.join(DEPS_DIR, repo)):
            ctx.end_msg("OK")
        else:
            ctx.end_msg("NOT FOUND", "YELLOW")
            if repo_data["type"] is "hg":
                ctx.clone_hg_repo(repo, repo_data["url"])

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

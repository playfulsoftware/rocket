#!/usr/bin/env python

top = "."
build = "build"

import os
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
    ctx.end_msg("ok")

@conf
def clone_git_repo(ctx, name, url):
    dest_dir = os.path.join(DEPS_DIR, name)
    ctx.start_msg("Downloading %s" % name)
    ctx.cmd_and_log([ctx.env.GIT, "clone", url, dest_dir])
    ctx.end_msg("ok")

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
            ctx.end_msg("yes")
        else:
            ctx.end_msg("not found", "YELLOW")
            if repo_data["type"] is "hg":
                ctx.clone_hg_repo(repo, repo_data["url"])
            elif repo_data["type"] is "git":
                ctx.clone_git_repo(repo, repo_data["url"])
        ctx.env["DEP_%s_PATH" % repo.upper()]  = os.path.join(os.getcwd(), DEPS_DIR, repo)

    ctx.check(features="c cxx", cflags=["-Wall", "-Werror"])

    ctx.check_cc(lib=["m","readline"], uselib_store="lua_deps")

    if ctx.env["DEST_OS"] == "darwin":
        # SDL on OSX needs these frameworks
        ctx.env.FRAMEWORK_SDL = "SDL"
        ctx.env.FRAMEWORK_COCOA = "Cocoa"
        ctx.env.FRAMEWORK_OPENGL = "OpenGL"
    else:
        ctx.check_cfg(path="sdl-config", package="", args="--cflags --libs", uselib_store="SDL")

@conf
def build_lua(ctx):
    lua_srcs = ctx.path.ant_glob("%s/*.c" % LUA_SRC, excl=map(lambda x: "%s/%s" % (LUA_SRC, x), ["lua.c", "luac.c"]))
    ctx.stlib(features = "c", source = lua_srcs, target="lua", use=["lua_deps"])
    return ["lua"]


def build(ctx):

    src = []
    incls = INCLUDE
    libs = []

    libs += ctx.build_lua()

    # OSX-specific files
    if ctx.env["DEST_OS"] == "darwin":
        src += [sdl_main]
        libs += ["COCOA", "SDL", "OPENGL"]

    src += ctx.path.ant_glob("%s/**/*.cpp" % ENGINE_SRC)

    ctx.program(
            features = "c cxx",
            source = src,
            target = "rocketdemo",
            includes = incls,
            use = libs,
            )

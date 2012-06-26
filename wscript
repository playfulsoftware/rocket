#!/usr/bin/env python

top = "."
build = "build"

import os
import subprocess

from waflib import TaskGen
from waflib.Configure import conf
from waflib.Task import Task, RUN_ME, SKIP_ME

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

INCLUDE = [ENGINE_SRC, LUA_SRC]

REPOS = {
        "sdl2": {
            "type": "hg",
            "url": "http://hg.libsdl.org/SDL"
        },
        "libuv": {
            "type": "git",
            "url": "https://github.com/joyent/libuv.git"
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
        ctx.env.FRAMEWORK_SDL2 = "SDL2"
        ctx.env.FRAMEWORK_COCOA = "Cocoa"
        ctx.env.FRAMEWORK_OPENGL = "OpenGL"
    else:
        ctx.check_cfg(path="sdl-config", package="", args="--cflags --libs", uselib_store="SDL")

@conf
def build_lua(ctx):
    lua_srcs = ctx.path.ant_glob("%s/*.c" % LUA_SRC, excl=map(lambda x: "%s/%s" % (LUA_SRC, x), ["lua.c", "luac.c"]))
    ctx.stlib(features = "c", source = lua_srcs, target="lua", use=["lua_deps"])
    return ["lua"]

class SDL2Builder(Task):

    def work_dir(self):
        return os.path.join(self.env["DEP_SDL2_PATH"], "Xcode", "SDL")

    def build_dir(self):
        return os.path.join(self.work_dir(), "build", "Release")

    def framework_path(self):
        return os.path.join(self.build_dir(), "SDL2.framework")

    def runnable_status(self):
        if os.path.exists(self.framework_path()):
            return SKIP_ME
        else:
            return RUN_ME

    def run(self):
        if self.env["DEST_OS"] is "darwin":
            print ("Building SDL Framework" )
            self.exec_command(["xcodebuild", "-configuration", "Release"], cwd=self.work_dir())


def build(ctx):

    src = []
    incls = INCLUDE
    libs = []

    libs += ctx.build_lua()


    # OSX-specific files
    if ctx.env["DEST_OS"] == "darwin":
        sdl_build = SDL2Builder(env=ctx.env)
        ctx.add_to_group(sdl_build)
        incls += [os.path.join(ctx.env["DEP_SDL2_PATH"], "include")]
        libs += ["COCOA", "SDL2", "OPENGL"]
        ctx.env.FRAMEWORKPATH_SDL2 = sdl_build.build_dir()

    src += ctx.path.ant_glob("%s/**/*.cpp" % ENGINE_SRC)

    ctx.program(
            features = "c cxx",
            source = src,
            target = "rocketdemo",
            includes = incls,
            use = libs,
            )

#!/usr/bin/env python

top = "."
build = "build"

import os
import subprocess

from waflib import TaskGen

@TaskGen.extension(".m")
def m_hook(self, node):
    return self.create_compiled_task("c", node)

@TaskGen.extension(".mm")
def mm_hook(self, node):
    return self.create_compiled_task("cxx", node)

ENGINE_SRC = "src"
BUILD_DIR = "build"
DEPS_DIR = os.path.abspath("deps")

def options(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")

    ctx.check(features="c cxx", cflags=["-Wall", "-Werror"])

    # lua lib info
    ctx.env.STLIB_LUA = "lua"
    ctx.env.STLIBPATH_LUA = os.path.join(DEPS_DIR, "lua-5.2.1", "lua-5.2.1", "src")
    ctx.env.INCLUDES_LUA = ctx.env.STLIBPATH_LUA

    # sdl2 lib info
    ctx.env.LIB_SDL2 = "SDL2"
    sdl2_path = os.path.join(DEPS_DIR, "sdl2-hg")
    ctx.env.LIBPATH_SDL2 = os.path.join(sdl2_path, "sdl_build", "build", ".libs")
    ctx.env.INCLUDES_SDL2 = os.path.join(sdl2_path, "include")

    # libuv lib info
    ctx.env.STLIB_UV = "uv"
    ctx.env.STLIBPATH_UV = os.path.join(DEPS_DIR, "libuv-git")
    ctx.env.INCLUDES_UV = os.path.join(ctx.env.STLIBPATH_UV, "include")

    ctx.env.FRAMEWORK_OPENGL = "OpenGL"


def build(ctx):

    src = []
    incls = [ENGINE_SRC]
    libs = ["LUA", "SDL2", "UV", "OPENGL"]

    src += ctx.path.ant_glob("%s/**/*.cpp" % ENGINE_SRC)

    ctx.program(
            features = "c cxx",
            source = src,
            target = "rocketdemo",
            includes = incls,
            use = libs,
            )

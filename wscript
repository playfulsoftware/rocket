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

    ctx.env.FRAMEWORK_OPENGL = "OpenGL"


def build(ctx):


    ctx.read_stlib("uv", [os.path.join(DEPS_DIR, "libuv-git")])
    ctx.read_stlib("lua", [os.path.join(DEPS_DIR, "lua-5.2.1", "lua-5.2.1", "src")])
    ctx.read_stlib("SDL2", [os.path.join(DEPS_DIR, "sdl2-hg", "sdl_build", "build", ".libs")])

    incls = [ENGINE_SRC]
    incls += [os.path.join(DEPS_DIR, "lua-5.2.1", "lua-5.2.1", "src")]
    incls += [os.path.join(DEPS_DIR, "sdl2-hg", "include")]

    libs = ["lua", "SDL2", "uv", "OPENGL"]

    src = ctx.path.ant_glob("%s/**/*.cpp" % ENGINE_SRC)

    ctx.program(
            features = "c cxx",
            source = src,
            target = "rocketdemo",
            includes = incls,
            use = libs,
            mac_app = True
            )

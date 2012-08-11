#!/usr/bin/env python

top = "."
build = "build"

import os
import sys
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

    ctx.env.DEFINES = []

    if sys.platform == "darwin":
        ctx.env.append_value("DEFINES", "USE_KQUEUE_EVENTS=1")
        ctx.env.append_value("DEFINES", "USE_PTHREAD_THREADS=1")

def build(ctx):

    ctx.read_stlib("lua", [os.path.join(DEPS_DIR, "lua-5.2.1", "lua-5.2.1", "src")])

    incls = [ENGINE_SRC]
    incls += [os.path.join(DEPS_DIR, "lua-5.2.1", "lua-5.2.1", "src")]

    libs = ["lua"]

    src = ctx.path.ant_glob("%s/**/*.cpp" % ENGINE_SRC, excl="%s/platform/**/*.cpp" % ENGINE_SRC)

    if sys.platform == "darwin":
        src += ctx.path.ant_glob("%s/platform/kqueue/*.cpp" % ENGINE_SRC)

    ctx.program(
            features = "c cxx",
            source = src,
            target = "rocketdemo",
            defines = ctx.env.DEFINES,
            includes = incls,
            use = libs,
            mac_app = True
            )

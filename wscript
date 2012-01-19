#!/usr/bin/env python

top = "."
build = "build"

ENGINE_SRC = "src"
INCLUDE = ["include"]

SUPPORT_DIR = "support"

LUA_DIR = "support/lua-5.2.0"
SDL_DIR = "support/SDL-1.2.14"


def options(ctx):
    ctx.load("compiler_cxx")

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.recurse(SUPPORT_DIR)

def build(ctx):

    ctx.program(
            source = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC),
            target = "engine",
            includes = INCLUDE
            )

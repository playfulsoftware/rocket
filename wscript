#!/usr/bin/env python

top = "."
build = "build"

ENGINE_SRC = "src"
SUPPORT_DIR = "support"

LUA_DIR = "%s/lua-5.2.0" % SUPPORT_DIR
SDL_DIR = "%s/SDL-1.2.14" % SUPPORT_DIR

INCLUDE = ["include", "%s/src" % LUA_DIR]

def options(ctx):
    ctx.load("compiler_cxx")
    ctx.recurse(LUA_DIR)

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.recurse(LUA_DIR)

def build(ctx):

    ctx.recurse(LUA_DIR)

    ctx.program(
            source = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC),
            target = "engine",
            includes = INCLUDE,
            use = "static-lua",
            )

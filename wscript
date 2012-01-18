#!/usr/bin/env python

top = "."
build = "build"

ENGINE_SRC = "src"
INCLUDE = ["include"]

def options(ctx):
    ctx.load("compiler_cxx")

def configure(ctx):
    ctx.load("compiler_cxx")

def build(ctx):
    ctx.program(
            source = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC),
            target = "engine",
            includes = INCLUDE
            )

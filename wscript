#!/usr/bin/env python

top = "."
build = "build"

def options(ctx):
    ctx.load("compiler_cxx")

def configure(ctx):
    ctx.load("compiler_cxx")

def build(ctx):
    ctx.program(
            source = "src/main.cpp",
            target = "engine"
            )

#!/usr/bin/env python

top = "."
build = "build"

from waflib import Scripting
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

LUA_DIR = "%s/lua-5.2.0" % DEPS_DIR
LUA_SRC = "%s/src" % LUA_DIR

IRR_DIR = "%s/irrlicht" % DEPS_DIR
IRR_SRC_DIR = "%s/source/Irrlicht" % IRR_DIR
IRR_DIRS = [IRR_SRC_DIR]
IRR_DIRS += ["%s/%s" % (IRR_SRC_DIR, "MacOSX")]
IRR_DIRS += ["%s/%s" % (IRR_SRC_DIR, "aesGladman")]
IRR_DIRS += ["%s/%s" % (IRR_SRC_DIR, "bzip2")]
IRR_DIRS += ["%s/%s" % (IRR_SRC_DIR, "jpeglib")]
IRR_DIRS += ["%s/%s" % (IRR_SRC_DIR, "libpng")]
IRR_DIRS += ["%s/%s" % (IRR_SRC_DIR, "lzma")]
IRR_DIRS += ["%s/%s" % (IRR_SRC_DIR, "zlib")]

BZIP2_EXCL = ["bzip2.c", "bzip2recover.c", "dlltest.c", "ansi2knr.c", "mk251.c", "unzcrash.c", "spewG.c"]
PNG_EXCL = ["pngtest.c"]

JPEG_INCL = ["jcapimin.c", "jcapistd.c", "jccoefct.c", "jccolor.c", "jcdctmgr.c", "jchuff.c", "jcinit.c", "jcmainct.c", "jcmarker.c", "jcmaster.c", 
  "jcomapi.c", "jcparam.c", "jcprepct.c", "jcsample.c", "jctrans.c", "jdapimin.c", "jdapistd.c", "jdatadst.c", "jdatasrc.c", "jdcoefct.c","jdcolor.c",
  "jddctmgr.c", "jdhuff.c", "jdinput.c", "jdmainct.c", "jdmarker.c", "jdmaster.c", "jdmerge.c", "jdpostct.c", "jdsample.c", "jdtrans.c", "jerror.c",
  "jfdctflt.c", "jfdctfst.c", "jfdctint.c", "jidctflt.c", "jidctfst.c", "jidctint.c", "jmemmgr.c", "jmemnobs.c", "jquant1.c", "jquant2.c", "jutils.c",
  "jcarith.c", "jdarith.c", "jaricom.c"]

SDL_OSX_DIR = "%s/sdl_osx" % SUPPORT_DIR

sdl_main = "%s/SDLMain.m" % SDL_OSX_DIR

INCLUDE = [LUA_SRC]

def options(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")

def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.load("compiler_c")
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
    
    ctx.objects(features = "c", source = lua_srcs, target="lua", use=["lua_deps"])

    libs += ["lua"]

    irr_srcs = []
    for path in IRR_DIRS:
      if path.endswith("bzip2"):
        irr_srcs += ctx.path.ant_glob(incl="%s/*.c" % path, excl=map(lambda x: "%s/%s" % (path, x), BZIP2_EXCL))
      elif path.endswith("jpeglib"):
        irr_srcs += ctx.path.ant_glob(incl=map(lambda x: "%s/%s" % (path, x), JPEG_INCL))
      elif path.endswith("libpng"):
        irr_srcs += ctx.path.ant_glob(incl="%s/*.c" % path, excl=map(lambda x: "%s/%s" % (path, x), PNG_EXCL))
      elif path.endswith("MacOSX"):
        irr_srcs += ctx.path.ant_glob(incl="%s/*.mm" % path)
      else:
        irr_srcs += ctx.path.ant_glob(incl=["%s/*.c" % path, "%s/*.cpp" % path])

    #ctx.objects(features = "c cxx", 
    #            source = irr_srcs, 
    #            includes = IRR_DIRS + ["%s/include" % IRR_DIR],
    #            target = "irrlicht")

    #incl += ["%s/include" % IRR_DIR]

    #libs += ["irrlicht"]


    if ctx.env["DEST_OS"] == "darwin":
        ctx.objects(
                features = "c",
                source = sdl_main,
                target="sdlmain",
                use=["SDL", "COCOA"]
                )

        libs += ["COCOA", "sdlmain"]

    ctx.program(
            source = ctx.path.ant_glob("%s/*.cpp" % ENGINE_SRC),
            target = "engine",
            includes = INCLUDE,
            use = libs,
            )

    ctx(rule="cp ${SRC} ${TGT}", source="%s/test.lua" % ENGINE_SRC, target="test.lua", always=True)

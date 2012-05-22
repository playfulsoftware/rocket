#!/usr/bin/env python

import itertools
import json
import os
import os.path

from waflib.Configure import conf

class Module(object):

  def __init__(self, info, platform):
    self.info = info
    self.platform = platform

  def _aggregate_val(self, key):
    ret = []
    for conf in self._config_iter(self.platform):
      ret += map(lambda x: x.encode("ascii"), conf.get(key, []))
    return ret

  def _build_shared(self, ctx):
     ctx.shlib(source=self.source_list(ctx), 
        target=self.lib_name(),
        cflags=self.cflags(),
        cxxflags=self.cxxflags(),
        name=self.shared_name())

  def _build_static(self, ctx):
    ctx.stlib(source=self.source_list(ctx), 
        target=self.lib_name(),
        cflags=self.cflags(),
        cxxflags=self.cxxflags(),
        name=self.static_name())

  def _config_iter(self, platform):
    yield self.info["all"]
    if self.info.has_key(platform):
      yield self.info[platform]

  def _iterate_source_list(self, ctx, conf):
    if not conf.has_key("sources"): return
    for src in conf["sources"]:
      if not src.has_key("include"):
        raise Exception("missing include path")
      inc = os.path.join(self.info["path"], src["include"]).encode("ascii")
      if src.has_key("exclude"):
        if type(src["exclude"]) == str:
          exc = os.path.join(self.info["path"], src["exclude"]).encode("ascii")
        elif type(src["exclude"]) == list:
          exc = map(lambda x: os.path.join(self.info["path"], x).encode("ascii"), src["exclude"])
      else:
        exc = []
      yield ctx.path.ant_glob(inc, excl=exc)

  def build(self, ctx):
    if ctx.env.DEP_TYPE == "shared":
      return self._build_shared(ctx)
    else:
      return self._build_static(ctx)

  def defines(self):
    return self._aggregate_val("defines")

  def cflags(self):
    return self._aggregate_val("cflags")

  def cxxflags(self):
    return self._aggregate_val("cxxflags")

  def ctype(self):
    return self.info["ctype"].encode("ascii")

  def flags(self):
    if self.ctype() is "cxx":
      return self.cxxflags()
    else:
      return self.cflags()

  def includes(self):
    return self._aggregate_val("includes")

  def lib_name(self):
    return self.info["name"].split("-", 1)[0]

  def libs(self):
    return self._aggregate_val("libs")

  def source_list(self, ctx):
    l = []
    for conf in self._config_iter(self.platform):
      for path in self._iterate_source_list(ctx, conf):
        l += path
    return l

  def shared_name(self):
    return "%s-shared" % self.lib_name()

  def static_name(self):
    return "%s-static" % self.lib_name()

def _complete_dep_name(ctx, dn):
  if dn in ctx.env.DEPS:
    return dn
  else:
    for name in ctx.env.DEPS:
      if name.startswith(dn):
        return name
  raise ValueError("Unable to lookup dependency name")


@conf
def find_dependencies(ctx, ddir):
  for d in os.listdir(ddir):
    dep_msg = "Resolving dependecy %s" % d
    dep_path = os.path.join(ddir, d)

    dep_file = os.path.join(dep_path, "module.json")
    if not os.path.exists(dep_file):
      ctx.msg(dep_msg, "module file not found", "RED")
      continue

    try:
      n = ctx.path.find_node(dep_file)
      dep = json.loads(n.read())
      dep["name"] = os.path.basename(dep_path)
      dep["path"] = dep_path
      if dep.has_key("name"):
        ctx.env.DEPS += [dep["name"]]
        ctx.env["DEPS_%s" % dep["name"]] = d = dep
      else:
        raise Exception("missing name field")
    except IOError as e:
      ctx.msg(dep_msg, "unable to load module file (%s)" % e, "RED")
      continue

    ctx.msg(dep_msg, "ok")

@conf
def load_dependencies(ctx):
  return [ctx.load_dependency(dep) for dep in ctx.env.DEPS]


@conf
def load_dependency(ctx, dep):
  return Module(ctx.env["DEPS_%s" % _complete_dep_name(ctx, dep)],
      ctx.env["DEST_OS"])

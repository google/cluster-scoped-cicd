licenses(["notice"])
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@com_google_asylo//asylo/bazel:asylo.bzl", "cc_enclave_binary")

#http_archive(
#  name = "com_github_libgit2",
#  url = "https://github.com/libgit2/libgit2/releases/download/v1.0.1/libgit2-1.0.1.tar.gz",
#  strip_prefix = "libgit2-1.0.1",
#   build_file = "BUILD.libgit2",
#)

cc_import(
  name = "libgit2",
  #srcs = ["test/libgit2-1.0.1/build/libgit2.so"],
  hdrs = ["test/libgit2-1.0.1/include/git2.h"],
  shared_library = "test/libgit2-1.0.1/build/libgit2.so.1.0",
)

cc_library(
    name = 'hello-world',
    srcs = ['hello-world.cc'],
    #deps = [":libgit2"],
    alwayslink = 1,
)

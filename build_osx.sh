#!/bin/bash
cd "$(dirname "$0")"
clang watcher.c watcher_osx.c -o watcher

#!/usr/bin/env python3

import os
import sys
import subprocess
from pathlib import Path

# ---------- cores (opcional) ----------
G = '\033[92m'
Y = '\033[93m'
R = '\033[91m'
B = '\033[94m'
X = '\033[0m'

def log(c, m):
    print(f"{c}{m}{X}")

# ---------- pastas ----------
SRC_DIRS = [
    "src",
    "../libbu/src",
]

INCLUDE_DIRS = [
    "src",
    "../libbu/src",
    "../libbu/include"
]

BUILD_DIR = "playground/build"
OUTPUT = "playground/bulang.js"

# ---------- flags ----------
CXXFLAGS = [
    "-std=c++17",
    "-O3",
    "-DNDEBUG",
    "-Wall"
]

EMFLAGS = [
    "-O3",
    "-s", "WASM=1",
    "-s", "ALLOW_MEMORY_GROWTH=1",
    "-s", "MODULARIZE=1",
    "-s", "EXPORT_NAME=createBuLangModule",
    "--no-entry",
    "-lembind"
]

# ---------- utils ----------
def needs_rebuild(src, obj):
    if not os.path.exists(obj):
        return True
    return os.path.getmtime(src) > os.path.getmtime(obj)

def find_cpp():
    files = []
    for d in SRC_DIRS:
        p = Path(d)
        if p.exists():
            files += list(p.rglob("*.cpp"))
    return files

def include_flags():
    return [f"-I{d}" for d in INCLUDE_DIRS]

# ---------- build ----------
def build():
    log(B, "Building WASM")

    try:
        subprocess.run(["em++", "--version"], check=True, capture_output=True)
    except:
        log(R, "em++ not found")
        return 1

    os.makedirs(BUILD_DIR, exist_ok=True)

    sources = find_cpp()
    if not sources:
        log(R, "no .cpp files")
        return 1

    objects = []
    rebuilt = False

    for src in sources:
        obj = os.path.join(BUILD_DIR, src.stem + ".o")

        if not needs_rebuild(src, obj):
            log(G, f"cache {src}")
            objects.append(obj)
            continue

        log(Y, f"compile {src}")

        cmd = (
            ["em++"]
            + CXXFLAGS
            + include_flags()
            + ["-c", str(src), "-o", obj]
        )

        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode != 0:
            log(R, "compile error")
            print(r.stderr)
            return 1

        rebuilt = True
        objects.append(obj)

    if not rebuilt and os.path.exists(OUTPUT):
        log(G, "nothing changed")
        return 0

    log(B, "linking")

    cmd = ["em++"] + objects + EMFLAGS + ["-o", OUTPUT]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        log(R, "link error")
        print(r.stderr)
        return 1

    log(G, "done")
    return 0

# ---------- main ----------
if __name__ == "__main__":
    sys.exit(build())


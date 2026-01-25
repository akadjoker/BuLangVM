#!/usr/bin/env python3

import os
import sys
import subprocess
from pathlib import Path

# ---------- cores ----------
G = '\033[92m'
Y = '\033[93m'
R = '\033[91m'
B = '\033[94m'
X = '\033[0m'

def log(c, m):
    print(f"{c}{m}{X}")

# ---------- configuração ----------
RAYLIB_SRC = "external/raylib/src"
RAYLIB_LIB = "external/raylib/src/libraylib.web.a"

SRC_DIRS = [
    "src",
    "../libwdiv/src",
]

INCLUDE_DIRS = [
    "src",
    "../libwdiv/src",
    "../libwdiv/include",
    f"{RAYLIB_SRC}",  # Raylib headers
]

BUILD_DIR = "playground/build"
OUTPUT = "playground/bulang.js"

# ---------- flags ----------
CXXFLAGS = [
    "-std=c++17",
    "-O3",
    "-DNDEBUG",
    "-DPLATFORM_WEB",  # Define para Raylib web
    "-Wall",
    "-Wno-unused-function"
]

EMFLAGS = [
    "-O3",
    "-s", "WASM=1",
    "-s", "USE_GLFW=3",              # GLFW para Raylib
    "-s", "ALLOW_MEMORY_GROWTH=1",
    "-s", "ASYNCIFY",                # Necessário para game loop
    "-s", "MODULARIZE=1",
    "-s", "EXPORT_NAME=createBuLangModule",
    "-s", "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']",
        "-lembind",
        "--no-entry",
        "-flto"
 
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

# ---------- check & compile raylib ----------
def check_raylib():
    log(B, "Checking Raylib...")
    
    if not os.path.exists(RAYLIB_SRC):
        log(Y, "Cloning Raylib...")
        try:
            subprocess.run([
                'git', 'clone', '--depth', '1',
                'https://github.com/raysan5/raylib.git',
                'external/raylib'
            ], check=True)
            log(G, "Raylib cloned!")
        except subprocess.CalledProcessError:
            log(R, "Failed to clone Raylib")
            return False
    
    if os.path.exists(RAYLIB_LIB):
        log(G, "Raylib already compiled")
        return True
    
    return compile_raylib()

def compile_raylib():
    log(B, "Compiling Raylib for web...")
    
    try:
        result = subprocess.run(
            ['make', 'PLATFORM=PLATFORM_WEB', '-B'],
            cwd=RAYLIB_SRC,
            capture_output=True,
            text=True,
            timeout=300
        )
        
        if result.returncode == 0:
            log(G, "Raylib compiled!")
            return True
        else:
            log(R, "Raylib compilation failed")
            print(result.stderr)
            return False
    except subprocess.TimeoutExpired:
        log(R, "Raylib compilation timeout")
        return False
    except Exception as e:
        log(R, f"Error: {e}")
        return False

# ---------- build ----------
def build():
    log(B, "=== BuLang Playground + Raylib Builder ===\n")

    # Check em++
    try:
        subprocess.run(["em++", "--version"], check=True, capture_output=True)
    except:
        log(R, "em++ not found! Run: source emsdk/emsdk_env.sh")
        return 1

    # Check/compile Raylib
    if not check_raylib():
        return 1

    os.makedirs(BUILD_DIR, exist_ok=True)

    # Find sources
    sources = find_cpp()
    if not sources:
        log(R, "No .cpp files found")
        return 1

    log(B, f"Found {len(sources)} source files\n")

    # Compile sources
    objects = []
    rebuilt = False
    compiled = 0
    cached = 0

    for src in sources:
        obj = os.path.join(BUILD_DIR, src.stem + ".o")

        if not needs_rebuild(src, obj):
            log(G, f"✓ cached: {src.name}")
            objects.append(obj)
            cached += 1
            continue

        log(Y, f"⚡ compile: {src.name}")

        cmd = (
            ["em++"]
            + CXXFLAGS
            + include_flags()
            + ["-c", str(src), "-o", obj]
        )

        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode != 0:
            log(R, f"✗ compile error: {src.name}")
            print(r.stderr)
            return 1

        rebuilt = True
        compiled += 1
        objects.append(obj)

    print()
    log(B, f"📊 Compiled: {compiled} | Cached: {cached}")

    # Check if link needed
    if not rebuilt and os.path.exists(OUTPUT):
        log(G, "✓ Nothing changed, skipping link")
        return 0

    # Link with Raylib
    print()
    log(B, "🔗 Linking with Raylib...")

    cmd = (
        ["em++"]
        + objects
        + [RAYLIB_LIB]  # Link Raylib
        + EMFLAGS
        + ["-o", OUTPUT]
    )

    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        log(R, "✗ Link error")
        print(r.stderr)
        return 1

    # Report
    print()
    log(G, "✅ Build complete!")
    
    if os.path.exists(OUTPUT):
        size_js = os.path.getsize(OUTPUT) / 1024
        wasm_file = OUTPUT.replace('.js', '.wasm')
        size_wasm = os.path.getsize(wasm_file) / 1024 if os.path.exists(wasm_file) else 0
        
        log(B, f"📦 {OUTPUT} ({size_js:.1f} KB)")
        log(B, f"📦 {wasm_file} ({size_wasm:.1f} KB)")
        log(B, f"📊 Total: {size_js + size_wasm:.1f} KB")
    
    return 0

# ---------- main ----------
if __name__ == "__main__":
    sys.exit(build())

#!/usr/bin/env python3
# ============================================
# playground.py - Build WASM Playground
# ============================================

import os
import sys
import subprocess
from pathlib import Path

# Cores
class C:
    G = '\033[92m'  # Green
    Y = '\033[93m'  # Yellow
    R = '\033[91m'  # Red
    B = '\033[94m'  # Blue
    X = '\033[0m'   # Reset

def log(emoji, msg, color=C.X):
    print(f"{color}{emoji} {msg}{C.X}")

# Config
CONFIG = {
    "src": "src",
    "include": "include",
    "build": "playground/build",
    "output": "playground/bulang.js",
    
    "cxxflags": [
        "-std=c++17",
        "-O3",
        "-DNDEBUG",
        "-flto",
        "-Wall"
    ],
    
    "emflags": [
        "-O3",
        "-s", "WASM=1",
        "-s", "ALLOW_MEMORY_GROWTH=1",
        "-s", "MODULARIZE=1",
        "-s", "EXPORT_NAME='createBuLangModule'",
        "-lembind",
        "--closure", "1",
        "--no-entry",
        "-flto"
    ]
}

def build():
    log("🚀", "Building BuLang Playground WASM", C.B)
    
    # Check emcc
    try:
        subprocess.run(['emcc', '--version'], capture_output=True, check=True)
        log("✅", "Emscripten OK", C.G)
    except:
        log("❌", "Emscripten not found!", C.R)
        return False
    
    # Create build dir
    os.makedirs(CONFIG["build"], exist_ok=True)
    
    # Find ALL .cpp files in src/
    src_dir = Path(CONFIG["src"])
    source_files = list(src_dir.glob("*.cpp"))
    
    if not source_files:
        log("❌", f"No .cpp files found in {CONFIG['src']}/", C.R)
        return False
    
    log("📦", f"Found {len(source_files)} .cpp files", C.B)
    
    # Compile all sources
    objects = []
    
    for src_file in source_files:
        obj_path = os.path.join(CONFIG["build"], src_file.stem + ".o")
        
        cmd = ['em++'] + CONFIG["cxxflags"] + [
            f'-I{CONFIG["include"]}',
            '-c', str(src_file),
            '-o', obj_path
        ]
        
        log("  ", f"Compiling {src_file.name}...", C.Y)
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            log("❌", f"Error compiling {src_file.name}", C.R)
            print(result.stderr)
            return False
        
        objects.append(obj_path)
        log("✅", f"{src_file.name}", C.G)
    
    # Link
    print()
    log("🔗", "Linking WASM...", C.B)
    
    cmd = ['em++'] + objects + CONFIG["emflags"] + [
        '-o', CONFIG["output"]
    ]
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode != 0:
        log("❌", "Link failed!", C.R)
        print(result.stderr)
        return False
    
    # Success!
    log("✅", "Build complete!", C.G)
    
    # Show files
    print()
    log("📦", "Generated files:", C.B)
    output_dir = os.path.dirname(CONFIG["output"])
    output_name = Path(CONFIG["output"]).stem
    
    total_size = 0
    for ext in ['.js', '.wasm']:
        filepath = os.path.join(output_dir, output_name + ext)
        if os.path.exists(filepath):
            size = os.path.getsize(filepath) / 1024
            total_size += size
            log("  ", f"- {filepath} ({size:.1f} KB)", C.G)
    
    log("📊", f"Total size: {total_size:.1f} KB", C.B)
    
    return True

if __name__ == '__main__':
    success = build()
    sys.exit(0 if success else 1)

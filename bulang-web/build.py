#!/usr/bin/env python3
# ============================================
# build.py - BuLang WebAssembly Builder
# Otimizado com cache e build incremental!
# ============================================

import os
import sys
import json
import time
import hashlib
import subprocess
import shutil
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

# ============================================
# Configuração
# ============================================

class BuildMode:
    DEBUG = "debug"
    RELEASE = "release"

def get_config(mode=BuildMode.DEBUG):
    """Get configuration based on build mode"""
    
    base_config = {
        "raylib_src": "external/raylib/src",
        "raylib_lib": "external/raylib/src/libraylib.web.a",
        "src_dir": "src",
        "include_dir": "include",
        "assets_dir": "assets",
        "build_dir": "build",
        "cache_file": "build/.build_cache.json",
        "max_workers": 4,
    }
    
    if mode == BuildMode.RELEASE:
        # 🚀 RELEASE: Máxima otimização
        return {
            **base_config,
            "output": "build/bulang.html",
            "cxxflags": [
                "-std=c++17",
                "-O3",                    # Máxima otimização [web:41]
                "-DNDEBUG",               # Remove asserts
                "-flto",                  # Link-time optimization
                "-fno-exceptions",        # Sem exceções (menor código)
                "-fno-rtti",             # Sem RTTI
                "-Wall",
                "-Wno-unused-function"
            ],
            "emflags": [
                "-O3",                    # Máxima otimização JS/WASM [web:42]
                "-s", "USE_GLFW=3",
                "-s", "ALLOW_MEMORY_GROWTH=1",
                "-s", "TOTAL_MEMORY=67108864",
                "-s", "STACK_SIZE=5242880",
                "-s", "FORCE_FILESYSTEM=1",
                "--preload-file", "assets@/assets",
                "--shell-file", "shell.html",
                "-s", "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']",
                "-s", "EXPORTED_FUNCTIONS=['_main']",
                # Release optimizations
                "-s", "ASSERTIONS=0",     # Remove runtime checks
                "-s", "SAFE_HEAP=0",      # Remove memory safety checks
                "--closure", "1",         # Closure compiler [web:41]
                "-flto",                  # Link-time optimization
                "--minify", "0",         # Minify JS (pode causar problemas)
            ],
        }
    else:
        # 🐛 DEBUG: Mais rápido, com debug info
        return {
            **base_config,
            "output": "build/bulang_debug.html",
            "cxxflags": [
                "-std=c++17",
                "-O2",                    # Otimização moderada
                "-g",                     # Debug symbols
                "-Wall",
                "-DDEBUG"
            ],
            "emflags": [
                "-O2",
                "-g",                     # Debug info
                "-s", "USE_GLFW=3",
                "-s", "ASYNCIFY",
                "-s", "ALLOW_MEMORY_GROWTH=1",
                "-s", "TOTAL_MEMORY=67108864",
                "-s", "STACK_SIZE=5242880",
                "-s", "FORCE_FILESYSTEM=1",
                "--preload-file", "assets@/assets",
                "--shell-file", "shell.html",
                "-s", "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']",
                "-s", "EXPORTED_FUNCTIONS=['_main']",
                "-s", "ASSERTIONS=2",     # Extra runtime checks
                "-s", "SAFE_HEAP=1",      # Memory safety
            ],
        }

# ============================================
# Cores e Emojis
# ============================================

class Colors:
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def log(emoji, msg, color=Colors.RESET):
    print(f"{color}{emoji} {msg}{Colors.RESET}")

def log_section(title):
    print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*50}{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.BLUE}{title:^50}{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.BLUE}{'='*50}{Colors.RESET}\n")

# ============================================
# Cache Management (mantém igual)
# ============================================

class BuildCache:
    def __init__(self, cache_file):
        self.cache_file = cache_file
        self.cache = self._load()
    
    def _load(self):
        if os.path.exists(self.cache_file):
            try:
                with open(self.cache_file, 'r') as f:
                    return json.load(f)
            except:
                return {}
        return {}
    
    def save(self):
        os.makedirs(os.path.dirname(self.cache_file), exist_ok=True)
        with open(self.cache_file, 'w') as f:
            json.dump(self.cache, f, indent=2)
    
    def get_file_hash(self, filepath):
        if not os.path.exists(filepath):
            return None
        md5 = hashlib.md5()
        with open(filepath, 'rb') as f:
            for chunk in iter(lambda: f.read(8192), b''):
                md5.update(chunk)
        return md5.hexdigest()
    
    def needs_rebuild(self, source, output, dependencies=None):
        if not os.path.exists(output):
            return True
        if os.path.getmtime(source) > os.path.getmtime(output):
            return True
        source_hash = self.get_file_hash(source)
        cached = self.cache.get(source, {})
        if cached.get('hash') != source_hash:
            return True
        if dependencies:
            for dep in dependencies:
                if os.path.exists(dep):
                    if os.path.getmtime(dep) > os.path.getmtime(output):
                        return True
        return False
    
    def update(self, source, output):
        self.cache[source] = {
            'hash': self.get_file_hash(source),
            'output': output,
            'timestamp': time.time()
        }

# ============================================
# Build Steps (mantém mas passa CONFIG)
# ============================================

def check_emscripten():
    log_section("🔍 Verificando Emscripten")
    try:
        result = subprocess.run(['emcc', '--version'], 
                              capture_output=True, 
                              text=True,
                              timeout=5)
        version = result.stdout.split('\n')[0]
        log("✅", f"Emscripten encontrado: {version}", Colors.GREEN)
        return True
    except:
        log("❌", "Emscripten não encontrado!", Colors.RED)
        log("ℹ️", "Execute: source /path/to/emsdk/emsdk_env.sh", Colors.YELLOW)
        return False

def check_raylib(cache, CONFIG):
    log_section("📦 Verificando Raylib")
    
    raylib_src = CONFIG["raylib_src"]
    raylib_lib = CONFIG["raylib_lib"]
    
    if not os.path.exists(raylib_src):
        log("❌", "Raylib não encontrado!", Colors.RED)
        log("ℹ️", "Clonando Raylib...", Colors.YELLOW)
        try:
            subprocess.run([
                'git', 'clone', '--depth', '1',
                'https://github.com/raysan5/raylib.git',
                'external/raylib'
            ], check=True)
            log("✅", "Raylib clonado!", Colors.GREEN)
        except subprocess.CalledProcessError:
            log("❌", "Falha ao clonar Raylib!", Colors.RED)
            return False
    
    if os.path.exists(raylib_lib):
        makefile_path = os.path.join(raylib_src, "Makefile")
        if os.path.exists(makefile_path):
            if not cache.needs_rebuild(makefile_path, raylib_lib):
                log("✅", "Raylib já compilado (cached)", Colors.GREEN)
                return True
        else:
            log("✅", "Raylib já compilado (cached)", Colors.GREEN)
            return True
        log("ℹ️", "Raylib precisa recompilação", Colors.YELLOW)
    
    return compile_raylib(cache, CONFIG)

def compile_raylib(cache, CONFIG):
    log("🔨", "Compilando Raylib para Web...", Colors.BLUE)
    raylib_src = CONFIG["raylib_src"]
    
    try:
        result = subprocess.run(
            ['make', 'PLATFORM=PLATFORM_WEB', '-B'],
            cwd=raylib_src,
            capture_output=True,
            text=True,
            timeout=300
        )
        
        if result.returncode == 0:
            log("✅", "Raylib compilado!", Colors.GREEN)
            makefile_path = os.path.join(raylib_src, "Makefile")
            if os.path.exists(makefile_path):
                cache.update(makefile_path, CONFIG["raylib_lib"])
            cache.save()
            return True
        else:
            log("❌", "Erro compilando Raylib!", Colors.RED)
            print(result.stderr)
            return False
    except Exception as e:
        log("❌", f"Erro: {e}", Colors.RED)
        return False

def find_source_files(CONFIG):
    src_dir = Path(CONFIG["src_dir"])
    return list(src_dir.glob("*.cpp"))

def compile_source_file(source_file, cache, CONFIG):
    src_path = str(source_file)
    obj_path = os.path.join(CONFIG["build_dir"], 
                           source_file.stem + ".o")
    
    include_dir = CONFIG["include_dir"]
    headers = list(Path(include_dir).glob("*.hpp")) if os.path.exists(include_dir) else []
    
    if not cache.needs_rebuild(src_path, obj_path, [str(h) for h in headers]):
        return obj_path, True, "cached"
    
    cmd = ['em++'] + CONFIG["cxxflags"] + [
        f'-I{CONFIG["include_dir"]}',
        f'-I{CONFIG["raylib_src"]}',
        '-c', src_path,
        '-o', obj_path
    ]
    
    try:
        result = subprocess.run(cmd, 
                              capture_output=True, 
                              text=True,
                              timeout=60)
        
        if result.returncode == 0:
            cache.update(src_path, obj_path)
            return obj_path, True, "compiled"
        else:
            return obj_path, False, result.stderr
    except Exception as e:
        return obj_path, False, str(e)

def compile_sources(cache, CONFIG):
    log_section("📦 Compilando Fontes")
    
    os.makedirs(CONFIG["build_dir"], exist_ok=True)
    sources = find_source_files(CONFIG)
    
    if not sources:
        log("❌", f"Nenhum ficheiro .cpp encontrado em {CONFIG['src_dir']}/", Colors.RED)
        return None
    
    log("ℹ️", f"Encontrados {len(sources)} ficheiros .cpp", Colors.BLUE)
    
    objects = []
    compiled = 0
    cached = 0
    errors = []
    
    with ThreadPoolExecutor(max_workers=CONFIG["max_workers"]) as executor:
        futures = {
            executor.submit(compile_source_file, src, cache, CONFIG): src 
            for src in sources
        }
        
        for future in as_completed(futures):
            src = futures[future]
            obj_path, success, status = future.result()
            
            if success:
                objects.append(obj_path)
                if status == "compiled":
                    log("✅", f"Compilado: {src.name}", Colors.GREEN)
                    compiled += 1
                else:
                    log("💾", f"Cached: {src.name}", Colors.BLUE)
                    cached += 1
            else:
                log("❌", f"Erro: {src.name}", Colors.RED)
                errors.append((src, status))
    
    print()
    log("📊", f"Compilados: {compiled} | Cached: {cached} | Erros: {len(errors)}", 
        Colors.BLUE)
    
    if errors:
        print()
        for src, error in errors:
            log("❌", f"{src.name}:", Colors.RED)
            print(error)
        return None
    
    cache.save()
    return objects

def link_wasm(objects, CONFIG):
    log_section("🌐 Linking WebAssembly")
    
    cmd = ['em++'] + objects + [
        CONFIG["raylib_lib"]
    ] + CONFIG["emflags"] + [
        '-o', CONFIG["output"]
    ]
    
    log("ℹ️", "Linking...", Colors.BLUE)
    
    try:
        result = subprocess.run(cmd, 
                              capture_output=True, 
                              text=True,
                              timeout=300)  # Aumentado para closure
        
        if result.returncode == 0:
            log("✅", "Link completo!", Colors.GREEN)
            
            output_dir = os.path.dirname(CONFIG["output"])
            output_name = Path(CONFIG["output"]).stem
            
            print()
            log("📦", "Ficheiros gerados:", Colors.BLUE)
            total_size = 0
            for ext in ['.html', '.js', '.wasm', '.data']:
                filepath = os.path.join(output_dir, output_name + ext)
                if os.path.exists(filepath):
                    size = os.path.getsize(filepath) / 1024
                    total_size += size
                    log("  ", f"- {filepath} ({size:.1f} KB)", Colors.GREEN)
            
            log("📊", f"Tamanho total: {total_size:.1f} KB", Colors.CYAN)
            return True
        else:
            log("❌", "Erro no link!", Colors.RED)
            print(result.stderr)
            return False
    except Exception as e:
        log("❌", f"Erro: {e}", Colors.RED)
        return False

def run_server(CONFIG):
    log_section("🚀 Iniciando Servidor Web")
    
    build_dir = CONFIG["build_dir"]
    output_file = os.path.basename(CONFIG["output"])
    
    log("ℹ️", f"Servidor em: http://localhost:8000", Colors.BLUE)
    log("ℹ️", f"Abre: http://localhost:8000/{output_file}", Colors.GREEN)
    log("ℹ️", "Pressiona Ctrl+C para parar", Colors.YELLOW)
    
    try:
        subprocess.run(['python3', '-m', 'http.server', '8000'],
                      cwd=build_dir)
    except KeyboardInterrupt:
        log("\n👋", "Servidor parado!", Colors.YELLOW)

# ============================================
# Main Build Function
# ============================================

def build(mode=BuildMode.DEBUG, run_after=False, clean=False):
    start_time = time.time()
    
    # Get config for mode
    CONFIG = get_config(mode)
    
    # Header
    mode_str = "RELEASE 🚀" if mode == BuildMode.RELEASE else "DEBUG 🐛"
    mode_color = Colors.MAGENTA if mode == BuildMode.RELEASE else Colors.CYAN
    
    print()
    print(f"{Colors.BOLD}{mode_color}╔══════════════════════════════════════════════╗{Colors.RESET}")
    print(f"{Colors.BOLD}{mode_color}║   🚀 BuLang WebAssembly Builder ({mode_str})  ║{Colors.RESET}")
    print(f"{Colors.BOLD}{mode_color}╚══════════════════════════════════════════════╝{Colors.RESET}")
    
    if mode == BuildMode.RELEASE:
        log("⚡", "Modo RELEASE: Otimizações máximas (-O3 + LTO + Closure)", Colors.YELLOW)
    
    if clean:
        log_section("🧹 Limpando")
        if os.path.exists(CONFIG["build_dir"]):
            shutil.rmtree(CONFIG["build_dir"])
            log("✅", "Build limpo!", Colors.GREEN)
    
    cache = BuildCache(CONFIG["cache_file"])
    
    if not check_emscripten():
        return False
    
    if not check_raylib(cache, CONFIG):
        return False
    
    objects = compile_sources(cache, CONFIG)
    if not objects:
        return False
    
    if not link_wasm(objects, CONFIG):
        return False
    
    elapsed = time.time() - start_time
    
    print()
    print(f"{Colors.BOLD}{Colors.GREEN}╔══════════════════════════════════════════════╗{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.GREEN}║            ✅ BUILD COMPLETO! ✅             ║{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.GREEN}╚══════════════════════════════════════════════╝{Colors.RESET}")
    log("⏱️", f"Tempo: {elapsed:.2f}s", Colors.BLUE)
    log("📦", f"Output: {CONFIG['output']}", Colors.CYAN)
    
    if run_after:
        print()
        run_server(CONFIG)
    
    return True

# ============================================
# CLI
# ============================================

def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description='BuLang WebAssembly Builder',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
Exemplos:
  python3 build.py                    Build debug
  python3 build.py --release          Build release (O3 + LTO)
  python3 build.py --release --run    Build release + servidor
  python3 build.py --clean            Limpar e rebuild
  python3 build.py --info             Ver configuração
        '''
    )
    
    parser.add_argument('--release', action='store_true',
                       help='Build em modo release (-O3, LTO, closure)')
    parser.add_argument('--run', action='store_true',
                       help='Iniciar servidor web após build')
    parser.add_argument('--clean', action='store_true',
                       help='Limpar build antes')
    parser.add_argument('--info', action='store_true',
                       help='Mostrar configuração')
    parser.add_argument('--server', action='store_true',
                       help='Só servidor (sem build)')
    
    args = parser.parse_args()
    
    mode = BuildMode.RELEASE if args.release else BuildMode.DEBUG
    CONFIG = get_config(mode)
    
    if args.info:
        log_section(f"📋 Configuração ({mode.upper()})")
        for key, value in CONFIG.items():
            print(f"  {key}: {value}")
        return
    
    if args.server:
        run_server(CONFIG)
        return
    
    success = build(mode=mode, run_after=args.run, clean=args.clean)
    sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()

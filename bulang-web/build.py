#!/usr/bin/env python3
# ============================================
# build.py - BuLang Web Builder (Raylib Edition)
# libbu + BuRay + raylib -> WebAssembly (SEM npm no RELEASE)
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
# Configuracao
# ============================================

class BuildMode:
    DEBUG = "debug"
    RELEASE = "release"

EXCLUDED_FILES = {
    "builtins_net.cpp",   # networking (web diferente)
}

BURAY_EXCLUDED = {"plugin.cpp"}  # excluir plugin.cpp do BuRay

# ============================================
# Shell HTML Template (estatico; sem injeção do emcc no RELEASE)
# ============================================

SHELL_TEMPLATE = '''<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no" />
  <title>{title}</title>
  <style>
    * {{ margin: 0; padding: 0; box-sizing: border-box; }}
    body {{
      background: {background};
      display: flex; flex-direction: column;
      align-items: center; justify-content: center;
      min-height: 100vh;
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      color: #e0e0e0;
    }}
    h1 {{ color: #4fc3f7; margin-bottom: 20px; font-size: 1.5rem; }}
    #canvas-container {{
      position: relative;
      background: #0f0f23;
      border-radius: 8px;
      box-shadow: 0 4px 20px rgba(0,0,0,0.5);
      overflow: hidden;
    }}
    canvas {{ display:block; }}
    #loading {{
      position: absolute;
      top: 50%; left: 50%;
      transform: translate(-50%, -50%);
      text-align: center;
    }}
    #loading-spinner {{
      width: 50px; height: 50px;
      border: 4px solid #333;
      border-top-color: #4fc3f7;
      border-radius: 50%;
      animation: spin 1s linear infinite;
      margin: 0 auto 15px;
    }}
    @keyframes spin {{ to {{ transform: rotate(360deg); }} }}
    #progress {{
      width: 200px; height: 8px;
      background: #333;
      border-radius: 4px;
      overflow: hidden;
      margin-top: 10px;
    }}
    #progress-bar {{
      height: 100%;
      background: linear-gradient(90deg, #4fc3f7, #29b6f6);
      width: 0%;
      transition: width 0.3s ease;
    }}
    #status {{ margin-top: 10px; font-size: 0.85rem; color: #888; }}
    .hidden {{ display:none !important; }}
    #output {{
      margin-top: 20px;
      padding: 15px;
      background: #0f0f23;
      border-radius: 8px;
      max-width: {width}px;
      width: 90%;
      max-height: 150px;
      overflow-y: auto;
      font-family: Consolas, Monaco, monospace;
      font-size: 0.85rem;
      white-space: pre-wrap;
      color: #888;
    }}
  </style>
</head>
<body>
  <h1>{title}</h1>

  <div id="canvas-container">
    <div id="loading">
      <div id="loading-spinner"></div>
      <div>Loading...</div>
      <div id="progress"><div id="progress-bar"></div></div>
      <div id="status"></div>
    </div>
    <canvas id="canvas" oncontextmenu="event.preventDefault()" tabindex="-1"></canvas>
  </div>

  <div id="output"></div>

  <script>
    // Emscripten Module (o bulang.js vai usar isto)
    var Module = {{
      print: function(text) {{
        console.log(text);
        var output = document.getElementById('output');
        if (output) {{ output.textContent += text + '\\n'; output.scrollTop = output.scrollHeight; }}
      }},
      printErr: function(text) {{
        console.error(text);
        var output = document.getElementById('output');
        if (output) {{ output.textContent += '[ERR] ' + text + '\\n'; }}
      }},
      canvas: (function() {{
        var canvas = document.getElementById('canvas');
        canvas.addEventListener('webglcontextlost', function(e) {{
          alert('WebGL context lost. Reload the page.');
          e.preventDefault();
        }}, false);
        return canvas;
      }})(),
      setStatus: function(text) {{
        var status = document.getElementById('status');
        var loading = document.getElementById('loading');
        var progressBar = document.getElementById('progress-bar');
        if (!text) {{ if (loading) loading.classList.add('hidden'); return; }}
        if (status) status.textContent = text;
        var match = text.match(/(\\d+)\\/(\\d+)/);
        if (match && progressBar) {{
          progressBar.style.width = (parseInt(match[1]) / parseInt(match[2])) * 100 + '%';
        }}
      }},
      totalDependencies: 0,
      monitorRunDependencies: function(left) {{
        this.totalDependencies = Math.max(this.totalDependencies, left);
        Module.setStatus(left ? 'Loading... (' + (this.totalDependencies - left) + '/' + this.totalDependencies + ')' : '');
      }}
    }};
    Module.setStatus('Downloading...');
    window.onerror = function() {{ Module.setStatus('Error! See console.'); }};
  </script>

  <!-- IMPORTANTE: aqui carregamos o output do em++ -->
  <script src="{js_name}"></script>
</body>
</html>
'''

def load_project_config(assets_dir):
    config_path = os.path.join(assets_dir, "config.json")
    defaults = {"title": "BuLang Web", "width": 800, "height": 600, "background": "#1a1a2e"}
    if os.path.exists(config_path):
        try:
            with open(config_path, "r", encoding="utf-8") as f:
                defaults.update(json.load(f))
        except:
            pass
    return defaults

def write_index_html(assets_dir, out_html_path, js_name):
    cfg = load_project_config(assets_dir)
    html = SHELL_TEMPLATE.format(
        title=cfg["title"],
        width=cfg["width"],
        height=cfg["height"],
        background=cfg["background"],
        js_name=js_name
    )
    with open(out_html_path, "w", encoding="utf-8") as f:
        f.write(html)
    return cfg

def get_config(mode=BuildMode.DEBUG):
    base = {
        "raylib_src": "../external/raylib/src",
        "raylib_lib": "../external/raylib/src/libraylib.a",
        "src_dir": "src",
        "libbu_src": "../libbu/src",
        "buray_src": "../BuRay/src",
        "include_dirs": [
            "../libbu/include",
            "../BuRay/src",
            "../external/raylib/src",
            "src",
        ],
        "build_dir": "build",
        "assets_dir": "assets",
        "cache_file": "build/.build_cache.json",
        "max_workers": 4,
    }

    if mode == BuildMode.RELEASE:
        # ✅ RELEASE: output JS (NÃO HTML) -> nunca tenta minificar HTML -> sem npm
        return {
            **base,
            "output": "build/bulang.js",
            "cxxflags": [
                "-std=c++17",
                "-O3",
                "-DNDEBUG",
                "-flto",
                "-fno-exceptions",
                "-fno-rtti",
                "-Wall",
                "-Wno-unused-function",
                "-Wno-unused-variable",
            ],
            "emflags": [
                "-O3",
                "-s", "WASM=1",
                "-s", "USE_GLFW=3",
                "-s", "ASYNCIFY",
                "-s", "ALLOW_MEMORY_GROWTH=1",
                "-s", "TOTAL_MEMORY=67108864",
                "-s", "STACK_SIZE=5242880",
                "-s", "FORCE_FILESYSTEM=1",
                "--preload-file", "assets@/assets",
                "-s", "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']",
                "-s", "EXPORTED_FUNCTIONS=['_main']",
                "-s", "ASSERTIONS=0",
                "-s", "SAFE_HEAP=0",
                "-flto",
                "--closure", "0",
            ],
        }

    # DEBUG (podes manter HTML gerado se quiseres; aqui também deixo como JS por consistência)
    return {
        **base,
        "output": "build/bulang_debug.js",
        "cxxflags": [
            "-std=c++17",
            "-O2",
            "-g",
            "-Wall",
            "-DDEBUG",
            "-Wno-unused-function",
            "-Wno-unused-variable",
        ],
        "emflags": [
            "-O2",
            "-g",
            "-s", "WASM=1",
            "-s", "USE_GLFW=3",
            "-s", "ASYNCIFY",
            "-s", "ALLOW_MEMORY_GROWTH=1",
            "-s", "TOTAL_MEMORY=67108864",
            "-s", "STACK_SIZE=5242880",
            "-s", "FORCE_FILESYSTEM=1",
            "--preload-file", "assets@/assets",
            "-s", "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']",
            "-s", "EXPORTED_FUNCTIONS=['_main']",
            "-s", "ASSERTIONS=2",
            "-s", "SAFE_HEAP=1",
            "--closure", "0",
        ],
    }

# ============================================
# Cores e Logging
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
# Cache
# ============================================

class BuildCache:
    def __init__(self, cache_file):
        self.cache_file = cache_file
        self.cache = self._load()

    def _load(self):
        if os.path.exists(self.cache_file):
            try:
                with open(self.cache_file, "r", encoding="utf-8") as f:
                    return json.load(f)
            except:
                return {}
        return {}

    def save(self):
        os.makedirs(os.path.dirname(self.cache_file), exist_ok=True)
        with open(self.cache_file, "w", encoding="utf-8") as f:
            json.dump(self.cache, f, indent=2)

    def get_file_hash(self, filepath):
        if not os.path.exists(filepath):
            return None
        md5 = hashlib.md5()
        with open(filepath, "rb") as f:
            for chunk in iter(lambda: f.read(8192), b""):
                md5.update(chunk)
        return md5.hexdigest()

    def needs_rebuild(self, source, output, dependencies=None):
        if not os.path.exists(output):
            return True
        if os.path.getmtime(source) > os.path.getmtime(output):
            return True
        source_hash = self.get_file_hash(source)
        cached = self.cache.get(source, {})
        if cached.get("hash") != source_hash:
            return True
        if dependencies:
            for dep in dependencies:
                if os.path.exists(dep) and os.path.getmtime(dep) > os.path.getmtime(output):
                    return True
        return False

    def update(self, source, output):
        self.cache[source] = {
            "hash": self.get_file_hash(source),
            "output": output,
            "timestamp": time.time()
        }

# ============================================
# Steps
# ============================================

def check_emscripten():
    log_section("Verificando Emscripten")
    try:
        r = subprocess.run(["emcc", "--version"], capture_output=True, text=True, timeout=5)
        log("OK", f"Emscripten: {r.stdout.splitlines()[0]}", Colors.GREEN)
        return True
    except:
        log("ERRO", "Emscripten nao encontrado!", Colors.RED)
        log("INFO", "Corre: source /path/to/emsdk/emsdk_env.sh", Colors.YELLOW)
        return False

def check_raylib(cache, CONFIG):
    log_section("Verificando Raylib")
    if not os.path.exists(CONFIG["raylib_src"]):
        log("ERRO", f"Raylib nao encontrado: {CONFIG['raylib_src']}", Colors.RED)
        return False
    log("OK", f"Raylib: {CONFIG['raylib_src']}", Colors.GREEN)

    if os.path.exists(CONFIG["raylib_lib"]):
        makefile_path = os.path.join(CONFIG["raylib_src"], "Makefile")
        if os.path.exists(makefile_path) and not cache.needs_rebuild(makefile_path, CONFIG["raylib_lib"]):
            log("OK", "Raylib ja compilado (cached)", Colors.GREEN)
            return True
        log("INFO", "Raylib precisa recompilacao", Colors.YELLOW)

    return compile_raylib(cache, CONFIG)

def compile_raylib(cache, CONFIG):
    log("BUILD", "Compilando Raylib para Web...", Colors.BLUE)
    try:
        r = subprocess.run(["make", "PLATFORM=PLATFORM_WEB", "-B"],
                           cwd=CONFIG["raylib_src"],
                           capture_output=True,
                           text=True,
                           timeout=300)
        if r.returncode == 0:
            log("OK", "Raylib compilado!", Colors.GREEN)
            makefile_path = os.path.join(CONFIG["raylib_src"], "Makefile")
            if os.path.exists(makefile_path):
                cache.update(makefile_path, CONFIG["raylib_lib"])
            cache.save()
            return True
        log("ERRO", "Erro compilando Raylib!", Colors.RED)
        print(r.stderr)
        return False
    except Exception as e:
        log("ERRO", f"Erro: {e}", Colors.RED)
        return False

def find_source_files(CONFIG):
    sources = []
    for cpp in Path(CONFIG["src_dir"]).glob("*.cpp"):
        if cpp.name not in EXCLUDED_FILES:
            sources.append(cpp)

    libbu_dir = Path(CONFIG["libbu_src"])
    if libbu_dir.exists():
        for cpp in libbu_dir.glob("*.cpp"):
            if cpp.name not in EXCLUDED_FILES:
                sources.append(cpp)
    else:
        log("WARN", f"libbu nao encontrado em {libbu_dir}", Colors.YELLOW)

    buray_dir = Path(CONFIG["buray_src"])
    if buray_dir.exists():
        for cpp in buray_dir.glob("*.cpp"):
            if cpp.name not in EXCLUDED_FILES and cpp.name not in BURAY_EXCLUDED:
                sources.append(cpp)
    else:
        log("WARN", f"BuRay nao encontrado em {buray_dir}", Colors.YELLOW)

    return sources

def compile_source_file(source_file, cache, CONFIG):
    src_path = str(source_file)

    obj_dir = os.path.join(CONFIG["build_dir"], "obj")
    os.makedirs(obj_dir, exist_ok=True)

    obj_name = f"{source_file.parent.name}_{source_file.stem}.o"
    obj_path = os.path.join(obj_dir, obj_name)

    if not cache.needs_rebuild(src_path, obj_path):
        return obj_path, True, "cached"

    cmd = ["em++"] + CONFIG["cxxflags"]
    for inc in CONFIG["include_dirs"]:
        cmd.append(f"-I{inc}")
    cmd += ["-c", src_path, "-o", obj_path]

    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
        if r.returncode == 0:
            cache.update(src_path, obj_path)
            return obj_path, True, "compiled"
        return obj_path, False, r.stderr
    except Exception as e:
        return obj_path, False, str(e)

def compile_sources(cache, CONFIG):
    log_section("Compilando Fontes")
    os.makedirs(CONFIG["build_dir"], exist_ok=True)

    sources = find_source_files(CONFIG)
    if not sources:
        log("ERRO", "Nenhum .cpp encontrado!", Colors.RED)
        return None

    objects, errors = [], []
    compiled = cached = 0

    with ThreadPoolExecutor(max_workers=CONFIG["max_workers"]) as ex:
        futs = {ex.submit(compile_source_file, s, cache, CONFIG): s for s in sources}
        for fut in as_completed(futs):
            src = futs[fut]
            obj, ok, st = fut.result()
            if ok:
                objects.append(obj)
                if st == "compiled":
                    compiled += 1
                    log("OK", f"Compilado: {src.parent.name}/{src.name}", Colors.GREEN)
                else:
                    cached += 1
            else:
                log("ERRO", f"Erro: {src.name}", Colors.RED)
                errors.append((src, st))

    log("INFO", f"Compilados: {compiled} | Cached: {cached} | Erros: {len(errors)}", Colors.BLUE)

    if errors:
        for src, err in errors:
            log("ERRO", f"{src}:", Colors.RED)
            print(err)
        return None

    cache.save()
    return objects

def link_wasm(objects, CONFIG):
    log_section("Linking WebAssembly")

    cmd = ["em++"] + objects + [CONFIG["raylib_lib"]] + CONFIG["emflags"] + ["-o", CONFIG["output"]]
    log("INFO", "Linking...", Colors.BLUE)

    r = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
    if r.returncode != 0:
        log("ERRO", "Erro no link!", Colors.RED)
        print(r.stderr)
        return False

    log("OK", "Link completo!", Colors.GREEN)
    return True

def write_html_after_link(CONFIG):
    # cria build/index.html sempre, apontando para o JS gerado
    build_dir = CONFIG["build_dir"]
    js_name = os.path.basename(CONFIG["output"])  # ex: bulang.js
    index_path = os.path.join(build_dir, "index.html")

    cfg = write_index_html(CONFIG["assets_dir"], index_path, js_name)
    log("OK", f"Gerado: {index_path} (title={cfg['title']})", Colors.GREEN)

def run_server(CONFIG):
    log_section("Servidor Web")
    log("INFO", "http://localhost:8000/index.html", Colors.GREEN)
    try:
        subprocess.run(["python3", "-m", "http.server", "8000"], cwd=CONFIG["build_dir"])
    except KeyboardInterrupt:
        log("OK", "Servidor parado", Colors.YELLOW)

def export_build(CONFIG, export_dir):
    log_section("Exportando Build")
    os.makedirs(export_dir, exist_ok=True)

    build_dir = CONFIG["build_dir"]
    js_name = os.path.basename(CONFIG["output"])
    wasm_name = Path(js_name).with_suffix(".wasm").name
    data_name = Path(js_name).with_suffix(".data").name

    files = ["index.html", js_name, wasm_name, data_name]
    copied = 0
    for f in files:
        src = os.path.join(build_dir, f)
        if os.path.exists(src):
            shutil.copy2(src, os.path.join(export_dir, f))
            copied += 1

    if copied:
        log("OK", f"Exportado para {export_dir} ({copied} ficheiros)", Colors.CYAN)
        return True
    log("ERRO", "Nada para exportar", Colors.RED)
    return False

def build(mode=BuildMode.DEBUG, run_after=False, clean=False, custom_name=None):
    start = time.time()
    CONFIG = get_config(mode)

    if custom_name:
        base = custom_name.replace(" ", "_")
        # sempre JS agora
        CONFIG["output"] = f"build/{base}.js"

    mode_str = "RELEASE" if mode == BuildMode.RELEASE else "DEBUG"
    mode_color = Colors.MAGENTA if mode == BuildMode.RELEASE else Colors.CYAN

    print(f"\n{Colors.BOLD}{mode_color}{'='*50}{Colors.RESET}")
    print(f"{Colors.BOLD}{mode_color} BuLang Web Builder ({mode_str}){Colors.RESET}")
    print(f"{Colors.BOLD}{mode_color}{'='*50}{Colors.RESET}")

    if clean and os.path.exists(CONFIG["build_dir"]):
        log_section("Limpando")
        shutil.rmtree(CONFIG["build_dir"])
        log("OK", "Build limpo!", Colors.GREEN)

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

    # ✅ gera HTML estatico depois do link (sem emcc -> sem npm)
    write_html_after_link(CONFIG)

    elapsed = time.time() - start
    log("TIME", f"{elapsed:.2f}s", Colors.BLUE)
    log("OUT", f"{CONFIG['build_dir']}/index.html", Colors.CYAN)

    if run_after:
        run_server(CONFIG)

    return True

def main():
    import argparse
    p = argparse.ArgumentParser(description="BuLang Web Builder (sem npm no release)")
    p.add_argument("--release", action="store_true", help="Build release")
    p.add_argument("--run", action="store_true", help="Servidor depois do build")
    p.add_argument("--clean", action="store_true", help="Limpar build")
    p.add_argument("--name", type=str, default=None, help="Nome do output (sem extensao)")
    p.add_argument("--export", type=str, default=None, help="Exportar build para pasta")
    args = p.parse_args()

    mode = BuildMode.RELEASE if args.release else BuildMode.DEBUG
    ok = build(mode=mode, run_after=args.run, clean=args.clean, custom_name=args.name)

    if ok and args.export:
        CONFIG = get_config(mode)
        if args.name:
            CONFIG["output"] = f"build/{args.name.replace(' ', '_')}.js"
        export_build(CONFIG, args.export)

    sys.exit(0 if ok else 1)

if __name__ == "__main__":
    main()

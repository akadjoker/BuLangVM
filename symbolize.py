#!/usr/bin/env python3
import sys
import subprocess
import re
from pathlib import Path
from collections import defaultdict
import argparse

class Symbolizer:
    def __init__(self, executable=None, use_color=True, verbose=False):
        self.executable = executable
        self.use_color = use_color
        self.verbose = verbose
        self.cache = {}
        self.stats = defaultdict(int)
        self.library_cache = {}
        
        # Cores
        self.RESET = '\033[0m' if use_color else ''
        self.BOLD = '\033[1m' if use_color else ''
        self.RED = '\033[31m' if use_color else ''
        self.GREEN = '\033[32m' if use_color else ''
        self.YELLOW = '\033[33m' if use_color else ''
        self.BLUE = '\033[34m' if use_color else ''
        self.CYAN = '\033[36m' if use_color else ''
    
    def find_library_path(self, lib_name):
        """Tenta encontrar o caminho completo de uma biblioteca"""
        if lib_name in self.library_cache:
            return self.library_cache[lib_name]
        
        # Tenta ldconfig
        try:
            result = subprocess.run(['ldconfig', '-p'], 
                                  capture_output=True, text=True, timeout=2)
            for line in result.stdout.split('\n'):
                if lib_name in line:
                    match = re.search(r'=>\s*(.+)', line)
                    if match:
                        path = match.group(1).strip()
                        self.library_cache[lib_name] = path
                        return path
        except:
            pass
        
        # Tenta caminhos comuns
        common_paths = [
            f"/lib/x86_64-linux-gnu/{lib_name}",
            f"/usr/lib/x86_64-linux-gnu/{lib_name}",
            f"/lib/{lib_name}",
            f"/usr/lib/{lib_name}",
        ]
        
        for path in common_paths:
            if Path(path).exists():
                self.library_cache[lib_name] = path
                return path
        
        return None
    
    def extract_frame_info(self, line):
        """Extrai informações do frame de forma mais robusta"""
        # Pattern para AddressSanitizer
        patterns = [
            # Com função e arquivo
            r'#\s*(\d+)\s+(0x[0-9a-f]+)\s+in\s+(.+?)\s+(.+?):(\d+)',
            # Com função sem arquivo
            r'#\s*(\d+)\s+(0x[0-9a-f]+)\s+in\s+(.+?)$',
            # Sem função
            r'#\s*(\d+)\s+(0x[0-9a-f]+)',
        ]
        
        for i, pattern in enumerate(patterns):
            match = re.search(pattern, line.strip())
            if match:
                groups = match.groups()
                info = {
                    'num': int(groups[0]),
                    'address': groups[1],
                    'original_line': line
                }
                
                if i == 0:  # Com função e arquivo
                    info['function'] = groups[2]
                    info['file'] = groups[3]
                    info['line'] = groups[4]
                elif i == 1:  # Com função
                    info['function'] = groups[2]
                
                return info
        
        return None
    
    def get_binary_from_maps(self, address):
        """Tenta descobrir qual binário contém o endereço usando /proc/maps"""
        # Isso só funciona se o processo ainda estiver rodando
        # Útil para análise pós-mortem de core dumps
        return None
    
    def symbolize_with_eu_addr2line(self, binary, address):
        """Tenta com eu-addr2line (elfutils) que é mais robusto"""
        try:
            cmd = ['eu-addr2line', '-e', binary, '-f', '-i', address]
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
            if result.returncode == 0 and result.stdout.strip():
                return result.stdout.strip()
        except:
            pass
        return None
    
    def symbolize_address(self, binary, address):
        """Simboliza endereço com múltiplas ferramentas"""
        if not binary or not Path(binary).exists():
            return None
        
        cache_key = f"{binary}:{address}"
        if cache_key in self.cache:
            self.stats['cache_hits'] += 1
            return self.cache[cache_key]
        
        self.stats['attempts'] += 1
        
        # Tenta múltiplas ferramentas
        tools = [
            ('llvm-symbolizer', lambda b, a: self._symbolize_llvm(b, a)),
            ('addr2line', lambda b, a: self._symbolize_addr2line(b, a)),
            ('eu-addr2line', lambda b, a: self.symbolize_with_eu_addr2line(b, a)),
        ]
        
        for tool_name, symbolize_func in tools:
            result = symbolize_func(binary, address)
            if result and result != '??':
                self.cache[cache_key] = result
                self.stats['success'] += 1
                if self.verbose:
                    print(f"  ✓ Simbolizado com {tool_name}: {address}", file=sys.stderr)
                return result
        
        self.stats['failed'] += 1
        return None
    
    def _symbolize_llvm(self, binary, address):
        """Simboliza com llvm-symbolizer"""
        try:
            cmd = ['llvm-symbolizer', '-e', binary, '-f', '-C', '-i', address]
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
            if result.returncode == 0:
                return result.stdout.strip()
        except:
            pass
        return None
    
    def _symbolize_addr2line(self, binary, address):
        """Simboliza com addr2line"""
        try:
            cmd = ['addr2line', '-e', binary, '-f', '-C', '-i', address]
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
            if result.returncode == 0:
                return result.stdout.strip()
        except:
            pass
        return None
    
    def format_symbol_info(self, symbol_info):
        """Formata informação de símbolo de forma legível"""
        if not symbol_info:
            return None
        
        lines = symbol_info.split('\n')
        formatted = []
        
        i = 0
        while i < len(lines):
            if i + 1 < len(lines):
                func = lines[i].strip()
                loc = lines[i + 1].strip()
                
                if func != '??' and loc != '??:0':
                    # Separa arquivo e linha
                    if ':' in loc:
                        parts = loc.rsplit(':', 1)
                        file = parts[0]
                        line_num = parts[1] if len(parts) > 1 else '?'
                        
                        # Simplifica caminho
                        if file.startswith('/'):
                            file_parts = file.split('/')
                            if len(file_parts) > 3:
                                file = '/'.join(file_parts[-3:])
                        
                        formatted.append({
                            'function': func,
                            'file': file,
                            'line': line_num
                        })
                i += 2
            else:
                i += 1
        
        return formatted if formatted else None
    
    def format_frame(self, frame_info, symbols=None):
        """Formata um frame com cores e indentação"""
        num = frame_info['num']
        addr = frame_info['address']
        
        # Linha principal
        output = f"{self.BLUE}#{num:2d}{self.RESET} {addr}"
        
        if symbols:
            # Primeira função (principal)
            first = symbols[0]
            output += f" in {self.GREEN}{first['function']}{self.RESET}\n"
            output += f"    at {self.YELLOW}{first['file']}:{first['line']}{self.RESET}"
            
            # Funções inlined (se houver)
            if len(symbols) > 1:
                for sym in symbols[1:]:
                    output += f"\n    {self.CYAN}[inlined]{self.RESET} {sym['function']}"
                    output += f"\n    at {self.YELLOW}{sym['file']}:{sym['line']}{self.RESET}"
        
        elif 'function' in frame_info:
            # Usa info do ASan se disponível
            output += f" in {frame_info['function']}"
            if 'file' in frame_info:
                output += f"\n    at {frame_info['file']}"
                if 'line' in frame_info:
                    output += f":{frame_info['line']}"
        else:
            output += f" {self.RED}<não simbolizado>{self.RESET}"
        
        return output
    
    def analyze_leak_summary(self, lines):
        """Analisa resumo de leaks e extrai informações úteis"""
        leak_info = {}
        
        for line in lines:
            if 'leaked in' in line:
                match = re.search(r'(\d+)\s+byte\(s\)\s+leaked\s+in\s+(\d+)', line)
                if match:
                    leak_info['bytes'] = int(match.group(1))
                    leak_info['allocations'] = int(match.group(2))
            
            if 'Direct leak of' in line:
                leak_info['type'] = 'direct'
            elif 'Indirect leak of' in line:
                leak_info['type'] = 'indirect'
        
        return leak_info
    
    def process_trace(self, input_text):
        """Processa trace completo"""
        lines = input_text.split('\n')
        output = []
        
        # Detecta tipo de erro
        error_type = None
        leak_info = {}
        
        for line in lines:
            if 'ERROR: AddressSanitizer' in line:
                error_type = 'asan'
                output.append(f"\n{self.RED}{self.BOLD}{line}{self.RESET}")
                continue
            
            if 'ERROR: LeakSanitizer' in line:
                error_type = 'leak'
                output.append(f"\n{self.YELLOW}{self.BOLD}{line}{self.RESET}")
                continue
            
            # Processa frames
            frame_info = self.extract_frame_info(line)
            if frame_info:
                self.stats['frames_found'] += 1
                
                # Tenta simbolizar
                binary = self.executable
                if binary and Path(binary).exists():
                    symbol_text = self.symbolize_address(binary, frame_info['address'])
                    symbols = self.format_symbol_info(symbol_text)
                    output.append(self.format_frame(frame_info, symbols))
                else:
                    output.append(self.format_frame(frame_info))
            else:
                # Linha normal
                output.append(line)
        
        return '\n'.join(output)
    
    def print_report(self):
        """Imprime relatório final"""
        print(f"\n{self.BOLD}{'='*70}", file=sys.stderr)
        print(f"Relatório de Simbolização", file=sys.stderr)
        print(f"{'='*70}{self.RESET}", file=sys.stderr)
        
        print(f"\n{self.CYAN}Estatísticas:{self.RESET}", file=sys.stderr)
        print(f"  Frames encontrados: {self.stats['frames_found']}", file=sys.stderr)
        print(f"  Tentativas: {self.stats['attempts']}", file=sys.stderr)
        print(f"  Sucesso: {self.GREEN}{self.stats['success']}{self.RESET}", file=sys.stderr)
        print(f"  Falhas: {self.RED}{self.stats['failed']}{self.RESET}", file=sys.stderr)
        print(f"  Cache hits: {self.stats['cache_hits']}", file=sys.stderr)
        
        if self.stats['attempts'] > 0:
            rate = (self.stats['success'] / self.stats['attempts']) * 100
            print(f"\n{self.GREEN}Taxa de sucesso: {rate:.1f}%{self.RESET}", file=sys.stderr)


def main():
    parser = argparse.ArgumentParser(description='Simbolizador melhorado')
    parser.add_argument('input', nargs='?', help='Arquivo de entrada')
    parser.add_argument('-e', '--executable', required=True, help='Executável')
    parser.add_argument('--no-color', action='store_true', help='Sem cores')
    parser.add_argument('-v', '--verbose', action='store_true', help='Verbose')
    
    args = parser.parse_args()
    
    # Lê input
    if args.input:
        with open(args.input) as f:
            text = f.read()
    else:
        text = sys.stdin.read()
    
    # Processa
    sym = Symbolizer(
        executable=args.executable,
        use_color=not args.no_color and sys.stdout.isatty(),
        verbose=args.verbose
    )
    
    output = sym.process_trace(text)
    print(output)
    
    # Relatório
    sym.print_report()

# python3 ../symbolize.py trace.txt -e ./bu --verbose
if __name__ == '__main__':
    main()
#!/usr/bin/env python3
"""
Analisador de leaks do AddressSanitizer
Identifica padrões, agrupa leaks similares e gera relatório
"""

import sys
import re
import json
from collections import defaultdict
from pathlib import Path

class LeakAnalyzer:
    def __init__(self):
        self.leaks = []
        self.leak_groups = defaultdict(list)
        
    def parse_leak_report(self, text):
        """Parse do relatório de leak"""
        lines = text.split('\n')
        current_leak = None
        in_stack = False
        
        for line in lines:
            # Detecta início de leak
            if 'Direct leak of' in line or 'Indirect leak of' in line:
                if current_leak:
                    self.leaks.append(current_leak)
                
                current_leak = {
                    'type': 'direct' if 'Direct' in line else 'indirect',
                    'frames': [],
                    'raw_line': line
                }
                
                # Extrai tamanho
                match = re.search(r'(\d+)\s+byte\(s\)\s+in\s+(\d+)\s+object', line)
                if match:
                    current_leak['bytes'] = int(match.group(1))
                    current_leak['objects'] = int(match.group(2))
                
                in_stack = True
                continue
            
            # Detecta frames
            if in_stack and line.strip().startswith('#'):
                frame_match = re.search(r'#\s*(\d+)\s+(0x[0-9a-f]+)\s+in\s+(.+?)\s+(.+?):(\d+)', line)
                if frame_match:
                    frame = {
                        'num': int(frame_match.group(1)),
                        'address': frame_match.group(2),
                        'function': frame_match.group(3),
                        'file': frame_match.group(4),
                        'line': int(frame_match.group(5))
                    }
                    current_leak['frames'].append(frame)
                else:
                    # Frame sem debug info
                    simple_match = re.search(r'#\s*(\d+)\s+(0x[0-9a-f]+)', line)
                    if simple_match:
                        current_leak['frames'].append({
                            'num': int(simple_match.group(1)),
                            'address': simple_match.group(2),
                            'function': '<unknown>'
                        })
            
            # Fim do stack
            if in_stack and line.strip() == '':
                in_stack = False
        
        # Adiciona último leak
        if current_leak:
            self.leaks.append(current_leak)
    
    def group_leaks(self):
        """Agrupa leaks por padrão de stack"""
        for leak in self.leaks:
            # Cria assinatura baseada nos top frames
            signature = self._get_leak_signature(leak)
            self.leak_groups[signature].append(leak)
    
    def _get_leak_signature(self, leak, depth=3):
        """Cria assinatura do leak baseado nos primeiros frames"""
        sig_parts = []
        for frame in leak['frames'][:depth]:
            if 'function' in frame:
                sig_parts.append(frame['function'])
        return ' -> '.join(sig_parts) if sig_parts else '<no signature>'
    
    def get_allocation_site(self, leak):
        """Identifica local de alocação (primeiro frame útil)"""
        for frame in leak['frames']:
            if 'file' in frame and frame.get('function', '') not in ['malloc', 'realloc', 'calloc', 'new', 'operator new']:
                return f"{frame['file']}:{frame['line']}"
        return '<unknown>'
    
    def print_summary(self):
        """Imprime resumo dos leaks"""
        print("=" * 70)
        print("ANÁLISE DE MEMORY LEAKS")
        print("=" * 70)
        
        total_bytes = sum(l.get('bytes', 0) for l in self.leaks)
        total_objects = sum(l.get('objects', 0) for l in self.leaks)
        
        print(f"\nTotal de leaks: {len(self.leaks)}")
        print(f"Bytes vazados: {total_bytes:,} ({self._format_bytes(total_bytes)})")
        print(f"Objetos vazados: {total_objects}")
        
        # Agrupa por tipo
        direct = [l for l in self.leaks if l['type'] == 'direct']
        indirect = [l for l in self.leaks if l['type'] == 'indirect']
        
        print(f"\nDirect leaks: {len(direct)}")
        print(f"Indirect leaks: {len(indirect)}")
        
        # Top leaks por tamanho
        print(f"\n{'─' * 70}")
        print("TOP 5 LEAKS POR TAMANHO:")
        print(f"{'─' * 70}")
        
        sorted_leaks = sorted(self.leaks, key=lambda x: x.get('bytes', 0), reverse=True)
        for i, leak in enumerate(sorted_leaks[:5], 1):
            bytes_leaked = leak.get('bytes', 0)
            objects = leak.get('objects', 1)
            site = self.get_allocation_site(leak)
            
            print(f"\n#{i} {self._format_bytes(bytes_leaked)} em {objects} objeto(s)")
            print(f"   Alocado em: {site}")
            print(f"   Tipo: {leak['type']}")
    
    def print_grouped_report(self):
        """Imprime leaks agrupados"""
        print(f"\n{'─' * 70}")
        print("LEAKS AGRUPADOS POR PADRÃO:")
        print(f"{'─' * 70}")
        
        sorted_groups = sorted(
            self.leak_groups.items(),
            key=lambda x: sum(l.get('bytes', 0) for l in x[1]),
            reverse=True
        )
        
        for i, (signature, group) in enumerate(sorted_groups[:10], 1):
            total_bytes = sum(l.get('bytes', 0) for l in group)
            total_objects = sum(l.get('objects', 0) for l in group)
            
            print(f"\nGrupo #{i}: {len(group)} leak(s)")
            print(f"  Total: {self._format_bytes(total_bytes)} em {total_objects} objeto(s)")
            print(f"  Padrão: {signature}")
            
            # Mostra primeiro exemplo
            first = group[0]
            site = self.get_allocation_site(first)
            print(f"  Exemplo: {site}")
    
    def print_detailed_report(self, max_leaks=3):
        """Imprime relatório detalhado dos maiores leaks"""
        print(f"\n{'─' * 70}")
        print(f"DETALHAMENTO DOS {max_leaks} MAIORES LEAKS:")
        print(f"{'─' * 70}")
        
        sorted_leaks = sorted(self.leaks, key=lambda x: x.get('bytes', 0), reverse=True)
        
        for i, leak in enumerate(sorted_leaks[:max_leaks], 1):
            print(f"\n{'═' * 70}")
            print(f"LEAK #{i}")
            print(f"{'═' * 70}")
            
            print(f"Tipo: {leak['type']}")
            print(f"Tamanho: {leak.get('bytes', 0)} bytes ({self._format_bytes(leak.get('bytes', 0))})")
            print(f"Objetos: {leak.get('objects', 1)}")
            
            print(f"\nStack Trace:")
            for frame in leak['frames'][:10]:  # Limita a 10 frames
                num = frame.get('num', '?')
                func = frame.get('function', '<unknown>')
                
                print(f"  #{num:2d} {func}")
                
                if 'file' in frame:
                    file = frame['file']
                    line = frame.get('line', '?')
                    # Simplifica caminho
                    if '/' in file:
                        file = '/'.join(file.split('/')[-2:])
                    print(f"      at {file}:{line}")
    
    def export_json(self, filename):
        """Exporta análise em JSON"""
        data = {
            'summary': {
                'total_leaks': len(self.leaks),
                'total_bytes': sum(l.get('bytes', 0) for l in self.leaks),
                'total_objects': sum(l.get('objects', 0) for l in self.leaks),
                'direct_leaks': len([l for l in self.leaks if l['type'] == 'direct']),
                'indirect_leaks': len([l for l in self.leaks if l['type'] == 'indirect']),
            },
            'leaks': self.leaks,
            'groups': {
                sig: {
                    'count': len(group),
                    'total_bytes': sum(l.get('bytes', 0) for l in group),
                    'total_objects': sum(l.get('objects', 0) for l in group),
                }
                for sig, group in self.leak_groups.items()
            }
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
    
    def _format_bytes(self, bytes_count):
        """Formata bytes de forma legível"""
        if bytes_count < 1024:
            return f"{bytes_count} B"
        elif bytes_count < 1024 * 1024:
            return f"{bytes_count / 1024:.1f} KB"
        else:
            return f"{bytes_count / (1024 * 1024):.1f} MB"
    
    def suggest_fixes(self):
        """Sugere correções baseadas nos padrões encontrados"""
        print(f"\n{'─' * 70}")
        print("SUGESTÕES DE CORREÇÃO:")
        print(f"{'─' * 70}")
        
        suggestions = []
        
        # Analisa padrões comuns
        for signature, group in self.leak_groups.items():
            total_bytes = sum(l.get('bytes', 0) for l in group)
            
            if total_bytes > 1024:  # Mais que 1KB
                site = self.get_allocation_site(group[0])
                suggestions.append({
                    'priority': 'HIGH',
                    'location': site,
                    'issue': f"{len(group)} leak(s) totaliz ando {self._format_bytes(total_bytes)}",
                    'suggestion': f"Verificar deallocação em {site}"
                })
        
        # Ordena por prioridade
        for i, sug in enumerate(sorted(suggestions, 
                                       key=lambda x: 0 if x['priority'] == 'HIGH' else 1), 1):
            print(f"\n{i}. [{sug['priority']}] {sug['issue']}")
            print(f"   Local: {sug['location']}")
            print(f"   Ação: {sug['suggestion']}")


def main():
    if len(sys.argv) < 2:
        print("Uso: python leak_analyzer.py <trace_file> [--json output.json]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    json_output = None
    
    if '--json' in sys.argv:
        idx = sys.argv.index('--json')
        if idx + 1 < len(sys.argv):
            json_output = sys.argv[idx + 1]
    
    # Lê arquivo
    with open(input_file) as f:
        text = f.read()
    
    # Analisa
    analyzer = LeakAnalyzer()
    analyzer.parse_leak_report(text)
    analyzer.group_leaks()
    
    # Relatórios
    analyzer.print_summary()
    analyzer.print_grouped_report()
    analyzer.print_detailed_report(max_leaks=3)
    analyzer.suggest_fixes()
    
    # Export JSON
    if json_output:
        analyzer.export_json(json_output)
        print(f"\n✓ Análise exportada para {json_output}")


if __name__ == '__main__':
    main()
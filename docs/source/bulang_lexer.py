from pygments.lexer import RegexLexer, words
from pygments.token import *

class BuLangLexer(RegexLexer):
    name = 'BuLang'
    aliases = ['bulang', 'bu']
    filenames = ['*.bu']

    tokens = {
        'root': [
            # Whitespace
            (r'\s+', Text),

            # Comments
            (r'//.*?$', Comment.Single),
            (r'#.*?$', Comment.Single),
            (r'/\*.*?\*/', Comment.Multiline),

            # Strings (Aspas duplas E simples)
            (r'"(\\\\|\\"|[^"])*"', String),
            (r"'(\\\\|\\'|[^'])*'", String),
            
            # Keywords
            (words((
                'if', 'elif', 'else', 'while', 'for', 'foreach', 'do', 
                'break', 'continue', 'return', 'switch', 'case', 'default',
                'try', 'catch', 'throw', 'goto', 'gosub', 'exit'
            ), suffix=r'\b'), Keyword),

            (words((
                'process', 'fiber', 'frame', 'yield', 'import', 'using', 
                'class', 'struct', 'super', 'this', 'self', 'new'
            ), suffix=r'\b'), Keyword.Reserved),

            # Declaração
            (words(('var', 'def', 'const'), suffix=r'\b'), Keyword.Declaration),

            # Built-ins
            (words((
                'print', 'write', 'format', 'input', 'print_stack', '_gc',
                'type', 'len', 'clock', 'assert'
            ), suffix=r'\b'), Name.Builtin),

            # Constants
            (words(('true', 'false', 'nil', 'null'), suffix=r'\b'), Keyword.Constant),

            # Buffers Types (Adicionado para highlighting correto)
            (words((
                'BUFFER_UINT8', 'BUFFER_INT16', 'BUFFER_UINT16', 
                'BUFFER_INT32', 'BUFFER_UINT32', 'BUFFER_FLOAT', 'BUFFER_DOUBLE',
                'TYPE_UINT8', 'TYPE_INT16', 'TYPE_INT32', 'TYPE_FLOAT'
            ), suffix=r'\b'), Name.Constant),

            # Numbers
            (r'0x[0-9a-fA-F]+', Number.Hex),
            (r'0b[01]+', Number.Bin),
            (r'\d*\.\d+', Number.Float),
            (r'\d+', Number.Integer),

            # Operators (Adicionado o @)
            (r'[\+\-\*/%&\|\^!=<>@]=?', Operator),
            (r'[.,:;\[\]\{\}\(\)]', Punctuation),

            # Identifiers
            (r'[a-zA-Z_]\w*', Name),
        ]
    }

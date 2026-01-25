import sys
import os
project = 'BuLang'
copyright = '2025, BuLang'
author = 'BuLang Team'

extensions = []
templates_path = ['_templates']
exclude_patterns = ['_build']

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_css_files = ['custom.css']

pygments_style = 'monokai'




sys.path.insert(0, os.path.abspath('.'))

from bulang_lexer import BuLangLexer

def setup(app):
    app.add_lexer('bulang', BuLangLexer)

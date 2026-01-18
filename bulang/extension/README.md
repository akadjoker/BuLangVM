# BuLang Language Support

Syntax highlighting and language support for BuLang programming language.

## Features

- **Syntax Highlighting** for BuLang code
- **Auto-closing** brackets, quotes, and parentheses
- **Code folding** support
- **Comment toggling** (line and block comments)

## Language Features

BuLang supports:
- Object-Oriented Programming (classes, inheritance)
- Process-based concurrency
- First-class functions
- Dynamic typing
- Game development integration (Raylib)

## Example
```bulang
class Player {
    var x, y, health;
    
    def init(x, y) {
        self.x = x;
        self.y = y;
        self.health = 100;
    }
    
    def move(dx, dy) {
        self.x = self.x + dx;
        self.y = self.y + dy;
    }
}

var player = Player(100, 100);
player.move(10, 0);
```

## Installation

1. Download the `.vsix` file
2. Open VSCode
3. Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac)
4. Type "Install from VSIX"
5. Select the downloaded file

## Release Notes

### 0.1.0
- Initial release
- Basic syntax highlighting
- Auto-closing pairs
- Comment support

## Links

- [GitHub Repository](https://github.com/your-username/bulang)
- [Language Documentation](https://github.com/your-username/bulang/wiki)

---

**Enjoy coding in BuLang!** 🚀
```

---

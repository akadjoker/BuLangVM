from pyray import *
import random

# --------------------
# Configurações
# --------------------
WIDTH = 800
HEIGHT = 450
GRAVITY = 0.5

# --------------------
# Sprite
# --------------------
class Sprite:
    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)
        self.vx = (random.randint(0, 199) - 100) / 10.0
        self.vy = (random.randint(0, 199) - 100) / 10.0

    def move(self, tex):
        self.x += self.vx
        self.y += self.vy

        self.vy += GRAVITY

        # chão
        if self.y > 400:
            self.y = 400
            self.vy *= -0.85

        # paredes
        if self.x < 0 or self.x > 800:
            self.vx *= -1.0

        draw_texture(tex, int(self.x), int(self.y), WHITE)

# --------------------
# Init
# --------------------
init_window(WIDTH, HEIGHT, "Bunnymark Python (BuLang style)")
set_target_fps(60)

BLACK = Color(0, 0, 0, 255)
WHITE = Color(255, 255, 255, 255)
LIGHTGRAY = Color(200, 200, 200, 255)
RED = Color(255, 0, 0, 255)

tex = load_texture("assets/wabbit_alpha.png")

lista = []

# cria sprites iniciais 
#for i in range(30000):
#    lista.append(Sprite(get_mouse_x(), get_mouse_y()))

# --------------------
# Loop principal
# --------------------
while not window_should_close():

    # adicionar sprites com o rato
    if is_mouse_button_down(MOUSE_LEFT_BUTTON):
        for i in range(500):
            lista.append(Sprite(get_mouse_x(), get_mouse_y()))

    begin_drawing()
    clear_background(BLACK)

    draw_texture(tex, get_mouse_x(), get_mouse_y(), WHITE)

    # update + draw
    for sprite in lista:
        sprite.move(tex)

    draw_text(f"count {len(lista)}", 10, 30, 20, LIGHTGRAY)
    draw_fps(10, 10)

    end_drawing()

# --------------------
# Cleanup
# --------------------
unload_texture(tex)
close_window()

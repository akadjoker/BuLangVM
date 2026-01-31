# 🎮 Pong AI com Rede Neural

Três scripts para treinar e jogar contra uma IA de Pong!

## 📁 Scripts disponíveis

### 1️⃣ `train_offline.bu` - Treino RÁPIDO (sem visualização)
Treina a rede neural em **máxima velocidade** sem nenhuma visualização.
- ⚡ **Mais rápido** (150 gerações em ~20-30 segundos)
- 🔇 Sem visualização
- 💾 Salva em `pong_weights.nn`

```bash
./bu train_offline.bu
```

### 2️⃣ `train_visual.bu` - Treino COM VISUALIZAÇÃO
Treina a rede neural enquanto mostra:
- 🎮 Jogo em tempo real
- 🧠 Visualização da rede neural (neurônios e conexões)
- 📊 Gráfico de evolução do fitness
- 🎯 Informações detalhadas de cada agente
- 💾 Salva em `pong_weights.nn`

```bash
./bu train_visual.bu
```

⚠️ **Mais lento** (150 gerações em ~3-5 minutos), mas super interessante de assistir!

### 3️⃣ `play.bu` - JOGAR contra a IA
Joga contra o modelo treinado.
- 🕹️ Controla com SETAS (UP/DOWN)
- 🤖 IA jogará automaticamente
- ❌ Requer `pong_weights.nn` já treinado

```bash
./bu play.bu
```

## 🚀 Workflow recomendado

### Opção A: Treino Rápido + Jogar
```bash
# 1. Treinar rápido (offline)
./bu train_offline.bu

# 2. Jogar contra a IA
./bu play.bu
```

### Opção B: Treino Bonito + Jogar
```bash
# 1. Treinar com visualização
./bu train_visual.bu

# 2. Jogar contra a IA
./bu play.bu
```

## 📊 O que você verá em `train_visual.bu`

**Esquerda:** Jogo em tempo real
- Paddles e bola
- Placar

**Direita:** Informações técnicas
- 🧠 **Rede Neural**: Visualização dos neurônios
  - Nós azuis/vermelhos = ativação dos neurônios
  - Linhas verdes/vermelhas = pesos das conexões
- 📈 **Gráfico de Fitness**: Evolução ao longo das gerações

**Topo:** Status do treino
- Geração atual
- Agente sendo treinado
- Jogo em andamento
- Fitness atual e hits

## 🧠 Arquitetura da Rede

```
Entradas (5):
├─ Paddle Y (posição vertical da raquete)
├─ Ball X (posição horizontal da bola)
├─ Ball Y (posição vertical da bola)
├─ Ball VX (velocidade horizontal)
└─ Ball VY (velocidade vertical)
        ↓
    Hidden Layer (12 neurônios)
        ↓
Saídas (2):
├─ UP (mover para cima)
└─ DOWN (mover para baixo)
```

## 🎯 Função de Fitness

A IA aprende otimizando:
- **Proximidade**: Quanto mais perto da bola, melhor (+10.0 max)
- **Movimento correto**: Mover na direção certa (+5.0)
- **Penalidade**: Mover na direção errada (-2.0)
- **Acerto**: Rebater a bola (+500)
- **Erro**: Perder ponto (-100)

## 💾 Arquivo de Pesos

`pong_weights.nn` - Arquivo binário com a rede neural treinada
- Criado automaticamente após treino
- Carregado automaticamente em `play.bu`
- Pode ser excluído para treinar do zero: `rm pong_weights.nn`

## ⚙️ Configurações

Para ajustar hiperparâmetros, edite no início de `train_visual.bu` ou `train_offline.bu`:

```bulang
var VISUAL_TRAINING = true;      // true = com visuals, false = offline
var GAMES_PER_GENERATION = 2;    // Jogos por agente por geração
var MAX_GAME_TIME = 15.0;        // Máximo tempo por jogo (segundos)
```

No `GeneticAlgorithm`:
```bulang
var population_size = 10;        // Número de agentes
var mutation_rate = 0.3;         // Taxa de mutação (30%)
var generation_limit = 150;      // Máximo de gerações
```

## 🐛 Troubleshooting

**"pong_weights.nn não encontrado"**
- Execute `train_offline.bu` ou `train_visual.bu` primeiro

**Jogo muito lento**
- Use `train_offline.bu` para treinar (mais rápido)
- Reduza `GAMES_PER_GENERATION` de 2 para 1

**IA sempre perde**
- Deixe treinar mais gerações (aumente generation_limit)
- Ajuste a função de fitness para priorizar certos comportamentos

## 📝 Notas

- A IA aprende por **Algoritmo Genético** (não backprop)
- 150 gerações leva ~20s offline, ~3-5min com visualização
- Os pesos são salvos em **formato binário** para eficiência
- Smooth movement implementado com aceleração (800 px/s²)

---

**Feito com ❤️ usando BuLang + Raylib + Redes Neurais**

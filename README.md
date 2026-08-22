# 💣 Campo Minadinho (Game Boy)

Um clone do clássico Campo Minado (Minesweeper), desenvolvido inteiramente em **C** para o console original **Nintendo Game Boy** (8-bits), utilizando a biblioteca **GBDK-2020**.

## 🎮 Sobre o Projeto

Este projeto foi um excelente desafio de programação de baixo nível (low-level). O objetivo foi entender a fundo as limitações de hardware de um console clássico lançado em 1989, lidando com gerenciamento de VRAM (Video RAM), paletas de cores e uso restrito de CPU e memória RAM.

**Destaques Técnicos:**
- **Algoritmo Flood Fill Otimizado:** Implementação de abertura em cadeia para revelar espaços vazios. Para evitar o temido *Stack Overflow* num hardware de apenas 8KB de RAM, a lógica de recursividade foi convertida em um algoritmo iterativo e altamente seguro.
- **Arte Customizada via Código:** Todos os *sprites* (Cursor, Bombas) e *backgrounds* (Menu, Letras, Blocos) foram desenhados e compilados convertendo Pixel Art diretamente em arrays de matrizes Hexadecimais.
- **Máquina de Estados (State Machine):** Gerenciamento leve das transições de tela entre "Start Menu", "In Game", "Game Over" e "Victory Screen".

## 🕹️ Controles

- **D-Pad (Setas):** Move o cursor pelo tabuleiro ou navega nos menus.
- **Botão A:** Revela o bloco sob o cursor ou confirma opções nos menus.
- **Botão B:** Coloca/Remove uma bandeira para marcar onde estão as bombas.
- **START:** Atalho para reiniciar a partida a qualquer momento.

## 🚀 Como Jogar

Você não precisa de um Game Boy de verdade!
1. Baixe o arquivo de ROM chamado `campominadinho.gb`.
2. Abra em qualquer emulador de Game Boy (recomendo o **SameBoy**, **BGB** ou até mesmo emuladores de celular/web).
3. Se você for purista, coloque a ROM em um *Flashcart* (como o EverDrive) e rode direto no hardware original de 1989!

## 💻 Como Compilar (Desenvolvedores)

Se quiser mexer no código ou compilar você mesmo, você vai precisar do ambiente de compilação do [GBDK-2020](https://github.com/gbdk-2020/gbdk-2020) instalado no seu sistema. 

Tendo as variáveis de ambiente configuradas, basta rodar o script no terminal:
```bash
./compile.sh
```

---
*Criado com dedicação por Carlos Alexandre Dias.*

# Sistema de Triagem Hospitalar

Projeto desenvolvido para a disciplina de **Algoritmos e Estruturas de Dados** do curso de Engenharia da Computação da **UNIVASF**.

**Autores:** Hiago de Oliveira Lima · Eduardo dos Santos Rodrigues

---

## O que é o projeto

É um projeto que simula um Sistema de triagem hospitalar automatizado com interface gráfica em GTK. O sistema classifica pacientes por urgência usando uma árvore binária de decisão, gerencia a fila de espera por prioridade e registra o histórico de atendimentos em uma pilha.

O projeto foi idealizado justamente pela quantidade de estruturas que podemos usar para fazer ele como, também, a possibilidade de evolução dele, podendo futuramente integrar com sistemas reais.

---

## Estruturas de dados utilizadas

| Estrutura                      | Uso                                             |
| ------------------------------ | ----------------------------------------------- |
| Árvore binária de decisão      | Classificação automática por perguntas clínicas |
| Fila com lista ligada ordenada | Gerenciamento da sala de espera por prioridade  |
| Pilha LIFO                     | Histórico de atendimentos concluídos            |

---

## Como rodar o programa

### Compilar do zero

Requer o **MSYS2** com GTK instalado. Siga os passos abaixo.

#### 1. Instalar o MSYS2

Acesse https://www.msys2.org e instale em `C:\msys64`.

#### 2. Instalar o GTK e o GCC

Abra o terminal **MSYS2 MinGW 64-bit** e rode:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gcc make pkg-config
```

#### 3. Verificar a instalação

```bash
pkg-config --modversion gtk+-3.0
```

Deve aparecer a versão instalada, por exemplo `3.24.38`.

#### 4. Clonar ou extrair o projeto

Coloque o projeto em qualquer pasta acessível pelo MSYS2. Exemplo:

```
C:\triagem_hospitalar\
```

Dentro do MSYS2 acesse:

```bash
cd /c/triagem_hospitalar
```

#### 5. Compilar

```bash
make all
```

#### 6. Rodar

```bash
./triagem.exe
```

---

## Comandos do Makefile

O Makefile automatiza a compilação. Sem ele o comando seria:

```bash
gcc -Wall -Wextra -g -Iinclude $(pkg-config --cflags --libs gtk+-3.0) \
    src/paciente.c src/arvore.c src/fila.c src/pilha.c src/gui.c src/main.c \
    -o triagem
```

Com o Makefile basta:

| Comando      | O que faz                                               |
| ------------ | ------------------------------------------------------- |
| `make all`   | Compila o projeto completo com GTK e gera `triagem.exe` |
| `make test`  | Compila só a lógica sem GTK e gera `test_triagem.exe`   |
| `make clean` | Remove os executáveis gerados                           |

> `make test` é útil para testar as estruturas de dados no terminal sem precisar da interface gráfica.

---

## Estrutura de pastas

```
triagem_hospitalar/
├── include/
│   ├── paciente.h    — struct Paciente e protótipos
│   ├── arvore.h      — struct Nodoarvore e protótipos
│   ├── fila.h        — structs NoFila e Fila e protótipos
│   ├── pilha.h       — structs NoPilha e Pilha e protótipos
│   └── gui.h         — struct AppState e protótipos da interface
├── src/
│   ├── paciente.c    — criação e destruição do paciente
│   ├── arvore.c      — árvore de decisão clínica
│   ├── fila.c        — fila ordenada por prioridade
│   ├── pilha.c       — pilha de histórico
│   ├── gui.c         — interface gráfica GTK
│   ├── main.c        — ponto de entrada do programa
│   └── test.c        — teste em terminal sem GTK
├── univasf_logo.png
├── casaicone.png
└── Makefile
```

---

## Dependências

| Dependência | Versão testada          |
| ----------- | ----------------------- |
| GCC         | 16.1.0 (MinGW-w64)      |
| GTK         | 3.24+                   |
| GNU Make    | 4.4+                    |
| pkg-config  | qualquer versão recente |

---

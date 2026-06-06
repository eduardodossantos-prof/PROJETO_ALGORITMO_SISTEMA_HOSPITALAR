# o arquivo makefile serve para automatizar o processo de compilação do programa, evitando a necessidade de digitar comandos longos no terminal toda vez que quisermos compilar o código
# declaração das variáveis para o compilador, flags de compilação, arquivos fonte e teste
CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude $(shell pkg-config --cflags gtk+-3.0)
LIBS    = $(shell pkg-config --libs gtk+-3.0)
SRC     = src/paciente.c src/arvore.c src/fila.c src/pilha.c src/gui.c
MAIN    = src/main.c
TEST    = src/test.c
 # Makefile é usado para compilar o programa de teste(alvo "test") e para limpar os arquivos compilados(alvo "clean")
 # aqui as variaveis são usadas para compilar o programa de teste, gerando um executável chamado "test_triagem"
 #com o comando "make test" no terminal, e o comando "make clean" para remover o executável gerado, deixando o diretório limpo
 # mas poderia fazer direto por comando puro do terminal, como: "src/paciente.c src/arvore.c src/test.c gcc src/paciente.c src/arvore.c src/test.c -o test_triagem"
 # make all, pra rodar o teste, e make clean, pra limpar os arquivos compilados
all: $(SRC) $(MAIN)
	$(CC) $(CFLAGS) $(SRC) $(MAIN) $(LIBS) -o triagem
 
	# Versão terminal sem GTK para testar a lógica
test: src/paciente.c src/arvore.c src/fila.c src/pilha.c $(TEST)
	$(CC) -Wall -Wextra -g -Iinclude src/paciente.c src/arvore.c src/fila.c src/pilha.c $(TEST) -o test_triagem
clean:# comando puro do terminal para remover o arquivo executável gerado
	rm -f test_triagem
 
# o arquivo makefile serve para automatizar o processo de compilação do programa, evitando a necessidade de digitar comandos longos no terminal toda vez que quisermos compilar o código
# declaração das variáveis para o compilador, flags de compilação, arquivos fonte e teste
CC     	= gcc
CFLAGS  = -Wall -Wextra -g -Iinclude
SRC     = src/paciente.c src/arvore.c
TEST    = src/test.c
 # Makefile para compilar o programa de teste(alvo "test") e para limpar os arquivos compilados(alvo "clean")
 # aqui as variaveis são usadas para compilar o programa de teste, gerando um executável chamado "test_triagem"
 # mas poderia fazer direto por comando puro do terminal, como: "src/paciente.c src/arvore.c src/test.c gcc src/paciente.c src/arvore.c src/test.c -o test_triagem"
 # make test, pra rodar o teste, e make clean, pra limpar os arquivos compilados
test:	$(SRC) $(TEST)	
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o test_triagem
clean:
 # comando puro do terminal para remover o arquivo executável gerado
	rm -f test_triagem
 
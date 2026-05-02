#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paciente.h"
#include "arvore.h"

// Lê 's' ou 'n' do terminal e retorna 1 (sim) ou 0 (nao)
int perguntar(const char *pergunta)
{
    char resp;
    while (1)
    {
        printf("\n  >> %s\n", pergunta);
        printf("     [s] Sim    [n] Nao\n");
        printf("     Resposta: \n");
        scanf(" %c", &resp); // lê a resposta do usuário para a pergunta atual, usando scanf para ler um caractere (a resposta) do terminal e armazená-la no buffer resp
        getchar();           // consome o \n que sobra após scanf
        if (resp == 's' || resp == 'S')
            return 1;
        if (resp == 'n' || resp == 'N')
            return 0;

        printf("     [!] Digite apenas 's' ou 'n'\n");
    }
}
// Simplesmente para criar uma linha de caracteres para separar seções do terminal, melhorando a organização visual
void linha(char c, int n)
{
    for (int i = 0; i < n; i++)
        printf("%c", c);
    printf("\n");
}

void testar_triagem(Nodoarvore *raiz)
{
    char nome[100];
    int idade;

    linha('=', 50);
    printf("  CADASTRO DO PACIENTE\n");
    linha('=', 50);

    printf("  Nome  : ");
    fflush(stdout);
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0'; // remove o \n do fgets

    printf("  Idade : ");
    fflush(stdout);
    scanf("%d", &idade); // aq como é só um número, não tem problema usar scanf
    getchar();           // consome o \n que sobra após scanf

    Paciente *p = criar_paciente(nome, idade);

    linha('-', 50);
    printf("  TRIAGEM - responda s/n para cada pergunta\n");
    linha('-', 50);

    // Navega pela arvore respondendo as perguntas
    Nodoarvore *atual = raiz;
    int passo = 1;               // contador de perguntas para mostrar no terminal
    while (atual->eh_folha == 0) // enquanto não chegar na folha, ou seja, enquanto for um nó de pergunta, continua perguntando e avançando na árvore
    {
        printf("\n  Pergunta %d:", passo++);
        int resposta = perguntar(atual->pergunta); // lê a resposta do usuário para a pergunta atual, usando a função perguntar que retorna 1 para 's' e 0 para 'n'
        atual = avancar_no(atual, resposta);       // avança para o próximo nó da árvore com base na resposta, usando a função avancar_no que retorna o nó filho correspondente à resposta dada
    }

    // Chegou na folha — aplica resultado ao paciente
    aplicar_triagem(atual, p);

    linha('=', 50);
    printf("  RESULTADO DA TRIAGEM\n");
    linha('=', 50);

    const char *cor = "";
    if (p->prioridade == PRIORIDADE_VERMELHA)
        cor = "*** VERMELHO - URGENTE  ***";
    else if (p->prioridade == PRIORIDADE_AMARELA)
        cor = "--- AMARELO  - MODERADO ---";
    else
        cor = "+++ VERDE    - LEVE     +++";

    printf("\n  %s\n", cor);
    printf("\n  Paciente    : %s, %d anos\n", p->nome, p->idade);
    printf("  Prioridade  : %s\n", prioridade_para_string(p->prioridade));
    printf("  Justificativa: %s\n\n", p->justificativa);

    linha('=', 50);

    destruir_paciente(p);
}

int main(void)
{
    linha('=', 50);
    printf("  SISTEMA DE TRIAGEM HOSPITALAR\n");
    printf("  Teste de arvore de decisao (terminal)\n");
    linha('=', 50);

    Nodoarvore *arvore = construir_arvore();

    // 1) Mostra a arvore inteira para debug
    printf("\n  [DEBUG] Estrutura da arvore:\n\n");
    imprimir_arvore(arvore, 2);

    // 2) Loop de triagem — permite testar varios pacientes
    char continuar = 's';
    while (continuar == 's' || continuar == 'S')
    {
        printf("\n");
        testar_triagem(arvore);

        printf("\n  Triar outro paciente? [s/n]: ");
        scanf(" %c", &continuar);
        getchar(); // consome o \n que sobra após scanf
    }

    destruir_arvore(arvore);

    printf("\n  Programa encerrado. Memoria liberada.\n\n");
    return 0;
}

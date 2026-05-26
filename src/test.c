#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paciente.h"
#include "arvore.h"
#include "fila.h"
#include "pilha.h"

// Lê 's' ou 'n' do terminal e retorna 1 (sim) ou 0 (nao)
int perguntar(const char *pergunta)
{
    char resp;
    while (1)
    {
        printf("\n  >> %s\n", pergunta);
        printf("     [s] Sim    [n] Nao\n");
        printf("     Resposta: \n");
        scanf(" %c", &resp); // lê a resposta do usuário para a pergunta atual
        getchar();           // consome o \n que sobra após scanf

        if (resp == 's' || resp == 'S')
            return 1;
        if (resp == 'n' || resp == 'N')
            return 0;

        printf("     [!] Digite apenas 's' ou 'n'\n");
    }
}

// Simplesmente para criar uma linha de caracteres para separar seções do terminal
void linha(char c, int n)
{
    for (int i = 0; i < n; i++)
        printf("%c", c);
    printf("\n");
}

// Tria o paciente pela arvore e insere na fila — NAO destrói aqui
void testar_triagem(Nodoarvore *raiz, Fila *fila)
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
    scanf("%d", &idade); // como é só um número, não tem problema usar scanf
    getchar();           // consome o \n que sobra após scanf

    Paciente *p = criar_paciente(nome, idade);

    linha('-', 50);
    printf("  TRIAGEM - responda s/n para cada pergunta\n");
    linha('-', 50);

    // Navega pela arvore respondendo as perguntas
    Nodoarvore *atual = raiz;
    int passo = 1;               // contador de perguntas para mostrar no terminal
    while (atual->eh_folha == 0) // enquanto for nó de pergunta, continua navegando
    {
        printf("\n  Pergunta %d:", passo++);
        int resposta = perguntar(atual->pergunta); // retorna 1 para 's' e 0 para 'n'
        atual = avancar_no(atual, resposta);       // avança para o filho correspondente
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
    printf("\n  Paciente     : %s, %d anos\n", p->nome, p->idade);
    printf("  Prioridade   : %s\n", prioridade_para_string(p->prioridade));
    printf("  Justificativa: %s\n\n", p->justificativa);

    linha('=', 50);

    // Insere na fila no lugar certo por prioridade
    inserir_fila(fila, p);
    printf("\n  Paciente inserido na fila!\n");
}

// Atende o primeiro da fila e empilha no historico
void atender_proximo(Fila *fila, Pilha *historico)
{
    if (fila_vazia(fila))
    {
        printf("\n  [AVISO] Fila vazia — nenhum paciente para atender\n");
        return;
    }

    // Remove o primeiro da fila — o mais prioritário
    Paciente *atendido = remover_fila(fila);

    printf("\n  Atendendo: %s (%s)\n",
           atendido->nome,
           prioridade_para_string(atendido->prioridade));

    // Empilha no historico
    empilhar(historico, atendido);
    printf("  Paciente registrado no historico!\n");
}

int main(void)
{
    linha('=', 50);
    printf("  SISTEMA DE TRIAGEM HOSPITALAR\n");
    printf("  Teste completo — arvore + fila + pilha\n");
    linha('=', 50);

    // Inicializa todas as estruturas
    Nodoarvore *arvore = construir_arvore();
    Fila *fila = criar_fila();        // sala de espera ordenada por prioridade
    Pilha *historico = criar_pilha(); // histórico de atendimentos — LIFO

    // Debug da arvore
    printf("\n  [DEBUG] Estrutura da arvore:\n\n");
    imprimir_arvore(arvore, 2);

    char opcao = ' ';
    while (opcao != 's' && opcao != 'S')
    {
        printf("\n");
        linha('-', 50);
        printf("  MENU\n");
        linha('-', 50);
        printf("  [1] Cadastrar e triar novo paciente\n");
        printf("  [2] Atender proximo da fila\n");
        printf("  [3] Ver fila atual\n");
        printf("  [4] Ver historico de atendimentos\n");
        printf("  [s] Sair\n");
        printf("  Opcao: ");
        scanf(" %c", &opcao);
        getchar(); // consome o \n que sobra após scanf

        switch (opcao)
        {
        case '1':
            printf("\n");
            testar_triagem(arvore, fila);
            printf("\n");
            imprimir_fila(fila);
            break;

        case '2':
            atender_proximo(fila, historico);
            printf("\n");
            imprimir_fila(fila);
            printf("\n");
            imprimir_pilha(historico);
            break;

        case '3':
            printf("\n");
            imprimir_fila(fila);
            break;

        case '4':
            printf("\n");
            imprimir_pilha(historico);
            break;

        case 's':
        case 'S':
            break;

        default:
            printf("\n  [!] Opcao invalida\n");
            opcao = ' '; // reseta para continuar o loop
            break;
        }
    }

    // Libera toda a memória antes de encerrar
    destruir_arvore(arvore);
    destruir_fila(fila);       // libera pacientes que ainda estão na fila
    destruir_pilha(historico); // libera pacientes do histórico

    printf("\n  Programa encerrado. Memoria liberada.\n\n");
    return 0;
}
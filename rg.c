#include <stdio.h>

int main()
{
    // Definição dos tamanhos considerando a pontuação e o caractere nulo '\0' no final
    char cpf[15];             // Ex: 000.000.000-00 (14 chars + 1)
    char rg[15];              // Ex: 00.000.000-0 (12 chars + 1)
    char data_nascimento[11]; // Ex: 31/12/1999 (10 chars + 1)

    // Leitura do CPF
    printf("Digite o CPF (com ou sem pontos/traço): ");
    scanf("%14s", cpf); // Limita a leitura a 14 caracteres por segurança

    // Leitura do RG
    printf("Digite o RG: ");
    scanf("%14s", rg);

    // Leitura da Data de Nascimento
    printf("Digite a data de nascimento (DD/MM/AAAA): ");
    scanf("%10s", data_nascimento);

    // Exibição dos dados salvos
    printf("\n--- Dados Gravados ---\n");
    printf("CPF: %s\n", cpf);
    printf("RG: %s\n", rg);
    printf("Data: %s\n", data_nascimento);

    return 0;
}

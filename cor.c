#include <stdio.h>

#include <stdlib.h>

void verificaQuadrado(int n, int **matriz);

int main()
{
    int n;
    printf("Digite um valor para n: ");
    scanf("%d", &n);

    int **ma = malloc(n * sizeof(int**));

    //alocando a memoria necessária
    for (int i = 0; i < n; i++)
    {

        ma[i] = malloc(n * sizeof(int));
    }

    //dando valores para a matriz ma
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {

            printf("Digite um valor para matriz[%d][%d]: ", i, j);
            scanf("%d", &ma[i][j]);
        }
    }

    //imprimindo a matriz
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", ma[i][j]);
        }
        printf("\n");
    }

    verificaQuadrado(n, ma);

    //liberando a matriz ma

    for (int i = 0; i < n; i++)
    {
        free(ma[i]);
    }
    free(ma);

    return 0;
}

void verificaQuadrado(int n, int **matriz)
{

    int somaLinhas[100], somaColunas[100], somaDiagPri, somaDiagSec;
    int contLI, contCO;

    for (int i = 0; i < n; i++)
    {   somaLinhas[i] = 0;
        for (int j = 0; j < n; j++)
        {

            somaLinhas[i] += matriz[i][j];
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {

            if (somaLinhas[i] == somaLinhas[i + 1])
            {
                contLI++;
            }
        }
    }

    for (int i = 0; i < n; i++)
    {   
        somaColunas[i] = 0;
        for (int j = 0; j < n; j++)
        {

            somaColunas[i] += matriz[i][j];
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {

            if (somaColunas[i] == somaColunas[i + 1])
            {
                contCO++;
            }
        }
    }
    somaDiagPri = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {

            if (i == j)
            {
                somaDiagPri += matriz[i][j];
            }
        }
    }

    somaDiagSec = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {

            if (i == (n - 1 - j))
                somaDiagSec += matriz[i][j];
        }
    }

    for (int i = 0; i < n; i++)
    {
        if (((contLI == n - 1) && (contCO == n - 1)) && ((somaDiagSec == somaDiagPri)))
        {
            printf("\nEsta matriz eh um quadrado mágico!");
        }
        else
        {
            printf("\nEsta matriz não eh um quadrado mágico!");
        }
    }
}
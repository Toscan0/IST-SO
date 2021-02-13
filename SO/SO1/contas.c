#include "contas.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define atrasar() sleep(ATRASO)
		     
int contasSaldos[NUM_CONTAS];


int contaExiste(int idConta) {
  return (idConta > 0 && idConta <= NUM_CONTAS);
}

void inicializarContas() {
  int i;
  for (i=0; i<NUM_CONTAS; i++)
    contasSaldos[i] = 0;
}

int debitar(int idConta, int valor) {
  atrasar();
  if (!contaExiste(idConta))
    return -1;
  if (contasSaldos[idConta - 1] < valor)
    return -1;
  atrasar();
  contasSaldos[idConta - 1] -= valor;
  return 0;
}

int creditar(int idConta, int valor) {
  atrasar();
  if (!contaExiste(idConta))
    return -1;
  contasSaldos[idConta - 1] += valor;
  return 0;
}

int lerSaldo(int idConta) {
  atrasar();
  if (!contaExiste(idConta))
    return -1;
  return contasSaldos[idConta - 1];
}


void simular(int numAnos) {
  int i, j;
  for (i = 0; i < numAnos+1; i++){

    printf("SIMULACAO: Ano %d\n=================\n", i);

    for (j = 1; j <= NUM_CONTAS ; j++){
      printf("conta %d, Saldo %d\n", j, lerSaldo(j));
    }

    for (j = 0; j < NUM_CONTAS; j++){
      contasSaldos[j] = max(contasSaldos[j] * (1 + TAXAJURO) - CUSTOMANUTENCAO,0);
    }
    printf("\n");
  }
}

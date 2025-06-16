#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {E0, E1, E2, NESTADOS} Estados;
typedef enum {A, B, C, FDC, NENTRADAS} Entradas; 
typedef enum {PESOS, E_R , NESTADOSPILA} EstadosPila; //PESOS = $
typedef enum {E0PESOS, E0R , E1R, E2R, E2PESOS, NMOVIMIENTOS} MOVIMIENTOS;
typedef enum {R, R$, RR, e} ACCIONES; 

//Pila
typedef struct Nodo {
  EstadosPila dato;
  struct Nodo* sgt;
} Nodo;

typedef struct{
  Estados estado;
  ACCIONES accion;
} Movimiento;

//Funciones de Pila
Nodo* crearNodo(EstadosPila);
void push(Nodo**, EstadosPila);
EstadosPila pop(Nodo**);

//Funciones

bool verificarADP (char[]);
MOVIMIENTOS identificarMovimiento (Estados, EstadosPila);
Entradas identificarEntrada(char);


#endif
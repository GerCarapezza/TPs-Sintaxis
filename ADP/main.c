#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <main.h>


bool debug = true;



typedef enum {E0, E1, E2, NESTADOS} Estados;
typedef enum {A, C, B, FDC, NENTRADAS} Entradas; 
typedef enum {PESOS, E_R , NESTADOSPILA} EstadosPila; //PESOS = $
typedef enum {E0PESOS, E0R , E1R, E2R, E2PESOS, NMOVIMIENTOS} MOVIMIENTOS;
typedef enum {$, R, R$, RR, e} ACCIONES; 

char* alfabeto[] = {"a", "b", "c"};

//Pila
typedef struct {
  EstadosPila dato;
  Nodo* sgt;
}Nodo;

Nodo* crearNodo(EstadosPila valor){
  Nodo* nuevoNodo = malloc(sizeof(Nodo));

  nuevoNodo->dato = valor;
  nuevoNodo->sgt = nuevoNodo;

  return nuevoNodo;
}

void push(Nodo* pila, EstadosPila valor){
  Nodo* nuevoNodo = crearNodo(valor);

  nuevoNodo->sgt = pila;
  pila = nuevoNodo; 
}

EstadosPila pop(Nodo* pila){
  if(pila == NULL) return -1;

  EstadosPila valor = pila->dato;
  Nodo* temp = pila;
  pila = pila->sgt;
  
  free(temp);

  return valor;
}

typedef struct{
  Entradas entrada;
  ACCIONES estado;
} Movimiento;


Movimiento tablaDeMovimientos[5][4] = { 
                                    {{E0, R$},  {-1, -1},  {-1, -1}, {-1, -1}},
                                    {{E0, RR},  {-1, -1},  {E1, R},  {-1, -1}},
                                    {{-1, -1},  {E2, e},   {-1, -1}, {-1, -1}},
                                    {{-1, -1},  {E2, e},   {-1, -1}, {-1, -1}},
                                    {{-1, -1},  {-1, -1},  {-1, -1}, {E2, e}}};

Entradas identificarEntrada(char* letra){
  if(strcmp(alfabeto[0], letra) == 0) return A;
  else if(strcmp(alfabeto[1], letra) == 0) return B;
  else if(strcmp(alfabeto[2], letra) == 0) return C;
  return -1;
}

bool verificarADP (char palabra[]){

  int longitud = strlen(palabra);
  
  Estados estadoActual = E0;

  //Pila


  for (int i = 0; i < longitud; i++){
    Entradas entrada = identificarEntrada(&palabra[i]);
    

  }
}

int main(){

  char palabra[50];
  printf("Ingrese una palabra a verificar... ");
  scanf("%s", palabra);
  if (verificarADP(palabra))
    printf("La palabra ", palabra, " es valida.");
  else
    printf("La palabra ", palabra, " NO es reconocida por el lenguaje.");

    
  return 0;
}








// int main() {

  // MoviemitablaMovimeintos[]
  
  // //Creamos tabla de movimiento
  // for (int j = 0; j < NMOVIMIENTOS; j++) //Filas e0, $; e0, R; ...
  // {
  //   for (int i = 0; i < NENTRADAS; i++) //Columnas - entradas (a, b, c, fdc)
  //   {
  //     switch (i)
  //     {
  //     case A:
  //         if (j == E0PESOS)
          
  //       break;
      
  //     case B:
  //       /* code */
  //       break;
  //     case C:
  //       /* code */
  //       break;
  //     case FDC: 
  //       //Code
  //       break;
  //     }
  //   }
    
  // }
  



//   return 0;
// }






// typedef enum { S0 = 0, S1, S2, NSTATES } State;

// /* ---- entrada: a c b + fin‑de‑cadena ---- */
// typedef enum { A = 0, C, B, EOS, NINPUTS } Input;

// /* ---- pila: fondo $  o  R --------------- */
// typedef enum { BOT = 0, R, NSTACK } StackSym;

// #define STACK_MAX 256

// typedef struct { State next; char push[3]; bool defined; } Delta;
// typedef struct { char v[STACK_MAX]; int top; } Stack;

// /* tabla δ(q, a, X) */
// Delta delta[NSTATES][NINPUTS][NSTACK] = { {{0}} };

// void st_init(Stack *s) { s->v[0] = '$'; s->top = 0; }
// char st_top (const Stack *s) { return s->v[s->top]; }
// void st_pop (Stack *s) { if (s->top > 0) --s->top; }
// void st_push(Stack *s, char c) { if (s->top < STACK_MAX-1) s->v[++s->top] = c; }

// /* traduce char → Input; fuera de {a,c,b} rechaza directo */
// Input sym(char ch)
// {
//     switch (ch) {
//         case 'a': return A;
//         case 'c': return C;
//         case 'b': return B;
//         case '\0':return EOS;
//         default : return NINPUTS;          /* marca de {-1, -1} */
//     }
// }

// /* hardcodeo de la tabla que tenés en la foto */
// void init_delta(void)
// {
//     /* estado 0 */
//     delta[S0][A][BOT] = (Delta){ S0, "R$", true };  /* a,$ → R$ */
//     delta[S0][A][R  ] = (Delta){ S0, "RR", true };  /* a,R → RR */
//     delta[S0][C][R  ] = (Delta){ S1, "R",  true };  /* c,R → R  */

//     /* estado 1 */
//     delta[S1][B][R  ] = (Delta){ S2, "",   true };  /* b,R → ε  */

//     /* estado 2 */
//     delta[S2][B][R  ]   = (Delta){ S2, "",   true };/* b,R → ε  */
//     delta[S2][EOS][BOT] = (Delta){ S2, "",   true };/* ⊣,$→ ε   */
// }

// bool pda_accepts(const char *w)
// {
//     State q = S0;
//     Stack st; st_init(&st);
//     size_t i = 0;

//     while (true) {
//         Input a = sym(w[i]);
//         if (a >= NINPUTS) return false;          /* char ilegítimo */

//         StackSym X = (st_top(&st) == '$') ? BOT : R;
//         Delta d = delta[q][a][X];
//         if (!d.defined) return false;            /* δ no definida  */

//         st_pop(&st);
//         for (int k=(int)strlen(d.push)-1; k>=0; --k) st_push(&st,d.push[k]);

//         q = d.next;
//         if (a != EOS) ++i;                       /* avanzar cinta */

//         if (a == EOS) break;
//     }
//     return (q == S2 && st_top(&st) == '$');
// }

// int main(int argc, char *argv[])
// {
//     if (argc != 2) { fprintf(stderr,"Uso: %s <cadena>\n",argv[0]); return 1; }

//     init_delta();
//     printf("Resultado: \"%s\" %s el lenguaje.\n",
//            argv[1], pda_accepts(argv[1]) ? "PERTENECE a" : "NO pertenece a");
//     return 0;
// }

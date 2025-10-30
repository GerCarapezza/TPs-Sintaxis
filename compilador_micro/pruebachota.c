// Aproximacion a C
/* Compilador del Lenguaje Micro (Fischer) - Extendido con Soporte Float */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TAMLEX 32 + 1
#define TAMNOM 20 + 1

/******************Declaraciones Globales*************************/
FILE *in;

typedef enum
{
    INICIO,
    FIN,
    LEER,
    ESCRIBIR,
    ID,
    CONSTANTE,
    PARENIZQUIERDO,
    PARENDERECHO,
    PUNTOYCOMA,
    COMA,
    ASIGNACION, // :=
    SUMA,
    RESTA,
    FDT,
    ERRORLEXICO,
    /* Palabras clave de control estructurado */
    SI,
    ENTONCES,
    SINO,
    MIENTRAS,
    HACER,
    REPETIR,
    HASTA
} TOKEN;

typedef enum { 
    TIPO_ENTERO, 
    TIPO_REAL, 
    TIPO_CHAR 
} TIPO_DATO;

typedef struct
{
    char identifi[TAMLEX];
    TOKEN t;
    TIPO_DATO tipoDato;  // Agregado para guardar el tipo
} RegTS;

/* Tabla de símbolos con palabras reservadas */
RegTS TS[1000] = {
    {"inicio", INICIO, TIPO_ENTERO},
    {"fin", FIN, TIPO_ENTERO},
    {"leer", LEER, TIPO_ENTERO},
    {"escribir", ESCRIBIR, TIPO_ENTERO},
    {"si", SI, TIPO_ENTERO},
    {"entonces", ENTONCES, TIPO_ENTERO},
    {"sino", SINO, TIPO_ENTERO},
    {"mientras", MIENTRAS, TIPO_ENTERO},
    {"hacer", HACER, TIPO_ENTERO},
    {"repetir", REPETIR, TIPO_ENTERO},
    {"hasta", HASTA, TIPO_ENTERO},
    {"$", 99, TIPO_ENTERO}
};

/* Registro de expresión con soporte para múltiples tipos */
typedef struct
{
    TOKEN clase;
    char nombre[TAMLEX];
    union {
        int valorEntero;
        float valorReal;
        char valorChar;
    } valor;
    TIPO_DATO tipo;
} REG_EXPRESION;

char buffer[TAMLEX];
TOKEN tokenActual;
int flagToken = 0;

/*************************Prototipos de Funciones*********************/
TOKEN scanner();
int columna(int c);
int estadoFinal(int e);
void Objetivo(void);
void Programa(void);
void ListaSentencias(void);
void Sentencia(void);
void ListaIdentificadores(void);
void Identificador(REG_EXPRESION *presul);
void ListaExpresiones(void);
void Expresion(REG_EXPRESION *presul);
void Primaria(REG_EXPRESION *presul);
void OperadorAditivo(char *presul);

REG_EXPRESION ProcesarCte(void);
REG_EXPRESION ProcesarId(void);
char *ProcesarOp(void);
void Leer(REG_EXPRESION in);
void Escribir(REG_EXPRESION out);
REG_EXPRESION GenInfijo(REG_EXPRESION e1, char *op, REG_EXPRESION e2);

void Match(TOKEN t);
TOKEN ProximoToken();
void ErrorLexico();
void ErrorSintactico();
void Generar(char *co, char *a, char *b, char *c);
char *Extraer(REG_EXPRESION *preg);
int Buscar(char *id, RegTS *TS, TOKEN *t);
void Colocar(char *id, RegTS *TS, TIPO_DATO tipo);
void Chequear(char *s, char *tipo);
TIPO_DATO ObtenerTipo(char *id);
char *TipoAString(TIPO_DATO tipo);
void Comenzar(void);
void Terminar(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);

/* Prototipos para sentencias estructuradas */
void SentenciaSi(void);
void SentenciaMientras(void);
void SentenciaRepetir(void);
void Condicion(REG_EXPRESION *presul);
void NuevaEtiqueta(char *out);

/**************************Scanner************************************/
#define NUMESTADOS 20
#define NUMCOLS 15

TOKEN scanner()
{
    // Tabla de transiciones del AFD
    // Columnas: letra, dígito, +, -, (, ), ,, ;, :, =, EOF, espacio, ., ', otro
    int tabla[NUMESTADOS][NUMCOLS] = {
        {1,  3,  5, 6, 7, 8, 9, 10, 11, 14, 13, 0, 15, 17, 14}, // 0: estado inicial
        {1,  1,  2, 2, 2, 2, 2,  2,  2,  2,  2, 2,  2,  2,  2}, // 1: identificador
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 2: ID final
        {4,  3,  4, 4, 4, 4, 4,  4,  4,  4,  4, 4, 15,  4,  4}, // 3: constante entera
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 4: CONST final
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 5: SUMA
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 6: RESTA
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 7: PAREN_IZQ
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 8: PAREN_DER
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 9: COMA
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 10: PUNTOYCOMA
        {14, 14, 14,14,14,14,14, 14, 14, 12, 14,14, 14, 14, 14}, // 11: DOS_PUNTOS
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 12: ASIGNACION
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 13: FDT
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 14: ERROR
        {14, 16, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}, // 15: PUNTO
        {4,  16,  4, 4, 4, 4, 4,  4,  4,  4,  4, 4,  4,  4,  4}, // 16: REAL
        {18, 18, 18,18,18,18,18, 18, 18, 18, 18,18, 18, 19, 18}, // 17: comilla inicial
        {18, 18, 18,18,18,18,18, 18, 18, 18, 18,18, 18, 19, 18}, // 18: contenido char
        {14, 14, 14,14,14,14,14, 14, 14, 14, 14,14, 14, 14, 14}  // 19: CHAR final
    };
    
    int car;
    int col;
    int estado = 0;
    int i = 0;
    
    do
    {
        car = fgetc(in);
        col = columna(car);
        estado = tabla[estado][col];
        if (col != 11) // No guardar espacios
        {
            buffer[i] = car;
            i++;
        }
    } while (!estadoFinal(estado) && estado != 14);
    
    buffer[i] = '\0';
    
    switch (estado)
    {
    case 2: // Identificador
        if (col != 11)
        {
            ungetc(car, in);
            buffer[i - 1] = '\0';
        }
        return ID;
        
    case 4: // Constante (entera o real finalizada)
        if (col != 11)
        {
            ungetc(car, in);
            buffer[i - 1] = '\0';
        }
        return CONSTANTE;
        
    case 16: // Estado 16 ya no es final - debe ir a estado 4
        if (col != 11)
        {
            ungetc(car, in);
            buffer[i - 1] = '\0';
        }
        return CONSTANTE;
        
    case 19: // Constante carácter
        return CONSTANTE;
        
    case 5:
        return SUMA;
    case 6:
        return RESTA;
    case 7:
        return PARENIZQUIERDO;
    case 8:
        return PARENDERECHO;
    case 9:
        return COMA;
    case 10:
        return PUNTOYCOMA;
    case 12:
        return ASIGNACION;
    case 13:
        return FDT;
    case 14:
        return ERRORLEXICO;
    }
    return ERRORLEXICO;
}

int estadoFinal(int e)
{
    if (e == 0 || e == 1 || e == 3 || e == 11 || e == 15 || e == 16 || e == 17 || e == 18)
        return 0;
    return 1;
}

int columna(int c)
{
    if (isalpha(c)) return 0;
    if (isdigit(c)) return 1;
    if (c == '+')   return 2;
    if (c == '-')   return 3;
    if (c == '(')   return 4;
    if (c == ')')   return 5;
    if (c == ',')   return 6;
    if (c == ';')   return 7;
    if (c == ':')   return 8;
    if (c == '=')   return 9;
    if (c == EOF)   return 10;
    if (isspace(c)) return 11;
    if (c == '.')   return 12;
    if (c == '\'')  return 13;
    return 14;
}

/*************Fin Scanner**********************************************/

/**********Procedimientos de Análisis Sintáctico (PAS) *****************/

void Objetivo(void)
{
    Programa();
    Match(FDT);
    Terminar();
}

void Programa(void)
{
    Comenzar();
    Match(INICIO);
    ListaSentencias();
    Match(FIN);
}

void ListaSentencias(void)
{
    Sentencia();
    while (1)
    {
        switch (ProximoToken())
        {
        case ID:
        case LEER:
        case ESCRIBIR:
        case SI:
        case MIENTRAS:
        case REPETIR:
            Sentencia();
            break;
        default:
            return;
        }
    }
}

void Sentencia(void)
{
    TOKEN tok = ProximoToken();
    REG_EXPRESION izq, der;
    
    switch (tok)
    {
    case ID:
        Identificador(&izq);
        Match(ASIGNACION);
        Expresion(&der);
        Asignar(izq, der);
        Match(PUNTOYCOMA);
        break;
        
    case LEER:
        Match(LEER);
        Match(PARENIZQUIERDO);
        ListaIdentificadores();
        Match(PARENDERECHO);
        Match(PUNTOYCOMA);
        break;
        
    case ESCRIBIR:
        Match(ESCRIBIR);
        Match(PARENIZQUIERDO);
        ListaExpresiones();
        Match(PARENDERECHO);
        Match(PUNTOYCOMA);
        break;
        
    case SI:
        SentenciaSi();
        break;
        
    case MIENTRAS:
        SentenciaMientras();
        break;
        
    case REPETIR:
        SentenciaRepetir();
        break;
        
    default:
        return;
    }
}

void ListaIdentificadores(void)
{
    TOKEN t;
    REG_EXPRESION reg;
    Identificador(&reg);
    Leer(reg);
    for (t = ProximoToken(); t == COMA; t = ProximoToken())
    {
        Match(COMA);
        Identificador(&reg);
        Leer(reg);
    }
}

void Identificador(REG_EXPRESION *presul)
{
    Match(ID);
    *presul = ProcesarId();
}

void ListaExpresiones(void)
{
    TOKEN t;
    REG_EXPRESION reg;
    Expresion(&reg);
    Escribir(reg);
    for (t = ProximoToken(); t == COMA; t = ProximoToken())
    {
        Match(COMA);
        Expresion(&reg);
        Escribir(reg);
    }
}

void Expresion(REG_EXPRESION *presul)
{
    REG_EXPRESION operandoIzq, operandoDer;
    char op[TAMLEX];
    TOKEN t;
    
    Primaria(&operandoIzq);
    for (t = ProximoToken(); t == SUMA || t == RESTA; t = ProximoToken())
    {
        OperadorAditivo(op);
        Primaria(&operandoDer);
        operandoIzq = GenInfijo(operandoIzq, op, operandoDer);
    }
    *presul = operandoIzq;
}

void Primaria(REG_EXPRESION *presul)
{
    TOKEN tok = ProximoToken();
    switch (tok)
    {
    case ID:
        Identificador(presul);
        break;
    case CONSTANTE:
        Match(CONSTANTE);
        *presul = ProcesarCte();
        break;
    case PARENIZQUIERDO:
        Match(PARENIZQUIERDO);
        Expresion(presul);
        Match(PARENDERECHO);
        break;
    default:
        return;
    }
}

void OperadorAditivo(char *presul)
{
    TOKEN t = ProximoToken();
    if (t == SUMA || t == RESTA)
    {
        Match(t);
        strcpy(presul, ProcesarOp());
    }
    else
        ErrorSintactico();
}

/************* Sentencias Estructuradas ****************/

void Condicion(REG_EXPRESION *presul)
{
    Expresion(presul);
}

static unsigned int numEtiqueta = 1;

void NuevaEtiqueta(char *out)
{
    char cadNum[TAMLEX];
    strcpy(out, "L&");
    sprintf(cadNum, "%u", numEtiqueta++);
    strcat(out, cadNum);
}

void SentenciaSi(void)
{
    REG_EXPRESION c;
    
    Match(SI);
    Condicion(&c);
    
    Match(ENTONCES);
    ListaSentencias();
    
    if (ProximoToken() == SINO)
    {
        Match(SINO);
        ListaSentencias();
    }
    
    Match(FIN);
}

void SentenciaMientras(void)
{
    REG_EXPRESION c;
    
    Match(MIENTRAS);
    Condicion(&c);
    
    Match(HACER);
    ListaSentencias();
    
    Match(FIN);
}

void SentenciaRepetir(void)
{
    REG_EXPRESION c;
    
    Match(REPETIR);
    ListaSentencias();
    
    Match(HASTA);
    Condicion(&c);
    
    Match(PUNTOYCOMA);
}

/**********************Rutinas Semánticas******************************/

char *TipoAString(TIPO_DATO tipo)
{
    switch(tipo)
    {
        case TIPO_ENTERO: return "Entera";
        case TIPO_REAL: return "Real";
        case TIPO_CHAR: return "Caracter";
        default: return "Entera";
    }
}

TIPO_DATO ObtenerTipo(char *id)
{
    int i = 0;
    while (strcmp("$", TS[i].identifi))
    {
        if (!strcmp(id, TS[i].identifi))
        {
            return TS[i].tipoDato;
        }
        i++;
    }
    return TIPO_ENTERO; // Por defecto
}

REG_EXPRESION ProcesarCte(void)
{
    REG_EXPRESION reg;
    reg.clase = CONSTANTE;
    
    if (strlen(buffer) >= TAMLEX)
    {
        printf("Error: Constante demasiado larga\n");
        buffer[TAMLEX-1] = '\0';
    }
    
    strcpy(reg.nombre, buffer);
    
    // Carácter entre comillas
    if (buffer[0] == '\'')
    {
        reg.tipo = TIPO_CHAR;
        if (strlen(buffer) >= 3)
            reg.valor.valorChar = buffer[1];
        else
        {
            printf("Error: Constante de carácter mal formada\n");
            reg.valor.valorChar = '\0';
        }
        return reg;
    }
    
    // Real (tiene punto decimal)
    if (strchr(buffer, '.') != NULL)
    {
        reg.tipo = TIPO_REAL;
        if (sscanf(buffer, "%f", &reg.valor.valorReal) != 1)
        {
            printf("Error: No se pudo convertir '%s' a float\n", buffer);
            reg.valor.valorReal = 0.0f;
        }
        return reg;
    }
    
    // Entero por defecto
    reg.tipo = TIPO_ENTERO;
    if (sscanf(buffer, "%d", &reg.valor.valorEntero) != 1)
    {
        printf("Error: No se pudo convertir '%s' a entero\n", buffer);
        reg.valor.valorEntero = 0;
    }
    return reg;
}

REG_EXPRESION ProcesarId(void)
{
    REG_EXPRESION reg;
    reg.clase = ID;
    strcpy(reg.nombre, buffer);
    // Obtener tipo desde la tabla de símbolos si ya existe
    reg.tipo = ObtenerTipo(buffer);
    return reg;
}

char *ProcesarOp(void)
{
    return buffer;
}

void Leer(REG_EXPRESION in)
{
    Chequear(in.nombre, "Entera");
    Generar("Read", in.nombre, "Entera", "");
}

void Escribir(REG_EXPRESION out)
{
    char *tipo;
    
    // Si es ID, obtener tipo de la tabla de símbolos
    if (out.clase == ID)
    {
        tipo = TipoAString(ObtenerTipo(out.nombre));
    }
    else
    {
        // Si es constante, usar su tipo
        tipo = TipoAString(out.tipo);
    }
    
    Generar("Write", Extraer(&out), tipo, "");
}

REG_EXPRESION GenInfijo(REG_EXPRESION e1, char *op, REG_EXPRESION e2)
{
    REG_EXPRESION reg;
    static unsigned int numTemp = 1;
    char cadTemp[TAMLEX] = "Temp&";
    char cadNum[TAMLEX];
    char cadOp[TAMLEX];
    TIPO_DATO tipoResultado;
    
    if (op[0] == '-')
        strcpy(cadOp, "Restar");
    if (op[0] == '+')
        strcpy(cadOp, "Sumar");
    
    sprintf(cadNum, "%d", numTemp);
    numTemp++;
    strcat(cadTemp, cadNum);
    
    // Obtener tipos reales si son IDs
    TIPO_DATO tipo1 = (e1.clase == ID) ? ObtenerTipo(e1.nombre) : e1.tipo;
    TIPO_DATO tipo2 = (e2.clase == ID) ? ObtenerTipo(e2.nombre) : e2.tipo;
    
    // Propagación de tipo
    if (tipo1 == TIPO_REAL || tipo2 == TIPO_REAL)
    {
        tipoResultado = TIPO_REAL;
    }
    else if (tipo1 == TIPO_CHAR || tipo2 == TIPO_CHAR)
    {
        tipoResultado = TIPO_CHAR;
    }
    else
    {
        tipoResultado = TIPO_ENTERO;
    }
    
    reg.tipo = tipoResultado;
    Chequear(cadTemp, TipoAString(tipoResultado));
    Generar(cadOp, Extraer(&e1), Extraer(&e2), cadTemp);
    strcpy(reg.nombre, cadTemp);
    
    return reg;
}

/***************Funciones Auxiliares**********************************/

void Match(TOKEN t)
{
    if (t != ProximoToken())
        ErrorSintactico();
    flagToken = 0;
}

TOKEN ProximoToken()
{
    if (!flagToken)
    {
        tokenActual = scanner();
        if (tokenActual == ERRORLEXICO)
            ErrorLexico();
        flagToken = 1;
        if (tokenActual == ID)
        {
            Buscar(buffer, TS, &tokenActual);
        }
    }
    return tokenActual;
}

void ErrorLexico()
{
    printf("Error Lexico\n");
}

void ErrorSintactico()
{
    printf("Error Sintactico\n");
}

void Generar(char *co, char *a, char *b, char *c)
{
    printf("%s %s%c%s%c%s\n", co, a, ',', b, ',', c);
}

char *Extraer(REG_EXPRESION *preg)
{
    return preg->nombre;
}

int Buscar(char *id, RegTS *TS, TOKEN *t)
{
    int i = 0;
    while (strcmp("$", TS[i].identifi))
    {
        if (!strcmp(id, TS[i].identifi))
        {
            *t = TS[i].t;
            return 1;
        }
        i++;
    }
    return 0;
}

void Colocar(char *id, RegTS *TS, TIPO_DATO tipo)
{
    int i = 11; // Después de las palabras reservadas
    while (strcmp("$", TS[i].identifi))
        i++;
    if (i < 999)
    {
        strcpy(TS[i].identifi, id);
        TS[i].t = ID;
        TS[i].tipoDato = tipo;  // Guardar el tipo
        strcpy(TS[++i].identifi, "$");
    }
}

void Chequear(char *s, char *tipo)
{
    TOKEN t;
    TIPO_DATO tipoDato;
    
    // Convertir string a TIPO_DATO
    if (strcmp(tipo, "Real") == 0)
        tipoDato = TIPO_REAL;
    else if (strcmp(tipo, "Caracter") == 0)
        tipoDato = TIPO_CHAR;
    else
        tipoDato = TIPO_ENTERO;
    
    if (!Buscar(s, TS, &t))
    {
        Colocar(s, TS, tipoDato);
        Generar("Declara", s, tipo, "");
    }
}

void Comenzar(void)
{
    /* Inicializaciones Semánticas */
}

void Terminar(void)
{
    Generar("Detiene", "", "", "");
}

void Asignar(REG_EXPRESION izq, REG_EXPRESION der)
{
    char *tipo = TipoAString(der.tipo);
    
    Chequear(izq.nombre, tipo);
    Generar("Almacena", Extraer(&der), izq.nombre, "");
}

/************************* Main ***************************************/

int main(int argc, char *argv[])
{
    char nombreArchivo[100];
    
    if (argc > 1)
    {
        strcpy(nombreArchivo, argv[1]);
    }
    else
    {
        printf("Ingrese el nombre del archivo fuente: ");
        scanf("%s", nombreArchivo);
    }
    
    in = fopen(nombreArchivo, "r");
    if (in == NULL)
    {
        printf("Error: No se pudo abrir el archivo %s\n", nombreArchivo);
        return 1;
    }
    
    Objetivo();
    
    fclose(in);
    
    return 0;
}
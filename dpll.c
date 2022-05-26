#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dpll.h"

static clause empty_clause();
static void free_clause(clause c);
static void print_clause(clause c);
static void print_litteral(litteral l);
static char* copy_str(char* str);
static clause add_litteral(clause c, char name, short isnot);
static int contains_letters(char* str);
static void free_array_str(array_str arr);
static array_str get_words(char* str, char* delim);

/*

    CHAQUE CLAUSE A UNE VALUE.
    SI TOUTE LES CLAUSES VALENT 1, ALORS ELLES ONT
    TOUTES DISPARUES. A = NULL et DONC REGLE (1) DE
    DPLL EST RESPECTE. A EST SATISFAISABLE.

    SI TOUTES LES CLAUSES VALENT 1 SAUF UNE QUI VAUT
    0, ALORS NULL APPARTIENT A A, ET DONC A N'EST PAS
    STATISFAISABLE D APRES LA REGLE (2) DE DPLL.

    DANS TOUS LES AUTRES CAS, AU MOINS UNE DES CLAUSES
    DE LA CNF ONT LA VALEUR -1. SOIT LA VALEUR PAR DEFAUT.

*/

CNF* empty_CNF() {
    CNF* cnf = malloc(sizeof(CNF));
    if(cnf == NULL) {
        perror("ERROR malloc");
        exit(1);
    }
    cnf->capacity = 5;
    cnf->size = 0;
    cnf->clauses = malloc(sizeof(clause) * cnf->capacity);
    if(cnf->clauses == NULL) {
        perror("ERROR malloc");
        exit(1);
    }
    return cnf;
}

CNF* string_to_CNF(char* str) {
    if(str == NULL)
        return NULL;
    int size = strlen(str);
    if(size < 2 || str[0] != '{' || str[size-1] != '}')
        return NULL;
    CNF* cnf = empty_CNF();
    array_str arr = get_words(str, "{}");
    char* word;
    for(size_t i=0;i<arr.len;i++) {
        word = arr.val[i];
        if(contains_letters(word))
            add_clause(cnf, word);
    }

    free_array_str(arr);

    return cnf;
}

static void free_array_str(array_str arr) {
    for(size_t i=0;i<arr.len;i++)
        free(arr.val[i]);
    free(arr.val);
}

static array_str get_words(char* str, char* delim) {
    array_str w = {.val=NULL, .len=0};
    if(str == NULL || delim == NULL)
        return w;
    size_t cap = 5;
    w.val = malloc(sizeof(char*) * cap);
    if(w.val == NULL) {
        perror("ERROR malloc\n");
        exit(1);
    }
    char* cpy = copy_str(str);
    char* word = strtok(cpy, delim);
    while(word != NULL) {
        if(w.len == cap) {
            char** ptr = realloc(w.val, sizeof(char*) * cap * 2);
            if(ptr == NULL) {
                perror("ERROR realloc\n");
                free(cpy);
                free(w.val);
                exit(1);
            }
            w.val = ptr;
            cap *= 2;
        }

        w.val[w.len] = copy_str(word);
        w.len++;
        word = strtok(NULL, delim);
    }

    free(cpy);

    return w;
}
static int contains_letters(char* str) {
    if(str == NULL)
        return 0;
    char* c = str;
    for(;!isalpha(*c) && *c != '\0';c++)
        ;
    return *c != '\0';
}

static clause empty_clause() {
    clause clause = {.capacity=5, .size=0};
    clause.litts = malloc(sizeof(litteral) * clause.capacity);
    if(clause.litts == NULL) {
        perror("ERROR malloc");
        exit(1);
    }
    return clause;
}

void free_CNF(CNF* cnf) {
    if(cnf == NULL)
        return;
    for(int i=0;i<cnf->size;i++)
        free_clause(cnf->clauses[i]);
    free(cnf->clauses);
    free(cnf);
}

static void free_clause(clause c) {
    free(c.litts);
}

void print_CNF(CNF* cnf) {
    if(cnf == NULL) {
        fprintf(stderr, "CNF is NULL !\n");
        return;
    }
    printf("{ ");
    for(int i=0;i<cnf->size;i++) {
        print_clause(cnf->clauses[i]);
        if(i != cnf->size-1)
            printf(", ");
    }
    printf(" }\n");
}

static void print_clause(clause c) {
    printf("{ ");
    if(c.size == 0)
        printf("empty");
    for(int i=0;i<c.size;i++) {
        print_litteral(c.litts[i]);
        if(i != c.size-1)
            printf(", ");
    }
    printf(" }");
}

static void print_litteral(litteral l) {
    if(l.isnot)
        printf("¬");
    printf("%c", l.name);
}

int add_clause(CNF* cnf, const char* exp) {
    if(cnf == NULL || exp == NULL)
        return 1;
    clause c = empty_clause();
    if(cnf->size == cnf->capacity) {
        clause* ptr = realloc(cnf->clauses, cnf->capacity * 2 * sizeof(clause));
        if(ptr == NULL) {
            perror("ERROR add clause malloc");
            exit(1);
        }
        cnf->clauses = ptr;
        cnf->capacity *= 2;
    }

    array_str arr = get_words((char*)exp, "{, }");

    char* word;
    char name;
    short isnot;
    size_t len;
    for(size_t i=0;i<arr.len;i++) {
        word = arr.val[i];
        len = strlen(word);
        if(len < 1)
            continue;
        isnot = !isalpha(word[0]);
        name = isnot ? word[1] : word[0];
        if(!isalpha(name))
            continue;
        c = add_litteral(c, name, isnot);
    }

    cnf->clauses[cnf->size] = c;
    cnf->size++;

    free_array_str(arr);
    return 0;
}

static char* copy_str(char* str) {
    if(str == NULL)
        return str;
    char* cpy = malloc(strlen(str) + 1);
    if(cpy == NULL) {
        perror("ERROR malloc\n");
        exit(1);
    }
    strcpy(cpy, str);
    return cpy;
}

static clause add_litteral(clause c, char name, short isnot) {
    if(c.litts == NULL)
        return c;
    if(c.size == c.capacity) {
        litteral* ptr = realloc(c.litts, c.capacity * 2 * sizeof(clause));
        if(ptr == NULL) {
            perror("ERROR add literral malloc");
            exit(1);
        }
        c.litts = ptr;
        c.capacity *= 2;
    }

    c.litts[c.size] = (litteral){.name=name, .isnot=isnot};
    c.size++;

    return c;
}

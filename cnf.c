#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cnf.h"

static char next_name = 'A';
static char get_next_name();

static clause empty_clause();
static void free_clause(clause c);
static char* copy_str(char* str);
static void add_litteral(clause* c, char name, short isnot);
static int contains_letters(char* str);
static int in_clause(clause c, char name);
static void free_array_str(array_str arr);
static array_str get_words(char* str, char* delim);
static int eval_litteral(litteral* l, short val);
static CNF* copy_CNF_aux(CNF* cnf, short clean);
static clause copy_clause_aux(clause c, short clean);

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
    cnf->name = get_next_name();
    cnf->capacity = 5;
    cnf->size = 0;
    cnf->val = -1;
    cnf->clauses = malloc(sizeof(clause) * cnf->capacity);
    if(cnf->clauses == NULL) {
        perror("ERROR malloc");
        exit(1);
    }
    cnf->vars = empty_clause();
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

static int in_clause(clause c, char name) {
    if(c.litts == NULL)
        return 0;
    for(int i=0;i<c.size;i++)
        if(c.litts[i].name == name)
            return 1;
    return 0;
}

static clause empty_clause() {
    clause clause = {.capacity=5, .size=0, .val=-1};
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
    free_clause(cnf->vars);
    free(cnf);
}

static void free_clause(clause c) {
    free(c.litts);
}

void print_CNF(CNF* cnf, short infos) {
    if(cnf == NULL) {
        fprintf(stderr, "CNF is NULL !\n");
        return;
    }
    if(!infos && cnf->val != -1) {
        if(cnf->val)
            puts(" ∅ ");
        else
            puts("{ ∅ }");
        return;
    }
    printf("{ ");
    if(cnf->size == 0)
        printf("empty");
    int one = 1;
    for(int i=0;i<cnf->size;i++) {
        print_clause(cnf->clauses[i], infos);
        if(one && !infos && cnf->clauses[i].val != -1)
            continue;
        one = 0;
        if(i != cnf->size-1 && (infos || cnf->clauses[i+1].val == -1))
            printf(", ");
    }
    printf(" }");
    if(infos && cnf->val != -1)
        printf("=%d", cnf->val);
    puts("");
}

void print_clause(clause c, short infos) {
    if(!infos && c.val != -1)
        return;
    printf("{ ");
    if(c.size == 0)
        printf("empty");
    int one = 1;
    for(int i=0;i<c.size;i++) {
        print_litteral(c.litts[i], infos);
        if(one && !infos && c.litts[i].val != -1)
            continue;
        one = 0;
        if(i != c.size-1 && (infos || c.litts[i+1].val == -1))
            printf(", ");
    }
    printf(" }");
    if(infos && c.val != -1)
        printf("=%d", c.val);
}

void print_litteral(litteral l, short infos) {
    if(!infos && l.val != -1)
        return;
    if(l.isnot)
        printf("¬");
    printf("%c", l.name);
    if(infos && l.eval != -1)
        printf("=%d", l.eval);
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
        add_litteral(&c, name, isnot);
        if(!in_clause(cnf->vars, name))
            add_litteral(&cnf->vars, name, 0);
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

static void add_litteral(clause* c, char name, short isnot) {
    if(c->litts == NULL)
        return;
    if(c->size == c->capacity) {
        litteral* ptr = realloc(c->litts, c->capacity * 2 * sizeof(clause));
        if(ptr == NULL) {
            perror("ERROR add literral malloc");
            exit(1);
        }
        c->litts = ptr;
        c->capacity *= 2;
    }

    c->litts[c->size] = (litteral){.name=name, .isnot=isnot, .val=-1, .eval=-1};
    c->size++;
}

int eval_CNF(CNF* cnf, char name, short val) {
    if(cnf == NULL)
        return -1;
    int final_eval = 1, eval;
    for(int i=0;i<cnf->size;i++) {
        eval = eval_clause(cnf->clauses + i, name, val);
        if(final_eval == 0)
            continue;
        if(eval == 0 || eval == -1)
            final_eval = eval;
    }
    cnf->val = final_eval;
    // On met a jour la valeur dans la variable dans la liste des variables
    litteral* l = get_litteral_by_name(cnf->vars, name);
    if(l != NULL)
        l->val = val;

    return cnf->val;
}

int eval_clause(clause* c, char name, short val) {
    if(c == NULL || c->size == 0) {
        c->val = -1;
        return -1;
    }
    int final_eval = 0, eval = -1;
    for(int i=0;i<c->size;i++) {
        eval = c->litts[i].eval;
        if(c->litts[i].name == name)
            eval = eval_litteral(c->litts + i, val);
        if(final_eval == 1)
            continue;
        if(eval == 1 || eval == -1)
            final_eval = eval;
    }
    c->val = final_eval;
    
    return c->val;
}

static int eval_litteral(litteral* l, short val) {
    l->val = val;
    if(val == -1)
        l->eval = -1;
    else
        l->eval = l->isnot ? !val : val;
    return l->eval;
}

// Correspond au nombre de variable encore non-evaluee dans la clause
size_t real_clause_size(clause* c) {
    if(c == NULL)
        return 0;
    size_t real_size = 0;
    for(int i=0;i<c->size;i++)
        if(c->litts[i].val == -1)
            real_size++;
    return real_size;
}

CNF* copy_CNF(CNF* cnf) {
    return copy_CNF_aux(cnf, 0);
}

CNF* clean_copy_CNF(CNF* cnf) {
    return copy_CNF_aux(cnf, 1);
}

// clean=0 -> no clean, 1-> clean
static CNF* copy_CNF_aux(CNF* cnf, short clean) {
    if(cnf == NULL)
        return NULL;
    CNF* cpy = malloc(sizeof(CNF));
    if(cpy == NULL) {
        perror("ERROR malloc copy_CNF\n");
        exit(1);
    }
    cpy->name = get_next_name();
    cpy->capacity = clean && cnf->val != -1 ? 5 : cnf->capacity;
    cpy->clauses = NULL;
    cpy->size = cnf->size;
    cpy->val = cnf->val;
    cpy->vars = copy_clause_aux(cnf->vars, clean);

    clause* ptr = malloc(sizeof(clause) * cpy->capacity);
    if(ptr == NULL) {
        perror("ERROR malloc copy_CNF\n");
        exit(1);
    }

    if(clean && cnf->val != -1) {
        cpy->clauses = ptr;
        cpy->size = 0;
        return cpy;
    }

    size_t new_size = 0;
    for(size_t i=0;i<cpy->size;i++) {
        if(clean && cnf->clauses[i].val != -1)
            continue;
        ptr[new_size++] = copy_clause_aux(cnf->clauses[i], clean);
    }

    cpy->size = new_size;
    cpy->clauses = ptr;

    return cpy;
}

// clean=0 -> no clean, 1-> clean
static clause copy_clause_aux(clause c, short clean) {
    clause new = {.capacity=c.capacity, .litts=NULL, .size=c.size, .val=c.val};
    if(clean && c.val != -1)
        new.capacity = 5;
    litteral* ptr = malloc(sizeof(litteral) * new.capacity);
    if(ptr == NULL) {
        perror("ERROR malloc copy_clause\n");
        exit(1);
    }
    if(!clean) {
        memmove(ptr, c.litts, sizeof(litteral) * c.size);
    }
    else {
        if(c.val != -1) {
            new.litts = ptr;
            new.size = 0;
            return new;
        }
        size_t new_size = 0;
        for(size_t i=0;i<c.size;i++)
            if(c.litts[i].val == -1)
                ptr[new_size++] = c.litts[i];
        new.size = new_size;
    }
    new.litts = ptr;
    return new;
}

// Renvoie un litteral non-evalue dans la clause c
litteral* get_litteral(clause c) {
    for(size_t i=0;i<c.size;i++)
        if(c.litts[i].val == -1)
            return c.litts + i;
    return NULL;
}

litteral* get_litteral_by_name(clause c, char name) {
    for(size_t i=0;i<c.size;i++)
        if(c.litts[i].name == name)
            return c.litts + i;
    return NULL;
}

static char get_next_name() {
    char name = next_name;
    next_name = next_name == 'Z' ? 'A' : next_name + 1;
    return name;
}
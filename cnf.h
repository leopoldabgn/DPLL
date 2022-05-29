#ifndef DPLL_H
#define DPLL_H

// when eval=1, then val is attributed to litteral
typedef struct {
    char name;
    short isnot, val, eval; // 0 by default. 1 if NOT(litt)
}litteral;

typedef struct {
    size_t size, capacity;
    short val;
    litteral* litts;
}clause;

typedef struct {
    size_t size, capacity;
    short val;
    clause* clauses;
    clause vars; // Liste avec le nom des variables
}CNF;

typedef struct {
    char** val;
    size_t len;
}array_str;

CNF* empty_CNF();
void free_CNF(CNF* cnf);
void print_CNF(CNF* cnf);
void print_clause(clause c);
CNF* string_to_CNF(char* str);
int add_clause(CNF* cnf, const char* exp);
int eval_clause(clause* c, char name, short val);
int eval_CNF(CNF* cnf, char name, short val);
size_t real_clause_size(clause* c);
void print_litteral(litteral l);

#endif
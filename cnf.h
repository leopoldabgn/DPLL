#ifndef DPLL_H
#define DPLL_H

// when eval=1, then val is attributed to litteral
typedef struct {
    char name;
    short isnot, val, eval; // 0 by default. 1 if NOT(litt)
}litteral;

typedef struct {
    size_t size, capacity;
    litteral* litts;
}clause;

typedef struct {
    size_t size, capacity;
    clause* clauses;
}CNF;

typedef struct {
    char** val;
    size_t len;
}array_str;

CNF* empty_CNF();
void free_CNF(CNF* cnf);
void print_CNF(CNF* cnf);
CNF* string_to_CNF(char* str);
int add_clause(CNF* cnf, const char* exp);

#endif
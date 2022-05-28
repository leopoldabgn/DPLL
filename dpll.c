#include <stdlib.h>
#include <stdio.h>

#include "cnf.h"

static void help();
int check_monotony(CNF* cnf, litteral l);
static int check_monotony_clause(clause c, litteral l);
int eval_CNF(CNF* cnf, char name, short val);
static int eval_clause(clause* c, char name, short val);
static int eval_litteral(litteral* l, short val);

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Merci de passer une string en argument !\n\n");
        help();
        exit(1);
    }
    CNF* cnf = string_to_CNF(argv[1]);

    print_CNF(cnf);
    eval_CNF(cnf, 'x', 1);
    print_CNF(cnf);

    
    
    litteral l = {.name='x', .isnot=0};
    printf("monotonie %c : %d\n", l.name, check_monotony(cnf, l));
    print_clause(cnf->vars);

    free_CNF(cnf);
    return EXIT_SUCCESS;
}

static void help() {
    puts("Help :");
    puts("./dpll \"{ {x, y, z}, {:x, :z}, {:z} }\"\n");
    puts("\":\" represente le NOT().");
    puts("Vous pouvez le remplacer par n'importe");
    puts("quel caractere non-alphabetic.");
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
    
    return cnf->val;
}

static int eval_clause(clause* c, char name, short val) {
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

int check_monotony(CNF* cnf, litteral l) {
    if(cnf == NULL)
        return 0;
    int not_in_cnf = 1, monotony = 0;
    for(int i=0;i<cnf->size;i++) {
        monotony = check_monotony_clause(cnf->clauses[i], l);
        if(!monotony)
            return 0;
        not_in_cnf = monotony == 2 ? not_in_cnf : 0;
    }
    return not_in_cnf ? 2 : 1;
}

// 0-> pas monotone, 1-> monotone, 2-> absent.
static int check_monotony_clause(clause c, litteral l) {
    if(c.litts == NULL)
        return 0;
    int not_in_clause = 1;
    for(int i=0;i<c.size;i++) {
        if(l.name == c.litts[i].name) {
            if(l.isnot == c.litts[i].isnot)
                not_in_clause = 0;
            else
                return 0;
        }
    }
    return not_in_clause ? 2 : 1;
}
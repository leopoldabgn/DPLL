#include <stdlib.h>
#include <stdio.h>

#include "cnf.h"

static void help();
static int check_monotony_clause(clause c, litteral l);
static int try_monotony(CNF* cnf, char name, short isnot);

int check_monotony(CNF* cnf, litteral l);
int DPLL(CNF* cnf);

static litteral* get_singleton_clause(clause c, short isnot);
litteral* get_singleton_CNF(CNF* cnf, short isnot);

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Merci de passer une string en argument !\n\n");
        help();
        exit(1);
    }
    CNF* cnf = string_to_CNF(argv[1]);

    DPLL(cnf);

    /*
    print_CNF(cnf);
    eval_CNF(cnf, 'x', 1);
    print_CNF(cnf);

    litteral l = {.name='x', .isnot=0};
    printf("monotonie %c : %d\n", l.name, check_monotony(cnf, l));
    print_clause(cnf->vars);
    */

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

int check_monotony(CNF* cnf, litteral l) {
    if(cnf == NULL || cnf->val != -1)
        return 0;
    int not_in_cnf = 1, monotony = 0;
    for(int i=0;i<cnf->size;i++) {
        if(cnf->clauses[i].val != -1)
            continue;
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
        if(l.name == c.litts[i].name && c.litts[i].val == -1) {
            if(l.isnot == c.litts[i].isnot)
                not_in_clause = 0;
            else
                return 0;
        }
    }
    return not_in_clause ? 2 : 1;
}

// Renvoie un litteral correspondant a un singleton.
litteral* get_singleton_CNF(CNF* cnf, short isnot) {
    if(cnf == NULL)
        return NULL;
    litteral* l = NULL;
    for(int i=0;i<cnf->size;i++) {
        l = get_singleton_clause(cnf->clauses[i], isnot);
        if(l != NULL)
            return l;
    }
    return NULL;
}

static litteral* get_singleton_clause(clause c, short isnot) {
    if(c.litts == NULL)
        return NULL;
    int total = 0;
    litteral* l = NULL;
    for(int i=0;i<c.size;i++) {
        if(c.litts[i].val == -1) {
            if(total)
                return NULL;
            if(isnot != c.litts[i].isnot)
                continue;
            total++;
            l = c.litts + i;
        }
    }

    return l;
}

static int try_monotony(CNF* cnf, char name, short isnot) {
    litteral l = {.name=name, .isnot=isnot};
    if(check_monotony(cnf, l)) {
        eval_CNF(cnf, name, !isnot);
        return 1;
    }

    return 0;
}

// Renvoie 1 si la CNF est satisfaisable. 0 sinon. (+ tard un arg. "int verbose").
int DPLL(CNF* cnf) {
    if(cnf == NULL)
        return 0;
    printf("Soit A = ");
    print_CNF(cnf);
    printf("\nOn cherche a resoudre DPLL(A) :\n\n");

    litteral* vars = cnf->vars.litts;
    size_t size = cnf->vars.size;
    int end = 0;
    litteral tmp;
    // Tant qu'on ne trouve pas une affection statisfaible pour la cnf
    while(cnf->val == -1) {
        end=0;
        // On verifie la regle (3) : la monotonie :
        for(int i=0;!end && i<size;i++) {
            for(int j=0;vars[i].val == -1 && j<2;j++) {
                if(try_monotony(cnf, vars[i].name, j)) { // On verifie la monotonie de x puis de NOT(x).
                    printf("Règle 3 : monotonie de ");
                    tmp = vars[i];
                    tmp.isnot = j;
                    print_litteral(tmp);
                    puts(" dans A.");
                    print_CNF(cnf);
                    puts("");
                    vars[i].val = 1; // Peut importe la valeur tant que != -1
                    end = 1;
                    break;
                }
            }
        }
        if(end) // Si la regle (3) a ete applique, on recommence du debut.
            continue;

        // On verifie la regle (4) : le singleton (ex: {x})
        // On verifie la regle (5) : le NOT(singleton) (ex: {¬x})
        litteral* singleton;
        for(int i=0;i<2;i++) {
            singleton = get_singleton_CNF(cnf, i);
            if(singleton != NULL) {
                printf("Règle %d : singleton {", (4+i));
                print_litteral(*singleton);
                printf("} dans A.\n");
                eval_CNF(cnf, singleton->name, !i);
                print_CNF(cnf);
                puts("");
                eval_clause(&cnf->vars, singleton->name, !i);
                end = 1;
                break;
            }
        }
        if(end) // Si regle 4 ou 5 applique, on recommence du debut.
            continue;

        // On applique la regle (6), derniere regle possible
        // Soit : SAT = [x/0]A OR [x/1]A
        // On a besoin d'une fonction pour copier une cnf... TODO.
        // On applique DPLL sur copy avec x/0 et x/1 ?
        // Attention, on aura pas le detail avec les bons printf ?
        // Comment resoudre ca ?
        break;
    }

    if(cnf->val == 1) {
        printf("Règle 1 : A = ∅\n");
        printf("----> SATISFAISABLE\n\n");
    }
    else if(cnf->val == 0) {
        printf("Règle 2 : ∅ ∈ A\n");
        printf("----> NON-SATISFAISABLE\n\n");
    }
    else {
        printf("----> CNF NON RESOLU\n\n");
    }

    return cnf->val;
}
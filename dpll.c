#include <stdlib.h>
#include <stdio.h>

#include "cnf.h"

static void help();
int check_monotony(CNF* cnf, litteral l);
static int check_monotony_clause(clause c, litteral l);

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Merci de passer une string en argument !\n\n");
        help();
        exit(1);
    }
    CNF* cnf = string_to_CNF(argv[1]);

    print_CNF(cnf);
    litteral l = {.name='x', .isnot=0};
    printf("monotonie %c : %d\n", l.name, check_monotony(cnf, l));

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
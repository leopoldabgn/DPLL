#include <stdlib.h>
#include <stdio.h>

#include "dpll.h"

int main(int argc, char* argv[]) {

    CNF* cnf = empty_CNF();

    add_clause(cnf, "{ x, y, z }");
    add_clause(cnf, "{ !x, y, !y }");

    print_CNF(cnf);

    free_CNF(cnf);

    return EXIT_SUCCESS;
}
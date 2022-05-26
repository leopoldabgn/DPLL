#include <stdlib.h>
#include <stdio.h>

#include "dpll.h"

int main(int argc, char* argv[]) {

    CNF* cnf = string_to_CNF(argv[1]);

    print_CNF(cnf);

    free_CNF(cnf);
    return EXIT_SUCCESS;
}
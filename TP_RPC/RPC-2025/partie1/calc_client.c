#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

int main(int argc, char **argv)
{
    CLIENT *clnt;
    int arg = 5;
    int *result;

    clnt = clnt_create("localhost", CALCPROG, CALCVERS, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror("Error creating client");
        return 1;
    }

    result = mul_0(&arg, clnt);
    if (result == NULL) {
        clnt_perror(clnt, "call failed");
        clnt_destroy(clnt);
        return 1;
    }

    printf("MUL(%d) = %d\n", arg, *result);

    clnt_destroy(clnt);
    return 0;
}

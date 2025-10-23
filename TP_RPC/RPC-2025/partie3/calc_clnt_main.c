#include "calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    CLIENT *clnt;
    tuple args;
    int *result;
    int val;

    if (argc < 3) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s <host> init <valeur>\n", argv[0]);
        fprintf(stderr, "  %s <host> add <a> <b|mem>\n", argv[0]);
        fprintf(stderr, "  %s <host> mul <a> <b|mem>\n", argv[0]);
        exit(1);
    }

    // Connexion au serveur RPC
    clnt = clnt_create(argv[1], CALCPROG, CALCVERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(argv[1]);
        exit(1);
    }

    // ----- INIT -----
    if (strcmp(argv[2], "init") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s <host> init <valeur>\n", argv[0]);
            exit(1);
        }
        val = atoi(argv[3]);
        result = init_1(&val, clnt);
        if (result == NULL) {
            clnt_perror(clnt, "Erreur RPC INIT");
        } else {
            printf("[CLIENT] Mémoire initialisée à %d\n", val);
        }
    }

    // ----- ADD -----
    else if (strcmp(argv[2], "add") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s <host> add <a> <b|mem>\n", argv[0]);
            exit(1);
        }
        args.a = atoi(argv[3]);
        if (strcmp(argv[4], "mem") == 0) {
            args.mem = 1;
        } else {
            args.mem = 0;
            args.b = atoi(argv[4]);
        }

        result = add_1(&args, clnt);
        if (result == NULL) {
            clnt_perror(clnt, "Erreur RPC ADD");
        } else {
            printf("[CLIENT] Résultat ADD = %d\n", *result);
        }
    }

    // ----- MUL -----
    else if (strcmp(argv[2], "mul") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s <host> mul <a> <b|mem>\n", argv[0]);
            exit(1);
        }
        args.a = atoi(argv[3]);
        if (strcmp(argv[4], "mem") == 0) {
            args.mem = 1;
        } else {
            args.mem = 0;
            args.b = atoi(argv[4]);
        }

        result = mul_1(&args, clnt);
        if (result == NULL) {
            clnt_perror(clnt, "Erreur RPC MUL");
        } else {
            printf("[CLIENT] Résultat MUL = %d\n", *result);
        }
    }

    // ----- Commande inconnue -----
    else {
        fprintf(stderr, "Commande inconnue: %s\n", argv[2]);
        exit(1);
    }

    clnt_destroy(clnt);
    return 0;
}

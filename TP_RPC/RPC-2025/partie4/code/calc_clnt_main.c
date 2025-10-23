#include "calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/* Configuration des timeouts et retry */
#define TIMEOUT_SECONDS 5
#define TIMEOUT_USECONDS 0
#define MAX_RETRY_ATTEMPTS 3
#define RETRY_DELAY_SECONDS 2

/* Structure pour gérer les timeouts */
struct timeval timeout = {TIMEOUT_SECONDS, TIMEOUT_USECONDS};

/* Fonction pour configurer les timeouts sur le client */
void configure_client_timeouts(CLIENT *clnt)
{
    clnt_control(clnt, CLSET_TIMEOUT, (char *)&timeout);
    
    /* Optionnel: configurer aussi le timeout de retry */
    struct timeval retry_timeout = {TIMEOUT_SECONDS * 2, 0};
    clnt_control(clnt, CLSET_RETRY_TIMEOUT, (char *)&retry_timeout);
}

/* Fonction pour afficher un message d'erreur détaillé */
void print_rpc_error(CLIENT *clnt, const char *operation)
{
    struct rpc_err err;
    clnt_geterr(clnt, &err);
    
    fprintf(stderr, "[ERREUR] Opération %s échouée\n", operation);
    fprintf(stderr, "  Code d'erreur RPC: %d\n", err.re_status);
    
    switch(err.re_status) {
        case RPC_TIMEDOUT:
            fprintf(stderr, "  Timeout: Le serveur n'a pas répondu dans les %d secondes\n", 
                    TIMEOUT_SECONDS);
            fprintf(stderr, "  Vérifiez que le serveur est démarré et accessible\n");
            break;
        case RPC_CANTRECV:
            fprintf(stderr, "  Impossible de recevoir la réponse du serveur\n");
            break;
        case RPC_CANTSEND:
            fprintf(stderr, "  Impossible d'envoyer la requête au serveur\n");
            break;
        case RPC_PROGNOTREGISTERED:
            fprintf(stderr, "  Le programme RPC n'est pas enregistré sur le serveur\n");
            break;
        case RPC_SYSTEMERROR:
            fprintf(stderr, "  Erreur système: %s\n", err.re_errno ? strerror(err.re_errno) : "inconnue");
            break;
        default:
            clnt_perror(clnt, "  Détails");
            break;
    }
}

/* Fonction pour effectuer un appel RPC avec retry automatique */
int* call_with_retry(CLIENT *clnt, int *(*rpc_func)(void *, CLIENT *), 
                     void *args, const char *op_name)
{
    int *result;
    int attempt;
    
    for (attempt = 1; attempt <= MAX_RETRY_ATTEMPTS; attempt++) {
        if (attempt > 1) {
            fprintf(stderr, "[RETRY] Tentative %d/%d pour %s...\n", 
                    attempt, MAX_RETRY_ATTEMPTS, op_name);
            sleep(RETRY_DELAY_SECONDS);
        }
        
        result = rpc_func(args, clnt);
        
        if (result != NULL) {
            if (attempt > 1) {
                printf("[SUCCÈS] Opération %s réussie après %d tentative(s)\n", 
                       op_name, attempt);
            }
            return result;
        }
        
        /* Vérifier le type d'erreur */
        struct rpc_err err;
        clnt_geterr(clnt, &err);
        
        /* Ne pas retenter si c'est une erreur non liée au timeout */
        if (err.re_status != RPC_TIMEDOUT && err.re_status != RPC_CANTRECV) {
            fprintf(stderr, "[ERREUR] Échec définitif (erreur non récupérable)\n");
            break;
        }
        
        if (attempt < MAX_RETRY_ATTEMPTS) {
            fprintf(stderr, "[WARNING] Timeout/erreur réseau détectée\n");
        }
    }
    
    /* Échec après tous les retry */
    fprintf(stderr, "\n[ÉCHEC] Opération %s échouée après %d tentatives\n", 
            op_name, MAX_RETRY_ATTEMPTS);
    print_rpc_error(clnt, op_name);
    return NULL;
}

int main(int argc, char *argv[])
{
    CLIENT *clnt;
    tuple args;
    int *result;
    int val;
    struct timeval start_time, end_time;
    double elapsed_ms;

    if (argc < 3) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s <host> init <valeur>\n", argv[0]);
        fprintf(stderr, "  %s <host> add <a> <b|mem>\n", argv[0]);
        fprintf(stderr, "  %s <host> mul <a> <b|mem>\n", argv[0]);
        fprintf(stderr, "\nConfiguration timeout: %d secondes\n", TIMEOUT_SECONDS);
        fprintf(stderr, "Nombre de tentatives: %d\n", MAX_RETRY_ATTEMPTS);
        exit(1);
    }

    // Connexion au serveur RPC avec timeout
    printf("[CLIENT] Connexion au serveur %s...\n", argv[1]);
    clnt = clnt_create(argv[1], CALCPROG, CALCVERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(argv[1]);
        fprintf(stderr, "\n[AIDE] Vérifiez que:\n");
        fprintf(stderr, "  1. Le serveur est démarré (./calc_server)\n");
        fprintf(stderr, "  2. Le serveur est accessible sur %s\n", argv[1]);
        fprintf(stderr, "  3. Aucun firewall ne bloque le port RPC\n");
        exit(1);
    }
    
    /* Configurer les timeouts sur le client */
    configure_client_timeouts(clnt);
    printf("[CLIENT] Timeouts configurés: %d secondes\n", TIMEOUT_SECONDS);

    // ----- INIT -----
    if (strcmp(argv[2], "init") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s <host> init <valeur>\n", argv[0]);
            clnt_destroy(clnt);
            exit(1);
        }
        val = atoi(argv[3]);
        
        printf("[CLIENT] Envoi commande INIT(%d)...\n", val);
        gettimeofday(&start_time, NULL);
        
        result = call_with_retry(clnt, (int *(*)(void *, CLIENT *))init_1, 
                                &val, "INIT");
        
        gettimeofday(&end_time, NULL);
        elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                     (end_time.tv_usec - start_time.tv_usec) / 1000.0;
        
        if (result == NULL) {
            clnt_destroy(clnt);
            exit(1);
        }
        printf("[CLIENT] ✓ Mémoire initialisée à %d (%.2f ms)\n", val, elapsed_ms);
    }

    // ----- ADD -----
    else if (strcmp(argv[2], "add") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s <host> add <a> <b|mem>\n", argv[0]);
            clnt_destroy(clnt);
            exit(1);
        }
        args.a = atoi(argv[3]);
        if (strcmp(argv[4], "mem") == 0) {
            args.mem = 1;
            printf("[CLIENT] Envoi commande ADD(%d, mémoire)...\n", args.a);
        } else {
            args.mem = 0;
            args.b = atoi(argv[4]);
            printf("[CLIENT] Envoi commande ADD(%d, %d)...\n", args.a, args.b);
        }

        gettimeofday(&start_time, NULL);
        
        result = call_with_retry(clnt, (int *(*)(void *, CLIENT *))add_1, 
                                &args, "ADD");
        
        gettimeofday(&end_time, NULL);
        elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                     (end_time.tv_usec - start_time.tv_usec) / 1000.0;
        
        if (result == NULL) {
            clnt_destroy(clnt);
            exit(1);
        }
        printf("[CLIENT] ✓ Résultat ADD = %d (%.2f ms)\n", *result, elapsed_ms);
    }

    // ----- MUL -----
    else if (strcmp(argv[2], "mul") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s <host> mul <a> <b|mem>\n", argv[0]);
            clnt_destroy(clnt);
            exit(1);
        }
        args.a = atoi(argv[3]);
        if (strcmp(argv[4], "mem") == 0) {
            args.mem = 1;
            printf("[CLIENT] Envoi commande MUL(%d, mémoire)...\n", args.a);
        } else {
            args.mem = 0;
            args.b = atoi(argv[4]);
            printf("[CLIENT] Envoi commande MUL(%d, %d)...\n", args.a, args.b);
        }

        gettimeofday(&start_time, NULL);
        
        result = call_with_retry(clnt, (int *(*)(void *, CLIENT *))mul_1, 
                                &args, "MUL");
        
        gettimeofday(&end_time, NULL);
        elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                     (end_time.tv_usec - start_time.tv_usec) / 1000.0;
        
        if (result == NULL) {
            clnt_destroy(clnt);
            exit(1);
        }
        printf("[CLIENT] ✓ Résultat MUL = %d (%.2f ms)\n", *result, elapsed_ms);
    }

    // ----- Commande inconnue -----
    else {
        fprintf(stderr, "[ERREUR] Commande inconnue: %s\n", argv[2]);
        fprintf(stderr, "Commandes valides: init, add, mul\n");
        clnt_destroy(clnt);
        exit(1);
    }

    clnt_destroy(clnt);
    printf("[CLIENT] Connexion fermée proprement\n");
    return 0;
}

/*
 * Implémentation serveur avec détection de pannes et logs
 */

#include "calc.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

int memoire = 0;
int *currante = &memoire;

/* Variables pour statistiques */
static unsigned long add_count = 0;
static unsigned long mul_count = 0;
static unsigned long init_count = 0;

/* Fonction pour obtenir un timestamp */
void print_timestamp() {
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmbuf[64];
    
    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
    printf("[%s.%03ld] ", tmbuf, tv.tv_usec / 1000);
}

/* Fonction pour logger les requêtes */
void log_request(const char *operation, struct svc_req *rqstp) {
    print_timestamp();
    printf("[SERVEUR] Requête %s reçue\n", operation);
}

int *add_1_svc(tuple *argp, struct svc_req *rqstp)
{
    static int result;
    
    add_count++;
    log_request("ADD", rqstp);
    
    if (argp->mem) {
        print_timestamp();
        printf("[ADD] Calcul: %d + mémoire(%d)\n", argp->a, *currante);
        result = argp->a + *currante;
    } else {
        print_timestamp();
        printf("[ADD] Calcul: %d + %d\n", argp->a, argp->b);
        result = argp->a + argp->b;
    }
    
    *currante = result;
    print_timestamp();
    printf("[ADD] Résultat: %d | Mémoire mise à jour: %d | Appels ADD: %lu\n", 
           result, *currante, add_count);
    
    /* Simuler un traitement (optionnel - peut être enlevé pour tests rapides) */
    // sleep(1);
    
    return &result;
}

int *mul_1_svc(tuple *argp, struct svc_req *rqstp)
{
    static int result;
    
    mul_count++;
    log_request("MUL", rqstp);
    
    if (argp->mem) {
        print_timestamp();
        printf("[MUL] Calcul: %d * mémoire(%d)\n", argp->a, *currante);
        result = argp->a * *currante;
    } else {
        print_timestamp();
        printf("[MUL] Calcul: %d * %d\n", argp->a, argp->b);
        result = argp->a * argp->b;
    }
    
    *currante = result;
    print_timestamp();
    printf("[MUL] Résultat: %d | Mémoire mise à jour: %d | Appels MUL: %lu\n", 
           result, *currante, mul_count);
    
    /* Simuler un traitement (optionnel) */
    // sleep(1);
    
    return &result;
}

int *init_1_svc(int *argp, struct svc_req *rqstp)
{
    static int result;
    
    init_count++;
    log_request("INIT", rqstp);
    
    print_timestamp();
    printf("[INIT] Ancienne mémoire: %d | Nouvelle valeur: %d\n", *currante, *argp);
    *currante = *argp;
    result = *currante;
    
    print_timestamp();
    printf("[INIT] Initialisation terminée | Mémoire: %d | Appels INIT: %lu\n", 
           *currante, init_count);
    
    return &result;
}

/* Fonction appelée au démarrage du serveur (optionnel) */
void __attribute__((constructor)) server_startup() {
    print_timestamp();
    printf("========================================\n");
    print_timestamp();
    printf("[SERVEUR] Démarrage du serveur RPC\n");
    print_timestamp();
    printf("[SERVEUR] Programme: 0x%x, Version: %d\n", CALCPROG, CALCVERS);
    print_timestamp();
    printf("========================================\n");
}

/* Fonction appelée à l'arrêt du serveur (optionnel) */
void __attribute__((destructor)) server_shutdown() {
    print_timestamp();
    printf("========================================\n");
    print_timestamp();
    printf("[SERVEUR] Arrêt du serveur\n");
    print_timestamp();
    printf("[STATS] Total ADD: %lu | MUL: %lu | INIT: %lu\n", 
           add_count, mul_count, init_count);
    print_timestamp();
    printf("========================================\n");
}

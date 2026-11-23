#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../include/structures.h"
#include "../include/historique.h"
#include "../include/interface.h"
#include "../include/compte.h"

// Fonction pour avoir la date actuelle
void obtenir_date_actuelle(char *buffer, int taille) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, taille, "%Y-%m-%d %H:%M:%S", tm_info);
}

void ajouter_historique(int id_compte, const char *type, double montant, int id_destinataire) {
    FILE *f = fopen("../data/historique.txt", "a");
    if (f == NULL) {
        printf("Erreur: Impossible d'ouvrir historique.txt\n");
        return;
    }

    char date[30];
    obtenir_date_actuelle(date, sizeof(date));

    // Format: ID,Type,Montant,Date,DestID
    fprintf(f, "%d,%s,%.2f,%s,%d\n", id_compte, type, montant, date, id_destinataire);
    fclose(f);
}

int charger_historique(const char *nom_fichier, Historique tab[], int max) {
    FILE *f = fopen(nom_fichier, "r");
    if (f == NULL) return 0;

    int compteur = 0;
    char buffer[256];
    
    while (compteur < max && fgets(buffer, sizeof(buffer), f)) {
        // On lit la ligne
        // Format: %d,%[^,],%lf,%[^,],%d
        sscanf(buffer, "%d,%19[^,],%lf,%29[^,],%d", 
               &tab[compteur].id_compte,
               tab[compteur].type_operation,
               &tab[compteur].montant,
               tab[compteur].date,
               &tab[compteur].id_destinataire);
        compteur++;
    }
    fclose(f);
    return compteur;
}

void afficher_historique(int id_compte) {
    Historique historique[MAX_HISTORIQUE];
    int nombre = charger_historique("../data/historique.txt", historique, MAX_HISTORIQUE);
    
    printf("\n");
    afficher_centre(COULEUR_CYAN "HISTORIQUE DES TRANSACTIONS" COULEUR_RESET);
    printf("\n");

    int trouve = 0;
    
    // En-tete
    printf(" %s+---------------------+----------------------+-----------------+-----------------+%s\n", COULEUR_BLEU, COULEUR_RESET);
    printf(" %s| %-19s | %-20s | %-15s | %-15s |%s\n", COULEUR_BLEU, "DATE", "TYPE OPERATION", "MONTANT", "DESTINATAIRE", COULEUR_RESET);
    printf(" %s+---------------------+----------------------+-----------------+-----------------+%s\n", COULEUR_BLEU, COULEUR_RESET);

    Compte *compte_user = trouver_compte_par_id(id_compte);
    char devise[10] = "DH";
    if (compte_user) strcpy(devise, compte_user->devise);

    for (int i = 0; i < nombre; i++) {
        if (historique[i].id_compte == id_compte) {
            char destStr[20] = "-";
            if (historique[i].id_destinataire != 0) {
                sprintf(destStr, "ID: %d", historique[i].id_destinataire);
            }

            printf(" %s|%s %-19s %s|%s %-20s %s|%s %12.2f %-3s %s|%s %-15s %s|%s\n", 
                COULEUR_BLEU, COULEUR_RESET, historique[i].date, 
                COULEUR_BLEU, COULEUR_RESET, historique[i].type_operation, 
                COULEUR_BLEU, COULEUR_RESET, historique[i].montant, devise,
                COULEUR_BLEU, COULEUR_RESET, destStr,
                COULEUR_BLEU, COULEUR_RESET);
            trouve = 1;
        }
    }
    
    printf(" %s+---------------------+----------------------+-----------------+-----------------+%s\n", COULEUR_BLEU, COULEUR_RESET);

    if (!trouve) {
        afficher_attention("Aucune operation trouvee pour ce compte.");
    }
}

void sauvegarder_historique(const char *nom_fichier, Historique tab[], int n) {
    // Pas vraiment necessaire si on ajoute a la fin, mais utile pour nettoyer
    FILE *f = fopen(nom_fichier, "w");
    if (f == NULL) return;
    
    for (int i = 0; i < n; i++) {
        fprintf(f, "%d,%s,%.2f,%s,%d\n", 
                tab[i].id_compte, 
                tab[i].type_operation, 
                tab[i].montant, 
                tab[i].date, 
                tab[i].id_destinataire);
    }
    fclose(f);
}

void supprimer_historique(int id_compte) {
    Historique historique[MAX_HISTORIQUE];
    int nombre = charger_historique("../data/historique.txt", historique, MAX_HISTORIQUE);
    
    FILE *f = fopen("../data/historique.txt", "w");
    if (f == NULL) return;
    
    for (int i = 0; i < nombre; i++) {
        // On garde l'enregistrement SEULEMENT s'il n'appartient pas au compte supprime
        if (historique[i].id_compte != id_compte) {
            fprintf(f, "%d,%s,%.2f,%s,%d\n", 
                    historique[i].id_compte, 
                    historique[i].type_operation, 
                    historique[i].montant, 
                    historique[i].date, 
                    historique[i].id_destinataire);
        }
    }
    fclose(f);
}

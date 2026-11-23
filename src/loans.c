#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/loans.h"
#include "../include/compte.h"
#include "../include/interface.h"
#include "../include/security.h"

const char* FICHIER_CREDITS = "../data/loans.txt";
const char* CLE_CREDIT = "LOAN_SECRET";

Credit *liste_credits = NULL;

void sauvegarder_credits() {
    FILE *f = fopen(FICHIER_CREDITS, "w");
    if (f == NULL) return;
    
    Credit *courant = liste_credits;
    char buffer[256];
    while (courant != NULL) {
        sprintf(buffer, "%d,%d,%.2f,%.2f,%d", 
            courant->id, courant->id_compte, courant->montant, courant->restant, courant->statut);
        chiffrement_xor(buffer, CLE_CREDIT);
        fprintf(f, "%s\n", buffer);
        courant = courant->suivant;
    }
    fclose(f);
}

void charger_credits() {
    // Liberer la liste
    Credit *curr = liste_credits;
    while (curr) {
        Credit *suiv = curr->suivant;
        free(curr);
        curr = suiv;
    }
    liste_credits = NULL;

    FILE *f = fopen(FICHIER_CREDITS, "r");
    if (f == NULL) return;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), f)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        chiffrement_xor(buffer, CLE_CREDIT);

        Credit *noeud = (Credit*)malloc(sizeof(Credit));
        sscanf(buffer, "%d,%d,%lf,%lf,%d", 
            &noeud->id, &noeud->id_compte, &noeud->montant, &noeud->restant, &noeud->statut);
        noeud->suivant = liste_credits;
        liste_credits = noeud;
    }
    fclose(f);
}

void demander_credit(int id_compte, double montant) {
    charger_credits();
    Credit *noeud = (Credit*)malloc(sizeof(Credit));
    
    // Generation ID simple
    int id_max = 0;
    Credit *courant = liste_credits;
    while(courant) {
        if(courant->id > id_max) id_max = courant->id;
        courant = courant->suivant;
    }
    
    noeud->id = id_max + 1;
    noeud->id_compte = id_compte;
    noeud->montant = montant;
    noeud->restant = montant; // Les interets pourraient etre ajoutes ici
    noeud->statut = 0; // En attente
    noeud->suivant = liste_credits;
    liste_credits = noeud;
    
    sauvegarder_credits();
    // Note: afficher_succes est dans interface.h
    afficher_succes("Demande de credit envoyee a l'administrateur.");
}

void gerer_credits() {
    charger_credits();
    Credit *courant = liste_credits;
    int trouve = 0;
    
    while(courant) {
        if (courant->statut == 0) {
            trouve = 1;
            printf("\nCredit ID: %d | Compte: %d | Montant: %.2f\n", courant->id, courant->id_compte, courant->montant);
            printf("Accepter (1) ou Rejeter (2) ? ");
            int choix;
            scanf("%d", &choix);
            if (choix == 1) {
                courant->statut = 1;
                // Ajouter l'argent sur le compte
                Compte *c = trouver_compte_par_id(courant->id_compte);
                if (c) {
                    c->solde += courant->montant;
                    sauvegarder_comptes();
                    afficher_succes("Credit approuve et fonds ajoutes.");
                }
            } else {
                courant->statut = 2;
                afficher_attention("Credit rejete.");
            }
        }
        courant = courant->suivant;
    }
    
    if (!trouve) afficher_info("Aucune demande de credit en attente.");
    sauvegarder_credits();
}

void payer_credit(int id_compte, double montant) {
    charger_credits();
    Credit *courant = liste_credits;
    int paye = 0;
    
    while(courant) {
        if (courant->id_compte == id_compte && courant->statut == 1 && courant->restant > 0) {
            Compte *c = trouver_compte_par_id(id_compte);
            if (c->solde >= montant) {
                if (montant > courant->restant) montant = courant->restant;
                
                c->solde -= montant;
                courant->restant -= montant;
                if (courant->restant <= 0) courant->statut = 3; // Paye
                
                sauvegarder_comptes();
                sauvegarder_credits();
                
                char msg[100];
                sprintf(msg, "Paiement de %.2f effectue. Restant: %.2f", montant, courant->restant);
                afficher_succes(msg);
                paye = 1;
                break; // Payer un credit a la fois pour simplifier
            } else {
                afficher_erreur("Solde insuffisant.");
                return;
            }
        }
        courant = courant->suivant;
    }
    
    if (!paye) afficher_info("Aucun credit actif a payer.");
}

void afficher_mes_credits(int id_compte) {
    charger_credits();
    Credit *courant = liste_credits;
    int trouve = 0;
    
    printf("\n");
    afficher_centre(COULEUR_MAGENTA "MES CREDITS" COULEUR_RESET);
    printf("\n");

    Compte *compte_user = trouver_compte_par_id(id_compte);
    char devise[10] = "DH";
    if (compte_user) strcpy(devise, compte_user->devise);

    while(courant) {
        if (courant->id_compte == id_compte) {
            const char* status_str = "Inconnu";
            const char* status_color = COULEUR_BLANC;
            
            if (courant->statut == 0) { status_str = "EN ATTENTE"; status_color = COULEUR_JAUNE; }
            if (courant->statut == 1) { status_str = "ACTIF"; status_color = COULEUR_VERT; }
            if (courant->statut == 2) { status_str = "REJETE"; status_color = COULEUR_ROUGE; }
            if (courant->statut == 3) { status_str = "PAYE"; status_color = COULEUR_CYAN; }
            
            printf("  %s+--------------------------------------------------+%s\n", status_color, COULEUR_RESET);
            printf("  %s| CREDIT #%-40d |%s\n", status_color, courant->id, COULEUR_RESET);
            printf("  %s+--------------------------------------------------+%s\n", status_color, COULEUR_RESET);
            printf("  %s|%s Montant Initial : %15.2f %-3s             %s|%s\n", status_color, COULEUR_RESET, courant->montant, devise, status_color, COULEUR_RESET);
            printf("  %s|%s Reste a Payer   : %15.2f %-3s             %s|%s\n", status_color, COULEUR_RESET, courant->restant, devise, status_color, COULEUR_RESET);
            printf("  %s|%s Statut          : %s%-28s%s %s|%s\n", status_color, COULEUR_RESET, status_color, status_str, COULEUR_RESET, status_color, COULEUR_RESET);
            printf("  %s+--------------------------------------------------+%s\n", status_color, COULEUR_RESET);
            printf("\n");
            
            trouve = 1;
        }
        courant = courant->suivant;
    }
    if (!trouve) {
        afficher_info("Vous n'avez aucun credit en cours.");
    }
}

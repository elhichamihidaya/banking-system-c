#include <stdio.h>
#include <string.h> 
#include <time.h>
#include "../include/structures.h"     
#include "../include/operations.h"     
#include "../include/compte.h"         
#include "../include/historique.h"     


/*
 verifier si le montant est positif.
 */
int verifier_montant(double montant) {
    if (montant <= 0.0) { // On ne peut pas deposer 0 ou moins.
        printf("Erreur (Op): Le montant doit etre positif.\n");
        return 0;
    }
    return 1;
}

/*
 * Generer un recu de transaction
 */
void generer_recu(int id_compte, const char* type, double montant, double nouveau_solde, int id_destinataire) {
    printf("\n Voulez-vous un recu ? (1=Oui, 0=Non): ");
    int choix;
    scanf("%d", &choix);
    
    if (choix != 1) return;
    
    FILE *f = fopen("../data/recu.txt", "w");
    if (f == NULL) {
        printf("Erreur: Impossible de creer le recu.\n");
        return;
    }
    
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date[30];
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm_info);
    
    Compte *c = trouver_compte_par_id(id_compte);
    char devise[10] = "DH";
    if (c) strcpy(devise, c->devise);

    fprintf(f, "========================================\n");
    fprintf(f, "           RECU DE TRANSACTION          \n");
    fprintf(f, "========================================\n");
    fprintf(f, "Date        : %s\n", date);
    fprintf(f, "Compte ID   : %d\n", id_compte);
    fprintf(f, "Operation   : %s\n", type);
    fprintf(f, "Montant     : %.2f %s\n", montant, devise);
    if (id_destinataire != 0) {
        fprintf(f, "Destinataire: %d\n", id_destinataire);
    }
    fprintf(f, "----------------------------------------\n");
    fprintf(f, "Nouveau Solde: %.2f %s\n", nouveau_solde, devise);
    fprintf(f, "========================================\n");
    fprintf(f, "Merci de votre confiance.\n");
    
    fclose(f);
    printf("Recu genere dans 'recu.txt'.\n");
}


/*
 * Logique pour deposer de l'argent
 */
int deposer_argent(int id_compte, double montant) {
    
    printf("LOG (Op): Depot de %.2f sur le compte %d...\n", montant, id_compte);
    
    // Etape 1: Verifier si le montant est valide.
    if (verifier_montant(montant) == 0) {
        return 0; // Echec
    }

    // Etape 2: Demander au module 'compte' de trouver le compte.
    Compte* ptr_compte = trouver_compte_par_id(id_compte); 
    
    // Etape 3: Verifier si on l'a trouve.
    if (ptr_compte == NULL) {
        printf("Erreur (Op): Compte ID %d non trouve.\n", id_compte);
        return 0; // Echec
    }
    
    // Etape 4: Verifier s'il est verrouille.
    if (ptr_compte->verrouille == 1) {
        printf("Erreur (Op): Le compte %d est verrouille. Depot impossible.\n", id_compte);
        return 0; // Echec
    }

    // OK, on peut faire l'operation :
    
    // On change le solde en memoire
    ptr_compte->solde = ptr_compte->solde + montant;
    
    // On dit au module 'historique' d'enregistrer ce qu'on a fait
    ajouter_historique(id_compte, "depot", montant, 0); 

    // On dit au module 'compte' de sauvegarder tous les changements dans le fichier txt
    sauvegarder_comptes(); 
    
    printf("LOG (Op): Depot reussi. Nouveau solde: %.2f\n", ptr_compte->solde);
    
    generer_recu(id_compte, "DEPOT", montant, ptr_compte->solde, 0);
    
    return 1; // Succes
}


/*
 * Logique pour retirer de l'argent
 */
int retirer_argent(int id_compte, double montant) {
    
    printf("LOG (Op): Tentative de retrait de %.2f du compte %d...\n", montant, id_compte);

    // Etape 1: Montant valide ?
    if (verifier_montant(montant) == 0) {
        return 0;
    }

    // Etape 2: Trouver le compte (comme pour deposit)
    Compte* ptr_compte = trouver_compte_par_id(id_compte); 
    
    // Etape 3: Il existe ?
    if (ptr_compte == NULL) {
        printf("Erreur (Op): Compte ID %d non trouve.\n", id_compte);
        return 0;
    }
    
    // Etape 4: Il est verrouille ?
    if (ptr_compte->verrouille == 1) {
        printf("Erreur (Op): Le compte %d est verrouille. Retrait impossible.\n", id_compte);
        return 0;
    }

    // Etape 5: (LA VERIF IMPORTANTE) Assez d'argent ?
    if (ptr_compte->solde < montant) {
        printf("Erreur (Op): Solde insuffisant (Solde: %.2f, Demande: %.2f).\n", ptr_compte->solde, montant);
        return 0;
    }

    // OK, on peut faire le retrait :
    
    // On change le solde
    ptr_compte->solde = ptr_compte->solde - montant;
    
    // On enregistre dans l'historique
    ajouter_historique(id_compte, "retrait", montant, 0); 

    // On sauvegarde tout
    sauvegarder_comptes(); 
    
    // Message de succes
    printf("LOG (Op): Le retrait d'un montant de %.2f a ete effectue sur le compte %d par succes.\n", montant, id_compte);
    printf("LOG (Op): Nouveau solde: %.2f\n", ptr_compte->solde);
    
    generer_recu(id_compte, "RETRAIT", montant, ptr_compte->solde, 0);
    
    return 1; // Succes
}


/*
 * Logique pour un virement (le plus dur)
 */
int faire_virement(int id_expediteur, int id_destinataire, double montant) {
    
    printf("LOG (Op): Tentative de virement de %.2f de %d vers %d...\n", montant, id_expediteur, id_destinataire);
    
    // Etape 1: Montant valide ?
    if (verifier_montant(montant) == 0) {
        return 0;
    }

    // On verifie que ce n'est pas le meme compte
    if (id_expediteur == id_destinataire) {
        printf("Erreur (Op): Un compte ne peut pas se faire un virement a lui-meme.\n");
        return 0;
    }

    // Etape 2: Trouver l'expediteur
    Compte* ptr_expediteur = trouver_compte_par_id(id_expediteur);
    
    // Etape 3: Trouver le destinataire
    Compte* ptr_destinataire = trouver_compte_par_id(id_destinataire);

    // Etape 4: Verifier que les deux existent et ne sont pas verrouilles
    if (ptr_expediteur == NULL) {
        printf("Erreur (Op): Compte expediteur %d non trouve.\n", id_expediteur);
        return 0;
    }
    if (ptr_destinataire == NULL) {
        printf("Erreur (Op): Compte destinataire %d non trouve.\n", id_destinataire);
        return 0;
    }
    
    if (ptr_expediteur->verrouille == 1) {
        printf("Erreur (Op): Compte expediteur %d est verrouille.\n", id_expediteur);
        return 0;
    }
    if (ptr_destinataire->verrouille == 1) {
        printf("Erreur (Op): Compte destinataire %d est verrouille. Virement impossible.\n", id_destinataire);
        return 0;
    }

    // Etape 5: Verifier si l'expediteur a assez d'argent
    if (ptr_expediteur->solde < montant) {
        printf("Erreur (Op): Solde insuffisant pour le compte %d (Solde: %.2f, Demande: %.2f).\n", id_expediteur, ptr_expediteur->solde, montant);
        return 0;
    }

    // C'est bon, on fait l'operation
    
    // On retire l'argent du premier...
    ptr_expediteur->solde = ptr_expediteur->solde - montant;
    
    // ...et on le donne au second.
    ptr_destinataire->solde = ptr_destinataire->solde + montant;
    
    // On enregistre l'historique pour l'expediteur
    ajouter_historique(id_expediteur, "transfert_de", montant, id_destinataire);
    
    // On enregistre l'historique pour le destinataire
    ajouter_historique(id_destinataire, "transfert_vers", montant, id_expediteur);
    
    // On sauvegarde tout (UNE SEULE FOIS !)
    sauvegarder_comptes();

    // Message de succes
    printf("LOG (Op): Virement de %.2f de %d vers %d reussi.\n", montant, id_expediteur, id_destinataire);
    printf("LOG (Op): Nouveau solde expediteur (%d): %.2f\n", id_expediteur, ptr_expediteur->solde);
    printf("LOG (Op): Nouveau solde destinataire (%d): %.2f\n", id_destinataire, ptr_destinataire->solde);

    generer_recu(id_expediteur, "VIREMENT", montant, ptr_expediteur->solde, id_destinataire);

    return 1;
}

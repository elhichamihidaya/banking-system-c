#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/structures.h"
#include "../include/compte.h"
#include "../include/security.h"
#include "../include/historique.h"

const char* FICHIER_COMPTES = "../data/comptes.txt";
const char* FICHIER_DEMANDES = "../data/demandes_creation.txt";
const char* CLE_CHIFFREMENT = "BANKFLOW_SECRET";

// Tete de la liste chainee
Compte *liste_comptes = NULL;
int est_charge = 0;

// Fonction pour ajouter un compte a la liste
void ajouter_compte_liste(Compte *nouveau_compte) {
    Compte *noeud = (Compte*)malloc(sizeof(Compte));
    if (noeud == NULL) {
        printf("Erreur d'allocation memoire !\n");
        return;
    }
    *noeud = *nouveau_compte; // Copie des donnees
    noeud->suivant = NULL;

    if (liste_comptes == NULL) {
        liste_comptes = noeud;
    } else {
        Compte *courant = liste_comptes;
        while (courant->suivant != NULL) {
            courant = courant->suivant;
        }
        courant->suivant = noeud;
    }
}

// Fonction pour liberer la memoire
void liberer_liste() {
    Compte *courant = liste_comptes;
    while (courant != NULL) {
        Compte *temp = courant;
        courant = courant->suivant;
        free(temp);
    }
    liste_comptes = NULL;
}

void verifier_chargement() {
    if (!est_charge) {
        charger_comptes();
        est_charge = 1;
    }
}

int charger_comptes() {
    // Si deja charge, on vide d'abord pour eviter les doublons si rappele
    if (est_charge) liberer_liste();

    FILE *f = fopen(FICHIER_COMPTES, "r");
    if (f == NULL) {
        return 0;
    }

    char buffer[512];
    int compteur = 0;
    
    while (fgets(buffer, sizeof(buffer), f)) {
        // Decryptage
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0'; // Enlever le saut de ligne pour le decryptage
        chiffrement_xor(buffer, CLE_CHIFFREMENT);
        
        Compte temp;
        // Initialisation pour eviter les problemes
        memset(&temp, 0, sizeof(Compte));
        
        int items_read = sscanf(buffer, "%d,%49[^,],%19[^,],%49[^,],%19[^,],%19[^,],%d,%99[^,],%19[^,],%19[^,],%d,%lf,%4[^,],%d,%d",
               &temp.id, temp.mot_de_passe, temp.date_creation, temp.nom, temp.cin, temp.date_naissance,
               &temp.age, temp.adresse, temp.telephone, temp.citoyennete,
               &temp.type_compte, &temp.solde, temp.devise, &temp.verrouille, &temp.role_admin);
        
        if (items_read < 15) {
            // Backward compatibility
            if (items_read == 14) {
                 // Old format: verrouille is item 13, role_admin is item 14. devise was missing.
                 // We need to re-parse or easier: wait, sscanf layout changed.
                 // OLD: type,%lf,%d,%d -> solde, verrouille, role_admin
                 // NEW: type,%lf,%s,%d,%d -> solde, devise, verrouille, role_admin
                 // If old line "...,1,100.00,0,1" is parsed with new format "%lf,%s,%d,%d":
                 // %lf reads 100.00. Next is "0" (verrouille). %s reads "0". Next is "1" (role). %d reads 1. 
                 // It might get messy.
                 // Easier approach: Parse manually or use 2 sscanf.
                 
                 // Let's try parsing with old format first if new fails?
                 // Or better: Re-parse with old format.
                 int old_v, old_r;
                 sscanf(buffer, "%d,%49[^,],%19[^,],%49[^,],%19[^,],%19[^,],%d,%99[^,],%19[^,],%19[^,],%d,%lf,%d,%d",
                    &temp.id, temp.mot_de_passe, temp.date_creation, temp.nom, temp.cin, temp.date_naissance,
                    &temp.age, temp.adresse, temp.telephone, temp.citoyennete,
                    &temp.type_compte, &temp.solde, &temp.verrouille, &temp.role_admin);
                 strcpy(temp.devise, "MAD");
            }
        }
        
        // Verifier les doublons dans la liste actuelle
        int existe = 0;
        Compte *verif = liste_comptes;
        while (verif != NULL) {
            if (verif->id == temp.id) {
                existe = 1;
                break;
            }
            verif = verif->suivant;
        }

        if (!existe) {
            ajouter_compte_liste(&temp);
            compteur++;
        }
    }
    fclose(f);
    est_charge = 1; // Marquer comme charge pour eviter les doublons
    return compteur;
}

void sauvegarder_comptes() {
    FILE *f = fopen(FICHIER_COMPTES, "w");
    if (f == NULL) {
        printf("Erreur: Impossible de sauvegarder les comptes.\n");
        return;
    }

    Compte *courant = liste_comptes;
    char buffer[512];
    while (courant != NULL) {
        sprintf(buffer, "%d,%s,%s,%s,%s,%s,%d,%s,%s,%s,%d,%.2f,%s,%d,%d",
               courant->id, courant->mot_de_passe, courant->date_creation, courant->nom, courant->cin, courant->date_naissance,
               courant->age, courant->adresse, courant->telephone, courant->citoyennete,
               courant->type_compte, courant->solde, courant->devise, courant->verrouille, courant->role_admin);
        
        // Chiffrement
        chiffrement_xor(buffer, CLE_CHIFFREMENT);
        fprintf(f, "%s\n", buffer);
        
        courant = courant->suivant;
    }
    fclose(f);
}

Compte* trouver_compte_par_id(int id) {
    verifier_chargement();
    Compte *courant = liste_comptes;
    while (courant != NULL) {
        if (courant->id == id) {
            return courant;
        }
        courant = courant->suivant;
    }
    return NULL;
}

void creer_demande_compte(Compte nouveau_compte) {
    FILE *f = fopen(FICHIER_DEMANDES, "a");
    if (f == NULL) return;

    fprintf(f, "%d,%s,%s,%s,%s,%s,%d,%s,%s,%s,%d,%.2f,%s,%d,%d\n",
            nouveau_compte.id, nouveau_compte.mot_de_passe, nouveau_compte.date_creation, nouveau_compte.nom, 
            nouveau_compte.cin, nouveau_compte.date_naissance, nouveau_compte.age, nouveau_compte.adresse, 
            nouveau_compte.telephone, nouveau_compte.citoyennete, nouveau_compte.type_compte, 
            nouveau_compte.solde, nouveau_compte.devise, nouveau_compte.verrouille, nouveau_compte.role_admin);
    
    fclose(f);
    printf("Demande de creation envoyee a l'admin.\n");
}

int obtenir_prochain_id() {
    verifier_chargement();
    int id_max = 0;
    Compte *courant = liste_comptes;
    while (courant != NULL) {
        if (courant->id > id_max) id_max = courant->id;
        courant = courant->suivant;
    }
    return id_max + 1;
}

void traiter_demandes() {
    FILE *f = fopen(FICHIER_DEMANDES, "r");
    if (f == NULL) {
        printf("Aucune demande en attente.\n");
        return;
    }

    // On utilise un tableau temporaire pour les demandes (pas besoin de liste chainee ici car c'est ephemere)
    Compte demandes[50];
    int nb_demandes = 0;
    char buffer[512];

    while (nb_demandes < 50 && fgets(buffer, sizeof(buffer), f)) {
        Compte *c = &demandes[nb_demandes];
        int r = sscanf(buffer, "%d,%49[^,],%19[^,],%49[^,],%19[^,],%19[^,],%d,%99[^,],%19[^,],%19[^,],%d,%lf,%4[^,],%d,%d",
               &c->id, c->mot_de_passe, c->date_creation, c->nom, c->cin, c->date_naissance,
               &c->age, c->adresse, c->telephone, c->citoyennete,
               &c->type_compte, &c->solde, c->devise, &c->verrouille, &c->role_admin);
        if (r < 15) strcpy(c->devise, "MAD"); // Default for old requests
        nb_demandes++;
    }
    fclose(f);

    if (nb_demandes == 0) {
        printf("Aucune demande en attente.\n");
        return;
    }

    FILE *f_nouveau = fopen(FICHIER_DEMANDES, "w"); 
    
    for (int i = 0; i < nb_demandes; i++) {
        printf("\nDemande %d:\nNom: %s, CIN: %s, Solde initial: %.2f\n", i+1, demandes[i].nom, demandes[i].cin, demandes[i].solde);
        printf("Accepter (1) ou Rejeter (0) ? ");
        int choix;
        scanf("%d", &choix);

        if (choix == 1) {
            verifier_chargement();
            demandes[i].id = obtenir_prochain_id();
            ajouter_compte_liste(&demandes[i]);
            sauvegarder_comptes();
            printf("Compte cree avec ID %d.\n", demandes[i].id);
        } else {
            printf("Demande rejetee.\n");
        }
    }
    fclose(f_nouveau);
}

void supprimer_compte(int id) {
    verifier_chargement();
    Compte *courant = liste_comptes;
    Compte *precedent = NULL;

    while (courant != NULL) {
        if (courant->id == id) {
            // Restriction suppression admin retiree pour maintenance
            
            // Suppression du noeud
            if (precedent == NULL) {
                liste_comptes = courant->suivant;
            } else {
                precedent->suivant = courant->suivant;
            }
            free(courant);
            sauvegarder_comptes();
            supprimer_historique(id); // Nettoyer l'historique
            printf("Compte %d supprime.\n", id);
            return;
        }
        precedent = courant;
        courant = courant->suivant;
    }
    printf("Compte non trouve.\n");
}

Compte* obtenir_tete_liste() {
    verifier_chargement();
    return liste_comptes;
}

StatistiquesBanque calculer_statistiques() {
    verifier_chargement();
    StatistiquesBanque stats = {0, 0, 0, 0.0, 0.0, 0};
    Compte *courant = liste_comptes;
    
    while (courant != NULL) {
        stats.total_comptes++;
        stats.total_fonds += courant->solde;
        
        if (courant->role_admin == 1) {
            stats.total_admins++;
        } else {
            stats.total_clients++;
        }
        
        if (courant->verrouille) {
            stats.comptes_bloques++;
        }
        
        courant = courant->suivant;
    }
    
    if (stats.total_comptes > 0) {
        stats.solde_moyen = stats.total_fonds / stats.total_comptes;
    }
    
    return stats;
}

Compte* trouver_compte_flexible(const char* input) {
    verifier_chargement();
    Compte *courant = liste_comptes;
    
    // Essayer de voir si c'est un ID (nombre)
    char *endptr;
    long id_val = strtol(input, &endptr, 10);
    int is_numeric = (*endptr == '\0');
    
    while (courant != NULL) {
        // 1. Verification par ID
        if (is_numeric && courant->id == (int)id_val) {
            return courant;
        }
        // 2. Verification par CIN (Exact match)
        if (strcmp(courant->cin, input) == 0) {
            return courant;
        }
        // 3. Verification par Nom (Exact match)
        if (strcmp(courant->nom, input) == 0) {
            return courant;
        }
        
        courant = courant->suivant;
    }
    return NULL;
}

void creer_admin_direct(Compte nouveau_admin) {
    verifier_chargement();
    nouveau_admin.id = obtenir_prochain_id();
    nouveau_admin.role_admin = 1;
    nouveau_admin.solde = 0; // Admin n'a pas besoin de solde specifique
    strcpy(nouveau_admin.devise, "MAD");
    nouveau_admin.verrouille = 0;
    
    ajouter_compte_liste(&nouveau_admin);
    sauvegarder_comptes();
    printf("Nouvel Administrateur cree avec succes (ID: %d) !\n", nouveau_admin.id);
}

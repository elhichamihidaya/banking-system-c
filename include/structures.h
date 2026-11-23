#ifndef STRUCTURES_H
#define STRUCTURES_H

// Constantes
// Constantes
#define MAX_COMPTES 100
#define MAX_HISTORIQUE 1000

// Structure pour un Compte
typedef struct Compte {
    int id;
    char mot_de_passe[50];
    char date_creation[20];
    char nom[50];
    char cin[20];
    char date_naissance[20];
    int age;
    char adresse[100];
    char telephone[20];
    char citoyennete[20];
    int type_compte; // 0: Courant, 1: Epargne, etc.
    double solde;
    char devise[5]; // Ex: "MAD", "EUR", "USD"
    int verrouille;      // 0: Actif, 1: Verrouille
    int role_admin;        // 0: Client, 1: Admin
    struct Compte *suivant; // Pointeur pour la liste chainee
} Compte;

// Structure pour l'Historique
typedef struct {
    int id_compte;
    char type_operation[20]; // "depot", "retrait", "transfert_de", "transfert_vers"
    double montant;
    char date[30];
    int id_destinataire;     // ID de l'autre compte (si transfert), sinon 0
} Historique;

// Structure pour les statistiques
typedef struct {
    int total_comptes;
    int total_clients;
    int total_admins;
    double total_fonds;
    double solde_moyen;
    int comptes_bloques;
} StatistiquesBanque;

#endif

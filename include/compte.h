#ifndef COMPTE_H
#define COMPTE_H

#include "structures.h"

// Constantes pour les noms de fichiers
extern const char* FICHIER_COMPTES;
extern const char* FICHIER_DEMANDES;

// Fonctions principales pour gerer les comptes
Compte* trouver_compte_par_id(int id);
void sauvegarder_comptes();
int charger_comptes(); // Retourne le nombre de comptes charges

// Fonctions pour l'administrateur
void creer_demande_compte(Compte nouveau_compte);
void traiter_demandes(); // Interface admin pour accepter/refuser
void supprimer_compte(int id);

// Fonction utilitaire
int obtenir_prochain_id();
void ajouter_compte_liste(Compte *nouveau_compte);
void liberer_liste();

// Nouvelles fonctions
Compte* obtenir_tete_liste();
StatistiquesBanque calculer_statistiques();


Compte* trouver_compte_flexible(const char* input);
void creer_admin_direct(Compte nouveau_admin);

#endif


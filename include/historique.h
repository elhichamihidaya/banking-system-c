#ifndef HISTORIQUE_H
#define HISTORIQUE_H

#include "structures.h"

void ajouter_historique(int id_compte, const char *type, double montant, int id_destinataire);
void afficher_historique(int id_compte);
int charger_historique(const char *nom_fichier, Historique tab[], int max);
void sauvegarder_historique(const char *nom_fichier, Historique tab[], int n);
void supprimer_historique(int id_compte); // Nouvelle fonction

#endif

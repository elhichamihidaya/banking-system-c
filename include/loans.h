#ifndef LOANS_H
#define LOANS_H

#include "structures.h"

typedef struct Credit {
    int id;
    int id_compte;
    double montant;
    double restant;
    int statut; // 0: En attente, 1: Approuve, 2: Rejete, 3: Paye
    struct Credit *suivant;
} Credit;

void demander_credit(int id_compte, double montant);
void gerer_credits(); // Fonction admin
void payer_credit(int id_compte, double montant);
void charger_credits();
void sauvegarder_credits();
void afficher_mes_credits(int id_compte);

#endif

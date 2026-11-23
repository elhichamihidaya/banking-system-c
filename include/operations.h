#ifndef OPERATIONS_H
#define OPERATIONS_H

// Verifie si le montant est positif
int verifier_montant(double montant);

// Deposer de l'argent sur le compte
int deposer_argent(int id_compte, double montant);

// Retirer de l'argent du compte
int retirer_argent(int id_compte, double montant);

// Virer de l'argent entre comptes
int faire_virement(int id_expediteur, int id_destinataire, double montant);

#endif

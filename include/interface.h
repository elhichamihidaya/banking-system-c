#ifndef INTERFACE_H
#define INTERFACE_H

#include "structures.h"

// Couleurs ANSI
#define COULEUR_RESET   "\033[0m"
#define COULEUR_ROUGE     "\033[1;31m"
#define COULEUR_VERT   "\033[1;32m"
#define COULEUR_JAUNE  "\033[1;33m"
#define COULEUR_BLEU    "\033[1;34m"
#define COULEUR_CYAN    "\033[1;36m"
#define COULEUR_MAGENTA "\033[1;35m"
#define COULEUR_BLANC   "\033[1;37m"
#define FOND_BLEU       "\033[44m"

void afficher_succes(const char* msg);
void afficher_erreur(const char* msg);
void afficher_attention(const char* msg);
void afficher_info(const char* msg);
void afficher_titre(const char* titre);
void afficher_centre(const char* texte);
void afficher_entete_variante(const char* couleur, const char* sous_titre);
void afficher_statistiques_banque(StatistiquesBanque stats);
void exporter_statistiques(StatistiquesBanque stats);
void afficher_tableau_comptes();
void afficher_boite_options(const char* options[], int nb_options, const char* couleur);
void afficher_boite_simple(const char* texte, const char* couleur);

void menu_principal();
void menu_client(Compte *c);
void menu_admin(Compte *admin);
void ecran_connexion();
void ecran_inscription();
void menu_recherche_compte();

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/compte.h"
#include "../include/interface.h"

int main() {
    // Definir la taille de la fenetre
    system("mode con: cols=120 lines=40");

    // Chargement des donnees
    int n = charger_comptes();
    
    // Creation automatique de l'admin si le systeme est vide
    if (n == 0) {
        printf("Systeme vide. Creation du compte Admin par defaut...\n");
        Compte admin;
        admin.id = 1;
        strcpy(admin.mot_de_passe, "admin");
        strcpy(admin.date_creation, "2023-01-01");
        strcpy(admin.nom, "Administrateur");
        strcpy(admin.cin, "ADMIN001");
        strcpy(admin.date_naissance, "2000-01-01");
        admin.age = 30;
        strcpy(admin.adresse, "Banque Centrale");
        strcpy(admin.telephone, "0000000000");
        strcpy(admin.citoyennete, "N/A");
        admin.type_compte = 0;
        admin.solde = 1000000.00;
        admin.verrouille = 0;
        admin.role_admin = 1; // ADMIN
        strcpy(admin.devise, "MAD");

        // On ajoute via la fonction securisee
        ajouter_compte_liste(&admin);
        sauvegarder_comptes();
        n = 1; // On a maintenant 1 compte
        printf("Compte Admin cree: ID=1, Password=admin\n");
    }

    printf("Systeme initialise. %d comptes charges.\n", n);
    
    // Lancement de l'interface
    menu_principal();
    
    return 0;
}

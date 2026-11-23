#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/interface.h"
#include "../include/compte.h"
#include "../include/operations.h"
#include "../include/historique.h"
#include "../include/loans.h"

// --- Helper Functions for Validation ---
int est_bissextile(int annee) {
    return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
}

int valider_format_date(const char *date) {
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;
    
    int annee, mois, jour;
    if (sscanf(date, "%d-%d-%d", &annee, &mois, &jour) != 3) return 0;
    
    if (annee < 1900 || annee > 2100) return 0;
    if (mois < 1 || mois > 12) return 0;
    
    int jours_par_mois[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (est_bissextile(annee)) jours_par_mois[2] = 29;
    
    if (jour < 1 || jour > jours_par_mois[mois]) return 0;
    
    return 1;
}

int extraire_annee(const char *date) {
    int annee;
    sscanf(date, "%d", &annee);
    return annee;
}


// --- Fonctions d'aide pour l'affichage ---

void effacer_ecran() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int obtenir_longueur_visible(const char* texte) {
    int len = 0;
    int dans_echappement = 0;
    for (int i = 0; texte[i] != '\0'; i++) {
        if (texte[i] == '\033') {
            dans_echappement = 1;
        } else if (dans_echappement && texte[i] == 'm') {
            dans_echappement = 0;
        } else if (!dans_echappement) {
            len++;
        }
    }
    return len;
}

void afficher_centre(const char* texte) {
    int largeur = 80;
    int len = obtenir_longueur_visible(texte);
    int marge = (largeur - len) / 2;
    if (marge < 0) marge = 0;
    
    for (int i = 0; i < marge; i++) printf(" ");
    printf("%s\n", texte);
}

void afficher_ligne() {
    int largeur = 50;
    int marge = (80 - largeur) / 2;
    for (int i = 0; i < marge; i++) printf(" ");
    printf(COULEUR_BLEU);
    for (int i = 0; i < largeur; i++) printf("=");
    printf(COULEUR_RESET "\n");
}

void afficher_succes(const char* msg) {
    printf(COULEUR_VERT "[SUCCES] %s" COULEUR_RESET "\n", msg);
}

void afficher_erreur(const char* msg) {
    printf(COULEUR_ROUGE "[ERREUR] %s" COULEUR_RESET "\n", msg);
}

void afficher_attention(const char* msg) {
    printf(COULEUR_JAUNE "[ATTENTION] %s" COULEUR_RESET "\n", msg);
}

void afficher_info(const char* msg) {
    printf(COULEUR_CYAN "[INFO] %s" COULEUR_RESET "\n", msg);
}

void afficher_titre(const char* titre) {
    printf(COULEUR_MAGENTA "\n=== %s ===" COULEUR_RESET "\n", titre);
}

void afficher_entete() {
    effacer_ecran();
    printf(COULEUR_CYAN "\n");
    afficher_centre("  ____    _    _   _ _  _______ _     _____  __        __ ");
    afficher_centre(" | __ )  / \\  | \\ | | |/ /  ___| |   / _ \\ \\      / / ");
    afficher_centre(" |  _ \\ / _ \\ |  \\| | ' /| |_  | |  | | | \\ \\ /\\ / /  ");
    afficher_centre(" | |_) / ___ \\| |\\  | . \\|  _| | |__| |_| |\\ V  V /   ");
    afficher_centre(" |____/_/   \\_\\_| \\_|_|\\_\\_|   |_____\\___/  \\_/\\_/    ");
    printf(COULEUR_RESET "\n");
    afficher_centre(COULEUR_JAUNE "\033[1mSYSTEME DE GESTION BANCAIRE\033[0m" COULEUR_RESET);
    afficher_ligne();
    printf("\n");
}

void afficher_entete_variante(const char* couleur, const char* sous_titre) {
    effacer_ecran();
    printf("%s\n", couleur);
    afficher_centre("  ____    _    _   _ _  _______ _     _____  __        __ ");
    afficher_centre(" | __ )  / \\  | \\ | | |/ /  ___| |   / _ \\ \\      / / ");
    afficher_centre(" |  _ \\ / _ \\ |  \\| | ' /| |_  | |  | | | \\ \\ /\\ / /  ");
    afficher_centre(" | |_) / ___ \\| |\\  | . \\|  _| | |__| |_| |\\ V  V /   ");
    afficher_centre(" |____/_/   \\_\\_| \\_|_|\\_\\_|   |_____\\___/  \\_/\\_/    ");
    printf(COULEUR_RESET "\n");
    
    char buffer[100];
    sprintf(buffer, "\033[1m%s\033[0m", sous_titre);
    afficher_centre(buffer); 
    
    printf("%s", couleur);
    afficher_ligne();
    printf(COULEUR_RESET "\n\n"); 
}

// --- MENUS ---


void afficher_statistiques_banque(StatistiquesBanque stats) {
    afficher_entete();
    afficher_centre(COULEUR_MAGENTA "STATISTIQUES DE LA BANQUE" COULEUR_RESET);
    printf("\n");
    
    printf("  %s+----------------------------+----------------------------+%s\n", COULEUR_BLEU, COULEUR_RESET);
    printf("  %s| %-26s | %26d |%s\n", COULEUR_BLEU, "Nombre total de comptes", stats.total_comptes, COULEUR_RESET);
    printf("  %s+----------------------------+----------------------------+%s\n", COULEUR_BLEU, COULEUR_RESET);
    printf("  %s| %-26s | %26d |%s\n", COULEUR_BLEU, "Clients actifs", stats.total_clients, COULEUR_RESET);
    printf("  %s| %-26s | %26d |%s\n", COULEUR_BLEU, "Administrateurs", stats.total_admins, COULEUR_RESET);
    printf("  %s| %-26s | %26d |%s\n", COULEUR_BLEU, "Comptes verrouilles", stats.comptes_bloques, COULEUR_RESET);
    printf("  %s+----------------------------+----------------------------+%s\n", COULEUR_BLEU, COULEUR_RESET);
    printf("  %s| %-26s | %23.2f %s |%s\n", COULEUR_BLEU, "Fonds totaux (Valeur Num)", stats.total_fonds, "(Mixte)", COULEUR_RESET);
    printf("  %s| %-26s | %23.2f %s |%s\n", COULEUR_BLEU, "Solde moyen (Valeur Num)", stats.solde_moyen, "(Mixte)", COULEUR_RESET);
    printf("  %s+----------------------------+----------------------------+%s\n", COULEUR_BLEU, COULEUR_RESET);
    
    printf("\n");
    afficher_info("Appuyez sur Entree pour revenir...");
    getchar(); getchar();
}

void exporter_statistiques(StatistiquesBanque stats) {
    FILE *f = fopen("../data/statistiques.txt", "w");
    if (f == NULL) {
        afficher_erreur("Impossible de creer le fichier statistiques.txt");
        return;
    }
    
    fprintf(f, "=== STATISTIQUES DE LA BANQUE ===\n\n");
    fprintf(f, "Nombre total de comptes : %d\n", stats.total_comptes);
    fprintf(f, "Clients actifs          : %d\n", stats.total_clients);
    fprintf(f, "Administrateurs         : %d\n", stats.total_admins);
    fprintf(f, "Comptes verrouilles     : %d\n", stats.comptes_bloques);
    fprintf(f, "Fonds totaux            : %.2f (Mixte)\n", stats.total_fonds);
    fprintf(f, "Solde moyen             : %.2f (Mixte)\n", stats.solde_moyen);
    
    fclose(f);
    afficher_succes("Statistiques exportees vers 'statistiques.txt' !");
    getchar(); getchar();
}

void afficher_tableau_comptes() {
    afficher_entete();
    afficher_centre(COULEUR_CYAN "LISTE DES COMPTES" COULEUR_RESET);
    printf("\n");
    
    Compte *courant = obtenir_tete_liste();
    
    if (courant == NULL) {
        afficher_attention("Aucun compte trouve.");
        printf("\n");
        afficher_info("Appuyez sur Entree pour revenir...");
        getchar(); getchar();
        return;
    }

    // --- Section ADMINISTRATEURS ---
    printf(COULEUR_ROUGE "\n [ ADMINISTRATEURS ]\n" COULEUR_RESET);
    printf(" %s+-----+----------------------+-----------------+--------------------+-----------+%s\n", COULEUR_ROUGE, COULEUR_RESET);
    printf(" %s| %-3s | %-20s | %-15s | %-18s | %-9s |%s\n", COULEUR_ROUGE, "ID", "NOM COMPLET", "CIN", "SOLDE", "ROLE", COULEUR_RESET);
    printf(" %s+-----+----------------------+-----------------+--------------------+-----------+%s\n", COULEUR_ROUGE, COULEUR_RESET);

    courant = obtenir_tete_liste();
    while (courant != NULL) {
        if (courant->role_admin == 1) {
            printf(" %s|%s %-3d %s|%s %-20s %s|%s %-15s %s|%s %14.2f %-3s %s|%s %-9s %s|%s\n", 
                COULEUR_ROUGE, COULEUR_RESET, courant->id, 
                COULEUR_ROUGE, COULEUR_RESET, courant->nom, 
                COULEUR_ROUGE, COULEUR_RESET, courant->cin, 
                COULEUR_ROUGE, COULEUR_RESET, courant->solde, courant->devise, 
                COULEUR_ROUGE, COULEUR_RESET, "ADMIN",
                COULEUR_ROUGE, COULEUR_RESET);
        }
        courant = courant->suivant;
    }
    printf(" %s+-----+----------------------+-----------------+-----------------+-----------+%s\n", COULEUR_ROUGE, COULEUR_RESET);


    // --- Section CLIENTS ---
    printf(COULEUR_BLEU "\n [ CLIENTS ]\n" COULEUR_RESET);
    printf(" %s+-----+----------------------+-----------------+--------------------+-----------+%s\n", COULEUR_BLEU, COULEUR_RESET);
    printf(" %s| %-3s | %-20s | %-15s | %-18s | %-9s |%s\n", COULEUR_BLEU, "ID", "NOM COMPLET", "CIN", "SOLDE", "ROLE", COULEUR_RESET);
    printf(" %s+-----+----------------------+-----------------+--------------------+-----------+%s\n", COULEUR_BLEU, COULEUR_RESET);

    courant = obtenir_tete_liste();
    while (courant != NULL) {
        if (courant->role_admin == 0) {
            printf(" %s|%s %-3d %s|%s %-20s %s|%s %-15s %s|%s %14.2f %-3s %s|%s %-9s %s|%s\n", 
                COULEUR_BLEU, COULEUR_RESET, courant->id, 
                COULEUR_BLEU, COULEUR_RESET, courant->nom, 
                COULEUR_BLEU, COULEUR_RESET, courant->cin, 
                COULEUR_BLEU, COULEUR_RESET, courant->solde, courant->devise, 
                COULEUR_BLEU, COULEUR_RESET, "CLIENT",
                COULEUR_BLEU, COULEUR_RESET);
        }
        courant = courant->suivant;
    }
    printf(" %s+-----+----------------------+-----------------+-----------------+-----------+%s\n", COULEUR_BLEU, COULEUR_RESET);

    printf("\n");
    afficher_info("Appuyez sur Entree pour revenir...");
    getchar(); getchar();
}


void afficher_aide() {
    effacer_ecran();
    printf(COULEUR_CYAN "\n");
    afficher_centre("  ____    _    _   _ _  _______ _     _____  __        __");
    afficher_centre(" | __ )  / \\  | \\ | | |/ /  ___| |   / _ \\ \\      / /");
    afficher_centre(" |  _ \\ / _ \\ |  \\| | ' /| |_  | |  | | | \\ \\ /\\ / / ");
    afficher_centre(" | |_) / ___ \\| |\\  | . \\|  _| | |__| |_| |\\ V  V /  ");
    afficher_centre(" |____/_/   \\_\\_| \\_|_|\\_\\_|   |_____\\___/  \\_/\\_/   ");
    printf(COULEUR_RESET "\n");
    
    afficher_centre(COULEUR_JAUNE "\033[1mA PROPOS DE BANKFLOW\033[0m" COULEUR_RESET);
    afficher_ligne();
    printf("\n");
    
    printf("   %sBankFlow%s est une solution bancaire moderne concue pour simplifier\n", COULEUR_CYAN, COULEUR_RESET);
    printf("   vos transactions quotidiennes avec une securite de pointe.\n\n");
    printf("   %sDeveloppe exclusivement par :%s Mohamed Reda EL MORABIT\n\n", COULEUR_MAGENTA, COULEUR_RESET);
    
    printf("   %sNos engagements :%s\n", COULEUR_VERT, COULEUR_RESET);
    printf("    * Securite et Confidentialite\n");
    printf("    * Transparence totale\n");
    printf("    * Service client disponible 24/7\n\n");
    
    printf("   %sBESOIN D'AIDE ?%s\n", COULEUR_ROUGE, COULEUR_RESET);
    printf("   ----------------\n");
    printf("   Pour toute assistance technique ou question sur votre compte :\n");
    printf("   %s Telephone :%s +212 5 22 00 00 00\n", COULEUR_BLEU, COULEUR_RESET);
    printf("   %s Email     :%s support@bankflow.ma\n", COULEUR_BLEU, COULEUR_RESET);
    printf("   %s Adresse   :%s AL HOCEIMA , RUE MOHAMED V , 32000\n\n", COULEUR_BLEU, COULEUR_RESET);

    afficher_ligne();
    printf("\n");
    afficher_info("Appuyez sur Entree pour revenir au menu principal...");
    getchar(); getchar();
}

int obtenir_longueur_visible(const char* texte); // Forward decl si besoin, mais deja defini plus haut

void afficher_boite_options(const char* options[], int nb_options, const char* couleur) {
    int largeur_max = 0;
    // Calculer la largeur max requise
    for (int i = 0; i < nb_options; i++) {
        int len = obtenir_longueur_visible(options[i]);
        if (len > largeur_max) largeur_max = len;
    }
    
    // On ajoute du padding (ex: 4 espaces de chaque cote)
    int largeur_totale = largeur_max + 8; // 4 padding left + 4 padding right
    int padding_ecran = (80 - largeur_totale) / 2;
    if (padding_ecran < 0) padding_ecran = 0;

    // Ligne superieure
    printf("%s", couleur);
    for(int i=0; i<padding_ecran; i++) printf(" ");
    printf("+");
    for(int i=0; i<largeur_totale; i++) printf("-");
    printf("+%s\n", COULEUR_RESET);

    // Options
    for (int i = 0; i < nb_options; i++) {
        printf("%s", couleur);
        for(int k=0; k<padding_ecran; k++) printf(" ");
        printf("|");
        
        int len_opt = obtenir_longueur_visible(options[i]);
        int espace_avant = (largeur_totale - len_opt) / 2;
        int espace_apres = largeur_totale - len_opt - espace_avant;
        
        for(int k=0; k<espace_avant; k++) printf(" ");
        printf("%s%s%s", COULEUR_RESET "\033[1m", options[i], couleur); // Reset pour le texte, puis remet couleur bordure
        for(int k=0; k<espace_apres; k++) printf(" ");
        
        printf("|%s\n", COULEUR_RESET);
    }

    // Ligne inferieure
    printf("%s", couleur);
    for(int i=0; i<padding_ecran; i++) printf(" ");
    printf("+");
    for(int i=0; i<largeur_totale; i++) printf("-");
    printf("+%s\n", COULEUR_RESET);
}

void afficher_boite_simple(const char* texte, const char* couleur) {
    const char* options[] = { texte };
    afficher_boite_options(options, 1, couleur);
}

void menu_principal() {
    int choix;
    
    const char* options_principales[] = {
        COULEUR_VERT "1.    Connexion    " COULEUR_RESET,
        COULEUR_BLEU "2. Ouvrir un Compte" COULEUR_RESET,
        COULEUR_MAGENTA "3.  A propos / Aide" COULEUR_RESET
    };

    while (1) {
        afficher_entete();
        afficher_centre(COULEUR_BLANC "\033[1mMENU PRINCIPAL\033[0m" COULEUR_RESET);
        printf("\n");
        
        // Affichage des options principales groupees
        afficher_boite_options(options_principales, 3, COULEUR_CYAN);
        
        printf("\n");
        
        // Affichage de l'option Quitter isolée
        afficher_boite_simple("4.      Quitter     ", COULEUR_ROUGE);

        printf("\n");
        afficher_ligne();
        printf(COULEUR_JAUNE " Votre choix: " COULEUR_RESET);
        
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n'); // On vide le buffer
            continue;
        }

        switch (choix) {
            case 1: ecran_connexion(); break;
            case 2: ecran_inscription(); break;
            case 3: afficher_aide(); break;
            case 4: exit(0);
            default: break;
        }
    }
}

void ecran_inscription() {
    Compte c;
    c.id = 0; 
    c.solde = 0.0;
    c.verrouille = 0;
    c.role_admin = 0; 
    
    afficher_entete();
    afficher_centre("INSCRIPTION");
    printf("\n");
    
    do {
        printf(" Nom complet: ");
        scanf(" %[^\n]", c.nom);
        int valide = 1;
        for (int i = 0; c.nom[i] != '\0'; i++) {
            if (!isalpha((unsigned char)c.nom[i]) && c.nom[i] != ' ') {
                valide = 0;
                break;
            }
        }
        if (!valide) {
            afficher_erreur("Le nom ne doit contenir que des lettres et des espaces.");
        } else {
            break;
        }
    } while (1);
    printf(" CIN: ");
    scanf("%s", c.cin);
    printf(" Mot de passe: ");
    scanf("%s", c.mot_de_passe);

    // Validation Date Naissance
    do {
        printf(" Date Naissance (YYYY-MM-DD): ");
        scanf("%s", c.date_naissance);
        if (!valider_format_date(c.date_naissance)) {
            afficher_erreur("Format invalide ou date inexistante (ex: 2000-01-31).");
        } else {
            break;
        }
    } while (1);

    // Validation Age et coherence
    do {
        printf(" Age: ");
        if (scanf("%d", &c.age) != 1) {
            while(getchar() != '\n'); 
            c.age = -1;
        }
        
        // Coherence avec annee de naissance
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        int annee_courante = tm.tm_year + 1900;
        int annee_naissance = extraire_annee(c.date_naissance);
        int age_calcule = annee_courante - annee_naissance;
        
        // On tolere une marge de 1 an (selon si l'anniversaire est passe ou non)
        if (c.age < 0 || c.age > 120) {
            afficher_erreur("Age invalide.");
        } else if (abs(c.age - age_calcule) > 1) {
             printf(COULEUR_JAUNE "[ATTENTION] L'age saisi (%d) ne correspond pas a l'annee de naissance (%d).\n" COULEUR_RESET, c.age, annee_naissance);
             printf("Age attendu: environ %d ans.\n", age_calcule);
             afficher_erreur("Veuillez ressaisir un age coherent.");
        } else {
            break;
        }
    } while (1);

    printf(" Adresse: ");
    scanf(" %[^\n]", c.adresse);
    // Validation Telephone
    do {
        printf(" Telephone: ");
        scanf("%s", c.telephone);
        int valid_tel = 1;
        for (int i = 0; c.telephone[i] != '\0'; i++) {
            if (!isdigit((unsigned char)c.telephone[i])) {
                valid_tel = 0;
                break;
            }
        }
        
        if (!valid_tel) {
            afficher_erreur("Le numero de telephone ne doit contenir que des chiffres.");
        } else if (strlen(c.telephone) < 8 || strlen(c.telephone) > 15) {
             afficher_erreur("Longueur invalide (entre 8 et 15 chiffres).");
        } else {
            break;
        }
    } while (1);
    printf(" Citoyennete: ");
    scanf("%s", c.citoyennete);
    int choix_type;
    do {
        printf(" Type Compte (1=Courant, 2=Epargne): ");
        if (scanf("%d", &choix_type) != 1) {
            while(getchar() != '\n'); // Vider buffer
            choix_type = 0; // Invalide
        }
        if (choix_type != 1 && choix_type != 2) {
            afficher_erreur("Veuillez choisir 1 ou 2.");
        }
    } while (choix_type != 1 && choix_type != 2);

    c.type_compte = choix_type - 1;

    // Choix Devise
    int choix_devise;
    do {
        printf(" Devise (1=MAD, 2=EUR, 3=USD): ");
        if (scanf("%d", &choix_devise) != 1) {
            while(getchar() != '\n');
            choix_devise = 0;
        }
        if (choix_devise < 1 || choix_devise > 3) {
            afficher_erreur("Veuillez choisir 1, 2 ou 3.");
        }
    } while (choix_devise < 1 || choix_devise > 3);
    
    if (choix_devise == 1) strcpy(c.devise, "MAD");
    else if (choix_devise == 2) strcpy(c.devise, "EUR");
    else strcpy(c.devise, "USD");
    
    strcpy(c.date_creation, "2023-01-01"); 

    creer_demande_compte(c);
    
    printf("\n");
    afficher_succes("Demande envoyee avec succes !");
    afficher_info("Appuyez sur Entree pour revenir...");
    getchar(); getchar();
}

void ecran_connexion() {
    // int id; // Removed unused variable
    char mot_de_passe[50];
    
    afficher_entete();
    afficher_centre("CONNEXION");
    printf("\n");
    
    printf(" Identifiant (ID, CIN ou Nom): ");
    char input_id[100];
    scanf(" %[^\n]", input_id);
    printf(" Mot de passe: ");
    scanf("%s", mot_de_passe);
    
    Compte *c = trouver_compte_flexible(input_id);
    
    if (c != NULL && strcmp(c->mot_de_passe, mot_de_passe) == 0) {
        if (c->verrouille) {
            printf("\n");
            afficher_erreur("COMPTE VERROUILLE !");
            getchar(); getchar();
            return;
        }
        
        if (c->role_admin == 1) {
            menu_admin(c);
        } else {
            menu_client(c);
        }
    } else {
        printf("\n");
        afficher_erreur("Identifiants incorrects !");
        getchar(); getchar();
    }
}

void menu_client(Compte *c) {
    int choix;
    while (1) {
        afficher_entete_variante(COULEUR_VERT, "ESPACE CLIENT");
        char buffer[100];
        sprintf(buffer, "Bienvenue, %s", c->nom);
        afficher_centre(buffer);
        sprintf(buffer, "Solde: %.2f %s", c->solde, c->devise);
        afficher_centre(buffer);
        sprintf(buffer, "Type: %s | Nationalite: %s", (c->type_compte == 0 ? "COURANT" : "EPARGNE"), c->citoyennete);
        afficher_centre(buffer);
        printf("\n");
        
        afficher_centre(COULEUR_VERT "\033[1m1. Depot\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_VERT "\033[1m2. Retrait\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_VERT "\033[1m3. Virement\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_BLEU "\033[1m4. Historique\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_MAGENTA "\033[1m5. Credits\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_ROUGE "\033[1m6. Deconnexion\033[0m" COULEUR_RESET);
        printf("\n");
        afficher_ligne();
        printf(COULEUR_JAUNE " Votre choix: " COULEUR_RESET);
        
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n');
            continue;
        }
        
        double montant;
        int destId;
        
        switch (choix) {
            case 1:
                printf("\n Montant a deposer: ");
                scanf("%lf", &montant);
                if (deposer_argent(c->id, montant)) {
                    char msg[100];
                    sprintf(msg, "Succes! Nouveau solde: %.2f", c->solde);
                    afficher_succes(msg);
                }
                getchar(); getchar();
                break;
            case 2:
                printf("\n Montant a retirer: ");
                scanf("%lf", &montant);
                if (retirer_argent(c->id, montant)) {
                    char msg[100];
                    sprintf(msg, "Succes! Nouveau solde: %.2f", c->solde);
                    afficher_succes(msg);
                }
                getchar(); getchar();
                break;
            case 3:
                printf("\n ID Destinataire: ");
                scanf("%d", &destId);
                printf(" Montant: ");
                scanf("%lf", &montant);
                if (faire_virement(c->id, destId, montant)) {
                    char msg[100];
                    sprintf(msg, "Succes! Nouveau solde: %.2f", c->solde);
                    afficher_succes(msg);
                }
                getchar(); getchar();
                break;
            case 4:
                afficher_historique(c->id);
                printf("\n Appuyez sur Entree...");
                getchar(); getchar();
                break;
            case 5:
                afficher_mes_credits(c->id);
                printf("\n1. Demander un credit\n2. Payer un credit\n3. Retour\nChoix: ");
                int lChoice;
                scanf("%d", &lChoice);
                if (lChoice == 1) {
                    printf("Montant: ");
                    scanf("%lf", &montant);
                    demander_credit(c->id, montant);
                } else if (lChoice == 2) {
                    printf("Montant a payer: ");
                    scanf("%lf", &montant);
                    payer_credit(c->id, montant);
                }
                getchar(); getchar();
                break;
            case 6:
                return;
            default:
                break;
        }
    }
}

// ... existing code ...

void menu_recherche_compte() {
    char input[100];
    afficher_entete();
    afficher_centre(COULEUR_CYAN "RECHERCHE DE COMPTE" COULEUR_RESET);
    printf("\n");
    
    printf(" Entrez l'ID, le Nom ou le CIN : ");
    scanf(" %[^\n]", input);
    
    Compte *c = trouver_compte_flexible(input);
    
    if (c == NULL) {
        printf("\n");
        afficher_erreur("Aucun compte trouve correspondant a votre recherche.");
    } else {
        printf("\n");
        afficher_centre(COULEUR_VERT "Compte Trouve !" COULEUR_RESET);
        printf("\n");
        
        const char* color = (c->role_admin == 1) ? COULEUR_ROUGE : COULEUR_BLEU;
        const char* role = (c->role_admin == 1) ? "ADMIN" : "CLIENT";
        
        printf(" %s+-----+----------------------+-----------------+-----------------+-----------+%s\n", color, COULEUR_RESET);
        printf(" %s| %-3s | %-20s | %-15s | %-15s | %-9s |%s\n", color, "ID", "NOM COMPLET", "CIN", "SOLDE", "ROLE", COULEUR_RESET);
        printf(" %s+-----+----------------------+-----------------+-----------------+-----------+%s\n", color, COULEUR_RESET);
        
        printf(" %s|%s %-3d %s|%s %-20s %s|%s %-15s %s|%s %14.2f %-3s %s|%s %-9s %s|%s\n", 
            color, COULEUR_RESET, c->id, 
            color, COULEUR_RESET, c->nom, 
            color, COULEUR_RESET, c->cin, 
            color, COULEUR_RESET, c->solde, c->devise, 
            color, COULEUR_RESET, role,
            color, COULEUR_RESET);
            
        printf(" %s+-----+----------------------+-----------------+-----------------+-----------+%s\n", color, COULEUR_RESET);
        
        // Detail supplementaire
        printf("\n %sDetails supplementaires:%s\n", color, COULEUR_RESET);
        printf("  - Adresse      : %s\n", c->adresse);
        printf("  - Telephone    : %s\n", c->telephone);
        printf("  - Age          : %d ans\n", c->age);
        printf("  - Date Creation: %s\n", c->date_creation);
        printf("  - Statut       : %s\n", c->verrouille ? "VERROUILLE" : "ACTIF");
    }
    
    printf("\n");
    afficher_info("Appuyez sur Entree pour revenir...");
    getchar(); getchar();
}

void menu_admin(Compte *admin) {
    int choix;
    while (1) {
        afficher_entete_variante(COULEUR_ROUGE, "ESPACE ADMINISTRATEUR");
        printf("\n");
        
        afficher_centre(COULEUR_BLEU "\033[1m1. Traiter les demandes\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_ROUGE "\033[1m2. Supprimer un compte\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_CYAN "\033[1m3. Liste des comptes\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_MAGENTA "\033[1m4. Gerer les credits\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_VERT "\033[1m5. Statistiques Banque\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_CYAN "\033[1m6. Ajouter un Admin\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_JAUNE "\033[1m7. Rechercher un compte\033[0m" COULEUR_RESET);
        afficher_centre(COULEUR_ROUGE "\033[1m8. Deconnexion\033[0m" COULEUR_RESET);
        printf("\n");
        afficher_ligne();
        printf(COULEUR_JAUNE " Votre choix: " COULEUR_RESET);
        
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n');
            continue;
        }
        
        int idSupp;
        
        switch (choix) {
            case 1:
                traiter_demandes();
                printf("\n Appuyez sur Entree...");
                getchar(); getchar();
                break;
            case 2:
                printf("\n ID du compte a supprimer: ");
                scanf("%d", &idSupp);
                
                // Debut modification: Confirmation
                Compte *trash = trouver_compte_par_id(idSupp);
                if (trash == NULL) {
                    afficher_erreur("Compte introuvable !");
                } else {
                    printf("\n");
                    afficher_attention("ETES-VOUS SUR DE VOULOIR SUPPRIMER CE COMPTE ?");
                    printf("  - ID    : %d\n", trash->id);
                    printf("  - Nom   : %s\n", trash->nom);
                    printf("  - CIN   : %s\n", trash->cin);
                    printf("  - Solde : %.2f %s\n", trash->solde, trash->devise);
                    if (trash->role_admin == 1) {
                        printf(COULEUR_ROUGE "  - ROLE  : ADMINISTRATEUR (Attention !)\n" COULEUR_RESET);
                    } else {
                        printf("  - Role  : Client\n");
                    }
                    
                    printf("\nConfirmez-vous ? (1=OUI, 0=NON): ");
                    int confirm;
                    scanf("%d", &confirm);
                    
                    if (confirm == 1) {
                         supprimer_compte(idSupp);
                    } else {
                        afficher_info("Suppression annulee.");
                    }
                }
                // Fin modification
                
                getchar(); getchar();
                break;
            case 3:
                afficher_tableau_comptes();
                break;
            case 4:
                gerer_credits();
                printf("\n Appuyez sur Entree...");
                getchar(); getchar();
                break;
            case 5:
                afficher_statistiques_banque(calculer_statistiques());
                printf("\n Voulez-vous exporter ces statistiques ? (1=Oui, 0=Non): ");
                int choixExport;
                scanf("%d", &choixExport);
                if (choixExport == 1) {
                    exporter_statistiques(calculer_statistiques());
                }
                break;
            case 6:
                {
                    Compte newAdmin;
                    char buffer[100];
                    printf("\n=== Creation Nouvel Admin ===\n");
                    do {
                        printf("Nom de l'admin: ");
                        scanf(" %[^\n]", newAdmin.nom);
                        int valide = 1;
                        for (int i = 0; newAdmin.nom[i] != '\0'; i++) {
                            if (!isalpha((unsigned char)newAdmin.nom[i]) && newAdmin.nom[i] != ' ') {
                                valide = 0;
                                break;
                            }
                        }
                        if (!valide) {
                            afficher_erreur("Le nom ne doit contenir que des lettres et des espaces.");
                        } else {
                            break;
                        }
                    } while (1);
                    printf("CIN: ");
                    scanf("%s", newAdmin.cin);
                    printf("Mot de passe: ");
                    scanf("%s", newAdmin.mot_de_passe);
                    
                    // Valeurs par defaut
                    strcpy(newAdmin.date_creation, "2023-01-01");
                    strcpy(newAdmin.date_naissance, "2000-01-01");
                    newAdmin.age = 30;
                    strcpy(newAdmin.adresse, "Banque Admin");
                    strcpy(newAdmin.telephone, "0000000000");
                    strcpy(newAdmin.citoyennete, "N/A");
                    newAdmin.type_compte = 0;
                    
                    creer_admin_direct(newAdmin);
                    printf("\nAppuyez sur Entree...");
                    getchar(); getchar();
                }
                break;
            case 7:
                menu_recherche_compte();
                break;
            case 8:
                return;
            default:
                break;
        }
    }
}

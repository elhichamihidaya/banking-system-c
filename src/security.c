#include <string.h>
#include "../include/security.h"

void chiffrement_xor(char *donnees, const char *cle) {
    int longueur_donnees = strlen(donnees);
    int longueur_cle = strlen(cle);
    
    for (int i = 0; i < longueur_donnees; i++) {
        donnees[i] = donnees[i] ^ cle[i % longueur_cle];
    }
}

//Protection contre les inclusions multiples de fichier.h
#ifndef FICHIER_H
#define FICHIER_H
#include <stddef.h>

// définir la structure d'un fichier selon 3 critères : nom-contenu-taille 
typedef struct {
    char *nom;
    char *contenu;
    size_t taille;
} Fichier;

//déclaration des fonctions de manipulations des fichiers 
Fichier *creer_fichier(const char *nom, const char *contenu);
void lire_fichier(const Fichier *f);
int modifier_fichier(Fichier *f, const char *nouveau_contenu);
void detruire_fichier(Fichier *f);

#endif
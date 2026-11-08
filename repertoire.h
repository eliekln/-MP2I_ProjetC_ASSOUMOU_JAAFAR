#ifndef REPERTOIRE_H
#define REPERTOIRE_H

#include <stddef.h>
#include "fichier.h"

typedef struct Repertoire {

    char *nom;

    struct Repertoire *parent;

    Fichier **fichiers;
    size_t nombre_fichiers;

    struct Repertoire **sous_repertoires;
    size_t nombre_sous_repertoires;

} Repertoire;

/* Creation */
Repertoire *creer_repertoire(const char *nom);

/* Ajout */
int ajouter_fichier(Repertoire *r, Fichier *f);

int ajouter_sous_repertoire(
    Repertoire *parent,
    Repertoire *enfant
);

/* Recherche */
Repertoire *chercher_repertoire(
    Repertoire *racine,
    const char *chemin
);

/* Navigation dans l'arborescence */
Repertoire *changer_repertoire(
    Repertoire *courant,
    const char *destination
);

Fichier *chercher_fichier(
    Repertoire *r,
    const char *nom
);

/* Supprimer un fichier d'un repertoire */
int supprimer_fichier(Repertoire *r, const char *nom);
/* Supprimer un sous-repertoire et tout son contenu */
int supprimer_sous_repertoire(
    Repertoire *parent,
    const char *nom
);

/* Affichage */
void afficher_fichiers(const Repertoire *r);

void afficher_sous_repertoires(const Repertoire *r);
/* Lister les fichiers et sous-repertoires directs */
void lister_contenu(const Repertoire *r);

void afficher_arborescence(
    const Repertoire *r,
    int profondeur
);

/* Afficher le chemin du repertoire courant */
void afficher_chemin(const Repertoire *r);
/* Calculer l'espace occupe par les fichiers */
size_t calculer_espace_utilise(const Repertoire *r);

/* Liberation de la memoire */
void detruire_repertoire(Repertoire *r);

#endif
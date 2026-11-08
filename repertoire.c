#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "repertoire.h"

/* =========================================
   CREATION D'UN REPERTOIRE
   ========================================= */

Repertoire *creer_repertoire(const char *nom)
{
    if (nom == NULL || nom[0] == '\0')
        return NULL;

    if (strchr(nom, '/') != NULL)
        return NULL;

    Repertoire *r = malloc(sizeof(Repertoire));

    if (r == NULL)
        return NULL;

    r->nom = malloc(strlen(nom) + 1);

    if (r->nom == NULL) {
        free(r);
        return NULL;
    }

    strcpy(r->nom, nom);

    r->parent = NULL;

    r->fichiers = NULL;
    r->nombre_fichiers = 0;

    r->sous_repertoires = NULL;
    r->nombre_sous_repertoires = 0;

    return r;
}

/* =========================================
   AJOUT D'UN FICHIER
   ========================================= */

int ajouter_fichier(Repertoire *r, Fichier *f)
{
    if (r == NULL || f == NULL)
        return 1;

    /* Verifier que le fichier n'existe pas deja */
    for (size_t i = 0; i < r->nombre_fichiers; i++) {

        if (r->fichiers[i] == f)
            return 1;

        if (strcmp(r->fichiers[i]->nom, f->nom) == 0)
            return 1;
    }

    /* Verifier qu'un repertoire ne porte pas ce nom */
    for (size_t i = 0; i < r->nombre_sous_repertoires; i++) {

        if (strcmp(r->sous_repertoires[i]->nom, f->nom) == 0)
            return 1;
    }

    size_t n = r->nombre_fichiers;

    if (n >= SIZE_MAX / sizeof(Fichier *))
        return 1;

    Fichier **temp = realloc(
        r->fichiers,
        (n + 1) * sizeof(Fichier *)
    );

    if (temp == NULL)
        return 1;

    r->fichiers = temp;

    r->fichiers[n] = f;

    r->nombre_fichiers++;

    return 0;
}

/* =========================================
   AJOUT D'UN SOUS-REPERTOIRE
   ========================================= */

int ajouter_sous_repertoire(
    Repertoire *parent,
    Repertoire *enfant
)
{
    if (parent == NULL || enfant == NULL)
        return 1;

    if (parent == enfant)
        return 1;

    if (enfant->parent != NULL)
        return 1;

    /* Eviter les cycles dans l'arborescence */
    for (Repertoire *p = parent; p != NULL; p = p->parent) {

        if (p == enfant)
            return 1;
    }

    /* Verifier les noms des sous-repertoires */
    for (size_t i = 0; i < parent->nombre_sous_repertoires; i++) {

        if (strcmp(parent->sous_repertoires[i]->nom, enfant->nom) == 0)
            return 1;
    }

    /* Verifier les noms des fichiers */
    for (size_t i = 0; i < parent->nombre_fichiers; i++) {

        if (strcmp(parent->fichiers[i]->nom, enfant->nom) == 0)
            return 1;
    }

    size_t n = parent->nombre_sous_repertoires;

    if (n >= SIZE_MAX / sizeof(Repertoire *))
        return 1;

    Repertoire **temp = realloc(
        parent->sous_repertoires,
        (n + 1) * sizeof(Repertoire *)
    );

    if (temp == NULL)
        return 1;

    parent->sous_repertoires = temp;

    parent->sous_repertoires[n] = enfant;

    parent->nombre_sous_repertoires++;

    enfant->parent = parent;

    return 0;
}

/* =========================================
   RECHERCHE D'UN REPERTOIRE
   ========================================= */

Repertoire *chercher_repertoire(
    Repertoire *racine,
    const char *chemin
)
{
    if (racine == NULL || chemin == NULL)
        return NULL;

    /* Le chemin 0 designe la racine */
    if (strcmp(chemin, "0") == 0)
        return racine;

    if (strcmp(chemin, racine->nom) == 0)
        return racine;

    Repertoire *courant = racine;

    const char *debut = chemin;

    while (*debut != '\0') {

        const char *fin = strchr(debut, '/');

        size_t longueur = (fin == NULL)
            ? strlen(debut)
            : (size_t)(fin - debut);

        if (longueur == 0)
            return NULL;

        Repertoire *trouve = NULL;

        for (size_t i = 0; i < courant->nombre_sous_repertoires; i++) {

            Repertoire *enfant = courant->sous_repertoires[i];

            if (
                strlen(enfant->nom) == longueur &&
                strncmp(enfant->nom, debut, longueur) == 0
            ) {
                trouve = enfant;
                break;
            }
        }

        if (trouve == NULL)
            return NULL;

        courant = trouve;

        if (fin == NULL)
            return courant;

        debut = fin + 1;
    }

    return NULL;
}

/* =========================================
   CHANGER DE REPERTOIRE
   ========================================= */

Repertoire *changer_repertoire(
    Repertoire *courant,
    const char *destination
)
{
    if (courant == NULL || destination == NULL)
        return NULL;

    /* Rester dans le repertoire courant */
    if (strcmp(destination, ".") == 0)
        return courant;

    /* Remonter au repertoire parent */
    if (strcmp(destination, "..") == 0) {

        if (courant->parent == NULL)
            return courant;

        return courant->parent;
    }

    /* Chercher un sous-repertoire direct */
    for (size_t i = 0; i < courant->nombre_sous_repertoires; i++) {

        Repertoire *enfant = courant->sous_repertoires[i];

        if (strcmp(enfant->nom, destination) == 0)
            return enfant;
    }

    /* Destination introuvable */
    return NULL;
}

/* =========================================
   RECHERCHE D'UN FICHIER
   ========================================= */

Fichier *chercher_fichier(
    Repertoire *r,
    const char *nom
)
{
    if (r == NULL || nom == NULL)
        return NULL;

    for (size_t i = 0; i < r->nombre_fichiers; i++) {

        if (strcmp(r->fichiers[i]->nom, nom) == 0)
            return r->fichiers[i];
    }

    return NULL;
}

int supprimer_fichier(Repertoire *r, const char *nom)
{
    if (r == NULL || nom == NULL)
        return 1;

    for (size_t i = 0; i < r->nombre_fichiers; i++) {

        if (strcmp(r->fichiers[i]->nom, nom) == 0) {

            /* Sauvegarder le fichier a supprimer */
            Fichier *fichier_a_supprimer = r->fichiers[i];

            /* Decaler les pointeurs */
            for (size_t j = i; j + 1 < r->nombre_fichiers; j++) {
                r->fichiers[j] = r->fichiers[j + 1];
            }

            /* Diminuer le nombre de fichiers */
            r->nombre_fichiers--;

            /* Liberer le fichier */
            detruire_fichier(fichier_a_supprimer);

            /* Cas particulier : plus aucun fichier */
            if (r->nombre_fichiers == 0) {

                free(r->fichiers);
                r->fichiers = NULL;

                return 0;
            }

            /* Reduire la taille du tableau */
            Fichier **temp = realloc(
                r->fichiers,
                r->nombre_fichiers * sizeof(Fichier *)
            );

            if (temp != NULL) {
                r->fichiers = temp;
            }

            return 0;
        }
    }

    return 1;
}
/* =========================================
   SUPPRESSION D'UN SOUS-REPERTOIRE
   ========================================= */

int supprimer_sous_repertoire(
    Repertoire *parent,
    const char *nom
)
{
    if (parent == NULL || nom == NULL)
        return 1;

    /* Rechercher le sous-repertoire */
    for (size_t i = 0; i < parent->nombre_sous_repertoires; i++) {

        if (strcmp(parent->sous_repertoires[i]->nom, nom) == 0) {

            /* Sauvegarder son adresse */
            Repertoire *repertoire_a_supprimer =
                parent->sous_repertoires[i];

            /* Decaler les pointeurs restants */
            for (
                size_t j = i;
                j + 1 < parent->nombre_sous_repertoires;
                j++
            ) {
                parent->sous_repertoires[j] =
                    parent->sous_repertoires[j + 1];
            }

            /* Mettre a jour le compteur */
            parent->nombre_sous_repertoires--;

            /* Detruire recursivement le repertoire */
            detruire_repertoire(repertoire_a_supprimer);

            /* Cas particulier : tableau devenu vide */
            if (parent->nombre_sous_repertoires == 0) {

                free(parent->sous_repertoires);

                parent->sous_repertoires = NULL;

                return 0;
            }

            /* Nettoyer la derniere case */
            parent->sous_repertoires[
                parent->nombre_sous_repertoires
            ] = NULL;

            /* Reduire le tableau dynamique */
            Repertoire **temp = realloc(
                parent->sous_repertoires,
                parent->nombre_sous_repertoires
                    * sizeof(Repertoire *)
            );

            if (temp != NULL) {
                parent->sous_repertoires = temp;
            }

            return 0;
        }
    }

    return 1;
}
/* =========================================
   AFFICHAGE DES FICHIERS
   ========================================= */

void afficher_fichiers(const Repertoire *r)
{
    if (r == NULL) {
        printf("Repertoire inexistant.\n");
        return;
    }

    printf("Repertoire : %s\n", r->nom);

    if (r->nombre_fichiers == 0) {
        printf("Aucun fichier.\n");
        return;
    }

    for (size_t i = 0; i < r->nombre_fichiers; i++) {
        printf("- %s\n", r->fichiers[i]->nom);
    }
}

/* =========================================
   AFFICHAGE DES SOUS-REPERTOIRES
   ========================================= */

void afficher_sous_repertoires(const Repertoire *r)
{
    if (r == NULL) {
        printf("Repertoire inexistant.\n");
        return;
    }

    printf("Sous-repertoires de %s :\n", r->nom);

    if (r->nombre_sous_repertoires == 0) {
        printf("Aucun sous-repertoire.\n");
        return;
    }

    for (size_t i = 0; i < r->nombre_sous_repertoires; i++) {
        printf("- %s/\n", r->sous_repertoires[i]->nom);
    }
}
/* =========================================
   LISTER LE CONTENU D'UN REPERTOIRE
   ========================================= */

void lister_contenu(const Repertoire *r)
{
    if (r == NULL) {
        printf("Erreur : repertoire inexistant.\n");
        return;
    }

    printf("\nContenu de %s :\n\n", r->nom);

    /* Cas particulier : repertoire vide */
    if (
        r->nombre_fichiers == 0 &&
        r->nombre_sous_repertoires == 0
    ) {
        printf("Le repertoire est vide.\n");
        return;
    }

    /* Affichage des fichiers */
    for (size_t i = 0; i < r->nombre_fichiers; i++) {

        Fichier *f = r->fichiers[i];

        printf(
            "[FICHIER] %-20s %zu octets\n",
            f->nom,
            f->taille
        );
    }

    /* Affichage des sous-repertoires */
    for (size_t i = 0; i < r->nombre_sous_repertoires; i++) {

        Repertoire *enfant = r->sous_repertoires[i];

        printf(
            "[DIR]     %s/\n",
            enfant->nom
        );
    }
}
/* =========================================
   AFFICHAGE RECURSIF DE L'ARBORESCENCE
   ========================================= */

void afficher_arborescence(
    const Repertoire *r,
    int profondeur
)
{
    if (r == NULL)
        return;

    for (int i = 0; i < profondeur; i++)
        printf("    ");

    printf("%s/\n", r->nom);

    /* Afficher les fichiers */
    for (size_t i = 0; i < r->nombre_fichiers; i++) {

        for (int j = 0; j < profondeur + 1; j++)
            printf("    ");

        printf("%s\n", r->fichiers[i]->nom);
    }

    /* Afficher les sous-repertoires */
    for (size_t i = 0; i < r->nombre_sous_repertoires; i++) {

        afficher_arborescence(
            r->sous_repertoires[i],
            profondeur + 1
        );
    }
}
/* =========================================
   AFFICHAGE DU CHEMIN COMPLET
   ========================================= */

void afficher_chemin(const Repertoire *r)
{
    if (r == NULL)
        return;

    /* Afficher d'abord le chemin du parent */
    if (r->parent != NULL) {

        afficher_chemin(r->parent);

        printf("/");
    }

    /* Afficher ensuite le nom du repertoire */
    printf("%s", r->nom);
}
/* =========================================
   CALCUL RECURSIF DE L'ESPACE UTILISE
   ========================================= */

size_t calculer_espace_utilise(const Repertoire *r)
{
    if (r == NULL)
        return 0;

    size_t total = 0;

    /* Additionner les tailles des fichiers */
    for (size_t i = 0; i < r->nombre_fichiers; i++) {

        total += r->fichiers[i]->taille;
    }

    /* Explorer recursivement les sous-repertoires */
    for (size_t i = 0; i < r->nombre_sous_repertoires; i++) {

        total += calculer_espace_utilise(
            r->sous_repertoires[i]
        );
    }

    return total;
}
/* =========================================
   DESTRUCTION RECURSIVE D'UN REPERTOIRE
   ========================================= */

void detruire_repertoire(Repertoire *r)
{
    if (r == NULL)
        return;

    /* Detruire tous les fichiers */
    for (size_t i = 0; i < r->nombre_fichiers; i++) {
        detruire_fichier(r->fichiers[i]);
    }

    free(r->fichiers);

    /* Detruire tous les sous-repertoires */
    for (size_t i = 0; i < r->nombre_sous_repertoires; i++) {

        detruire_repertoire(r->sous_repertoires[i]);
    }

    free(r->sous_repertoires);

    free(r->nom);

    free(r);
}
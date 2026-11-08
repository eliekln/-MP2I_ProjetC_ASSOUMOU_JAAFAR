#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "fichier.h"
#include "repertoire.h"

/* =========================================
   CAPACITE DU DISQUE VIRTUEL : 1 MIO
   ========================================= */

#define CAPACITE_TOTALE (1024ULL * 1024ULL)

/* =========================================
   LECTURE SECURISEE D'UNE CHAINE
   ========================================= */

int lire_chaine(char *chaine, size_t taille)
{
    if (chaine == NULL || taille == 0 || taille > INT_MAX)
        return 0;

    if (fgets(chaine, (int)taille, stdin) == NULL)
        return 0;

    size_t longueur = strlen(chaine);

    if (longueur > 0 && chaine[longueur - 1] == '\n') {
        chaine[longueur - 1] = '\0';
    }
    else if (!feof(stdin)) {
        int c;

        while ((c = getchar()) != '\n' && c != EOF) {
        }

        return 0;
    }

    return 1;
}

/* =========================================
   VERIFIER L'ESPACE AVANT UNE CREATION
   ========================================= */

int espace_suffisant_creation(
    const Repertoire *racine,
    size_t nouvelle_taille
)
{
    if (racine == NULL)
        return 0;

    size_t utilise = calculer_espace_utilise(racine);

    if (utilise > CAPACITE_TOTALE)
        return 0;

    return nouvelle_taille <= CAPACITE_TOTALE - utilise;
}

/* =========================================
   VERIFIER L'ESPACE AVANT UNE MODIFICATION
   ========================================= */

int espace_suffisant_modification(
    const Repertoire *racine,
    const Fichier *f,
    size_t nouvelle_taille
)
{
    if (racine == NULL || f == NULL)
        return 0;

    size_t utilise = calculer_espace_utilise(racine);

    if (utilise > CAPACITE_TOTALE || f->taille > utilise)
        return 0;

    size_t utilise_sans_fichier = utilise - f->taille;

    return nouvelle_taille <=
           CAPACITE_TOTALE - utilise_sans_fichier;
}

/* =========================================
   VERIFIER LA VALIDITE D'UN NOM
   ========================================= */

int nom_valide(const char *nom)
{
    if (nom == NULL || nom[0] == '\0')
        return 0;

    if (strcmp(nom, ".") == 0 ||
        strcmp(nom, "..") == 0)
        return 0;

    if (strchr(nom, '/') != NULL ||
        strchr(nom, '\\') != NULL)
        return 0;

    return 1;
}

/* =========================================
   FONCTION PRINCIPALE
   ========================================= */

int main(void)
{
    Repertoire *racine = creer_repertoire("racine");

    if (racine == NULL) {
        printf("Erreur : creation de la racine impossible.\n");
        return 1;
    }

    /* Repertoire courant */
    Repertoire *courant = racine;

    /* Variables de saisie */
    char choix[20];
    char nom[100];
    char chemin_parent[256];
    char contenu[1024];

    int continuer = 1;

    while (continuer) {

        /* =====================================
           MENU PRINCIPAL
           ===================================== */

        printf("\n========== SYSTEME DE FICHIERS ==========\n");

        printf("1. Creer un repertoire\n");
        printf("2. Afficher l'arborescence\n");
        printf("3. Creer un fichier\n");
        printf("4. Lire un fichier\n");
        printf("5. Modifier un fichier\n");
        printf("6. Supprimer un fichier\n");
        printf("7. Supprimer un repertoire\n");
        printf("8. Naviguer dans les repertoires\n");
        printf("9. Afficher l'espace de stockage\n");
        printf("0. Quitter\n");

        printf("\nVotre choix : ");

        if (!lire_chaine(choix, sizeof(choix))) {

            printf("Saisie invalide.\n");

            if (feof(stdin))
                break;

            continue;
        }

        /* =====================================
           1. CREER UN REPERTOIRE
           ===================================== */

        if (strcmp(choix, "1") == 0) {

            printf("\nNom du nouveau repertoire : ");

            if (!lire_chaine(nom, sizeof(nom))) {
                printf("Nom invalide.\n");
                continue;
            }

            if (!nom_valide(nom)) {
                printf("Erreur : nom de repertoire invalide.\n");
                continue;
            }

            printf("Chemin du repertoire parent (0 pour racine) : ");

            if (!lire_chaine(chemin_parent, sizeof(chemin_parent))) {
                printf("Chemin invalide.\n");
                continue;
            }

            Repertoire *parent = chercher_repertoire(
                racine,
                chemin_parent
            );

            if (parent == NULL) {
                printf("Erreur : repertoire parent introuvable.\n");
                continue;
            }

            Repertoire *nouveau = creer_repertoire(nom);

            if (nouveau == NULL) {
                printf("Erreur : creation impossible.\n");
                continue;
            }

            if (ajouter_sous_repertoire(parent, nouveau) != 0) {

                printf("Erreur : ajout impossible ou nom deja utilise.\n");

                detruire_repertoire(nouveau);
                continue;
            }

            printf("Repertoire \"%s\" cree avec succes !\n", nom);
        }

        /* =====================================
           2. AFFICHER L'ARBORESCENCE
           ===================================== */

        else if (strcmp(choix, "2") == 0) {

            printf("\nArborescence actuelle :\n\n");

            afficher_arborescence(racine, 0);
        }

        /* =====================================
           3. CREER UN FICHIER
           ===================================== */

        else if (strcmp(choix, "3") == 0) {

            printf("\nNom du fichier : ");

            if (!lire_chaine(nom, sizeof(nom))) {
                printf("Nom invalide.\n");
                continue;
            }

            if (!nom_valide(nom)) {
                printf("Erreur : nom de fichier invalide.\n");
                continue;
            }

            printf("Contenu du fichier : ");

            if (!lire_chaine(contenu, sizeof(contenu))) {
                printf("Contenu invalide.\n");
                continue;
            }

            printf("Chemin du repertoire parent (0 pour racine) : ");

            if (!lire_chaine(chemin_parent, sizeof(chemin_parent))) {
                printf("Chemin invalide.\n");
                continue;
            }

            Repertoire *parent = chercher_repertoire(
                racine,
                chemin_parent
            );

            if (parent == NULL) {
                printf("Erreur : repertoire introuvable.\n");
                continue;
            }

            if (!espace_suffisant_creation(
                    racine,
                    strlen(contenu))) {

                printf("Erreur : espace de stockage insuffisant.\n");
                continue;
            }

            Fichier *nouveau = creer_fichier(nom, contenu);

            if (nouveau == NULL) {
                printf("Erreur : creation du fichier impossible.\n");
                continue;
            }

            if (ajouter_fichier(parent, nouveau) != 0) {

                printf("Erreur : ajout impossible ou nom deja utilise.\n");

                detruire_fichier(nouveau);
                continue;
            }

            printf("Fichier \"%s\" cree avec succes !\n", nom);
        }

        /* =====================================
           4. LIRE UN FICHIER
           ===================================== */

        else if (strcmp(choix, "4") == 0) {

            printf("\nChemin du repertoire (0 pour racine) : ");

            if (!lire_chaine(chemin_parent, sizeof(chemin_parent))) {
                printf("Chemin invalide.\n");
                continue;
            }

            Repertoire *parent = chercher_repertoire(
                racine,
                chemin_parent
            );

            if (parent == NULL) {
                printf("Erreur : repertoire introuvable.\n");
                continue;
            }

            printf("Nom du fichier a lire : ");

            if (!lire_chaine(nom, sizeof(nom))) {
                printf("Nom invalide.\n");
                continue;
            }

            Fichier *f = chercher_fichier(parent, nom);

            if (f == NULL) {
                printf("Erreur : fichier introuvable.\n");
                continue;
            }

            printf("\n");

            lire_fichier(f);

            printf("Taille : %zu octets\n", f->taille);
        }

        /* =====================================
           5. MODIFIER UN FICHIER
           ===================================== */

        else if (strcmp(choix, "5") == 0) {

            printf("\nChemin du repertoire (0 pour racine) : ");

            if (!lire_chaine(chemin_parent, sizeof(chemin_parent))) {
                printf("Chemin invalide.\n");
                continue;
            }

            Repertoire *parent = chercher_repertoire(
                racine,
                chemin_parent
            );

            if (parent == NULL) {
                printf("Erreur : repertoire introuvable.\n");
                continue;
            }

            printf("Nom du fichier a modifier : ");

            if (!lire_chaine(nom, sizeof(nom))) {
                printf("Nom invalide.\n");
                continue;
            }

            Fichier *f = chercher_fichier(parent, nom);

            if (f == NULL) {
                printf("Erreur : fichier introuvable.\n");
                continue;
            }

            printf("Nouveau contenu : ");

            if (!lire_chaine(contenu, sizeof(contenu))) {
                printf("Contenu invalide.\n");
                continue;
            }

            if (!espace_suffisant_modification(
                    racine,
                    f,
                    strlen(contenu))) {

                printf("Erreur : espace de stockage insuffisant.\n");
                continue;
            }

            if (modifier_fichier(f, contenu) != 0) {

                printf("Erreur : modification impossible.\n");
                continue;
            }

            printf("Fichier \"%s\" modifie avec succes !\n", nom);
        }

        /* =====================================
           6. SUPPRIMER UN FICHIER
           ===================================== */

        else if (strcmp(choix, "6") == 0) {

            printf("\nChemin du repertoire (0 pour racine) : ");

            if (!lire_chaine(chemin_parent, sizeof(chemin_parent))) {
                printf("Chemin invalide.\n");
                continue;
            }

            Repertoire *parent = chercher_repertoire(
                racine,
                chemin_parent
            );

            if (parent == NULL) {
                printf("Erreur : repertoire introuvable.\n");
                continue;
            }

            printf("Nom du fichier a supprimer : ");

            if (!lire_chaine(nom, sizeof(nom))) {
                printf("Nom invalide.\n");
                continue;
            }

            if (supprimer_fichier(parent, nom) != 0) {

                printf("Erreur : fichier introuvable.\n");
                continue;
            }

            printf("Fichier \"%s\" supprime avec succes !\n", nom);
        }

        /* =====================================
           7. SUPPRIMER UN REPERTOIRE
           ===================================== */

        else if (strcmp(choix, "7") == 0) {

            printf("\nChemin du repertoire parent (0 pour racine) : ");

            if (!lire_chaine(chemin_parent, sizeof(chemin_parent))) {
                printf("Chemin invalide.\n");
                continue;
            }

            Repertoire *parent = chercher_repertoire(
                racine,
                chemin_parent
            );

            if (parent == NULL) {
                printf("Erreur : repertoire parent introuvable.\n");
                continue;
            }

            printf("Nom du sous-repertoire a supprimer : ");

            if (!lire_chaine(nom, sizeof(nom))) {
                printf("Nom invalide.\n");
                continue;
            }

            Repertoire *cible = NULL;

            for (size_t i = 0;
                 i < parent->nombre_sous_repertoires;
                 i++) {

                if (strcmp(
                        parent->sous_repertoires[i]->nom,
                        nom) == 0) {

                    cible = parent->sous_repertoires[i];
                    break;
                }
            }

            if (cible == NULL) {
                printf("Erreur : sous-repertoire introuvable.\n");
                continue;
            }

            char confirmation[10];

            printf(
                "\nAttention : \"%s\" et tout son contenu "
                "seront supprimes.\n",
                nom
            );

            printf("Confirmer (oui/non) : ");

            if (!lire_chaine(confirmation, sizeof(confirmation))) {
                printf("Confirmation invalide.\n");
                continue;
            }

            if (strcmp(confirmation, "oui") != 0) {
                printf("Suppression annulee.\n");
                continue;
            }

            /* Protection du repertoire courant */

            for (Repertoire *p = courant;
                 p != NULL;
                 p = p->parent) {

                if (p == cible) {
                    courant = racine;
                    break;
                }
            }

            if (supprimer_sous_repertoire(parent, nom) != 0) {

                printf("Erreur : suppression impossible.\n");
                continue;
            }

            printf(
                "Repertoire \"%s\" et son contenu supprimes !\n",
                nom
            );
        }

        /* =====================================
           8. MINI-TERMINAL
           ===================================== */

        else if (strcmp(choix, "8") == 0) {

            char commande[256];

            printf("\n========== MINI-TERMINAL ==========\n");

            printf("Commandes disponibles :\n");
            printf("  cd nom    : entrer dans un repertoire\n");
            printf("  cd ..     : remonter au parent\n");
            printf("  cd .      : rester dans le repertoire courant\n");
            printf("  pwd       : afficher le chemin actuel\n");
            printf("  ls        : lister le contenu\n");
            printf("  mkdir nom : creer un sous-repertoire\n");
            printf("  touch nom : creer un fichier vide\n");
            printf("  cat nom   : lire un fichier\n");
            printf("  write nom : ecrire dans un fichier\n");
            printf("  rm nom    : supprimer un fichier\n");
            printf("  exit      : retourner au menu principal\n");

            int navigation = 1;

            while (navigation) {

                printf("\n");

                afficher_chemin(courant);

                printf("> ");

                if (!lire_chaine(commande, sizeof(commande))) {

                    if (feof(stdin)) {
                        navigation = 0;
                        continuer = 0;
                        break;
                    }

                    printf("Commande invalide.\n");
                    continue;
                }

                /* ---------------------------------
                   COMMANDE pwd
                   --------------------------------- */

                if (strcmp(commande, "pwd") == 0) {

                    afficher_chemin(courant);
                    printf("\n");
                }

                /* ---------------------------------
                   COMMANDE ls
                   --------------------------------- */

                else if (strcmp(commande, "ls") == 0) {

                    lister_contenu(courant);
                }

                /* ---------------------------------
                   COMMANDE exit
                   --------------------------------- */

                else if (strcmp(commande, "exit") == 0) {

                    navigation = 0;
                }

                /* ---------------------------------
                   COMMANDE cd
                   --------------------------------- */

                else if (strncmp(commande, "cd ", 3) == 0) {

                    const char *destination = commande + 3;

                    if (destination[0] == '\0') {
                        printf("Erreur : destination vide.\n");
                        continue;
                    }

                    Repertoire *nouveau = changer_repertoire(
                        courant,
                        destination
                    );

                    if (nouveau == NULL) {

                        printf("Erreur : repertoire introuvable.\n");
                        continue;
                    }

                    courant = nouveau;
                }

                /* ---------------------------------
                   COMMANDE mkdir
                   --------------------------------- */

                else if (strncmp(commande, "mkdir ", 6) == 0) {

                    const char *nom_repertoire = commande + 6;

                    if (!nom_valide(nom_repertoire)) {

                        printf("Erreur : nom invalide.\n");
                        continue;
                    }

                    Repertoire *nouveau = creer_repertoire(
                        nom_repertoire
                    );

                    if (nouveau == NULL) {

                        printf("Erreur : creation impossible.\n");
                        continue;
                    }

                    if (ajouter_sous_repertoire(
                            courant,
                            nouveau) != 0) {

                        printf("Erreur : ajout impossible.\n");

                        detruire_repertoire(nouveau);
                        continue;
                    }

                    printf("Repertoire cree avec succes.\n");
                }

                /* ---------------------------------
                   COMMANDE touch
                   --------------------------------- */

                else if (strncmp(commande, "touch ", 6) == 0) {

                    const char *nom_fichier = commande + 6;

                    if (!nom_valide(nom_fichier)) {

                        printf("Erreur : nom invalide.\n");
                        continue;
                    }

                    if (chercher_fichier(
                            courant,
                            nom_fichier) != NULL) {

                        printf("Le fichier existe deja.\n");
                        continue;
                    }

                    Fichier *nouveau = creer_fichier(
                        nom_fichier,
                        ""
                    );

                    if (nouveau == NULL) {

                        printf("Erreur : creation impossible.\n");
                        continue;
                    }

                    if (ajouter_fichier(
                            courant,
                            nouveau) != 0) {

                        printf("Erreur : ajout impossible.\n");

                        detruire_fichier(nouveau);
                        continue;
                    }

                    printf("Fichier vide cree avec succes.\n");
                }

                /* ---------------------------------
                   COMMANDE cat
                   --------------------------------- */

                else if (strncmp(commande, "cat ", 4) == 0) {

                    const char *nom_fichier = commande + 4;

                    if (nom_fichier[0] == '\0') {

                        printf("Erreur : nom vide.\n");
                        continue;
                    }

                    Fichier *f = chercher_fichier(
                        courant,
                        nom_fichier
                    );

                    if (f == NULL) {

                        printf("Erreur : fichier introuvable.\n");
                        continue;
                    }

                    lire_fichier(f);
                }

                /* ---------------------------------
                   COMMANDE write
                   --------------------------------- */

                else if (strncmp(commande, "write ", 6) == 0) {

                    const char *nom_fichier = commande + 6;

                    if (nom_fichier[0] == '\0') {

                        printf("Erreur : nom vide.\n");
                        continue;
                    }

                    /* Rechercher le fichier */

                    Fichier *f = chercher_fichier(
                        courant,
                        nom_fichier
                    );

                    if (f == NULL) {

                        printf("Erreur : fichier introuvable.\n");
                        continue;
                    }

                    /* Lire le nouveau contenu */

                    char nouveau_contenu[1024];

                    printf("Nouveau contenu : ");

                    if (!lire_chaine(
                            nouveau_contenu,
                            sizeof(nouveau_contenu))) {

                        printf("Erreur : contenu invalide.\n");
                        continue;
                    }

                    /* Verifier l'espace disponible */

                    if (!espace_suffisant_modification(
                            racine,
                            f,
                            strlen(nouveau_contenu))) {

                        printf(
                            "Erreur : espace de stockage insuffisant.\n"
                        );

                        continue;
                    }

                    /* Modifier le contenu du fichier */

                    if (modifier_fichier(
                            f,
                            nouveau_contenu) != 0) {

                        printf("Erreur : modification impossible.\n");
                        continue;
                    }

                    printf("Fichier modifie avec succes.\n");
                }

                /* ---------------------------------
                   COMMANDE rm
                   --------------------------------- */

                else if (strncmp(commande, "rm ", 3) == 0) {

                    const char *nom_fichier = commande + 3;

                    if (nom_fichier[0] == '\0') {

                        printf("Erreur : nom vide.\n");
                        continue;
                    }

                    if (supprimer_fichier(
                            courant,
                            nom_fichier) != 0) {

                        printf("Erreur : fichier introuvable.\n");
                        continue;
                    }

                    printf("Fichier supprime avec succes.\n");
                }

                /* ---------------------------------
                   COMMANDE INCONNUE
                   --------------------------------- */

                else {

                    printf("Commande inconnue.\n");
                }
            }
        }

        /* =====================================
           9. ESPACE DE STOCKAGE
           ===================================== */

        else if (strcmp(choix, "9") == 0) {

            size_t utilise = calculer_espace_utilise(racine);

            printf("\n========== ESPACE DE STOCKAGE ==========\n");

            printf(
                "Capacite totale   : %zu octets\n",
                (size_t)CAPACITE_TOTALE
            );

            printf(
                "Espace utilise    : %zu octets\n",
                utilise
            );

            if (utilise <= CAPACITE_TOTALE) {

                size_t disponible = CAPACITE_TOTALE - utilise;

                printf(
                    "Espace disponible : %zu octets\n",
                    disponible
                );
            }
            else {

                printf("Espace disponible : 0 octet\n");
                printf("Attention : capacite depassee !\n");
            }

            double pourcentage =
                100.0 * (double)utilise /
                (double)CAPACITE_TOTALE;

            printf(
                "Utilisation       : %.2f %%\n",
                pourcentage
            );
        }

        /* =====================================
           0. QUITTER
           ===================================== */

        else if (strcmp(choix, "0") == 0) {

            continuer = 0;
        }

        /* =====================================
           CHOIX INVALIDE
           ===================================== */

        else {

            printf("Choix invalide.\n");
        }
    }

    /* =========================================
       LIBERATION DE TOUTE L'ARBORESCENCE
       ========================================= */

    detruire_repertoire(racine);

    printf("\nProgramme termine.\n");

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//inclure la structure fichier déclaré en fichier.h
#include "fichier.h"

/*----------------------------------
Creer un nouveau fichier 
-----------------------------------*/

//declarer la fonction 
Fichier *creer_fichier(const char *nom, const char *contenu)
{
// Verifier que les pointeurs sont valides et que le nom n'est pas vide , si c'est le cas on retourne null
    if (nom == NULL || contenu == NULL || nom[0] == '\0')
        return NULL;
//on alloue au fichier créer la taille dont la structure d'un fichier a besoin
    Fichier *f = malloc(sizeof(Fichier));
//on vérifie si malloc a échouer , si c'est le cas on retourne null en action
    if (f == NULL)
        return NULL;
//on alloue au nom du fichier une memoire de taille de nombre de lettre du nom + 1
    f->nom = malloc(strlen(nom) + 1);
//on verifie si malloc a echouer ; si c'est le cas ; on libere le fichier de la memoire et on retourne null en action
    if (f->nom == NULL) {
        free(f);
        return NULL;
    }
//copier le nom saisi par l'utilisateur dans la composante nom de la structure du fichier créer
    strcpy(f->nom, nom);
//on alloue au contenu de fichier une memoire de taille de nombre de letttre de contenu + 1
    f->contenu = malloc(strlen(contenu) + 1);
//on verifie si malloc a echoue ; si c'est le cas on libere le nom de f , et on libere le contenu de f et apres on libere la structure de f
    if (f->contenu == NULL) {
        free(f->nom);
        free(f);
        return NULL;
    }
//on copie le contenu saisi par l'utilisateur dont la composante contenu de la structure de f 
    strcpy(f->contenu, contenu);
//on associe à la composante taille de la structure de f le nombre de lettre de son contenu
    f->taille = strlen(contenu);
//on retourne le fichier f
    return f;
}

/*----------------------------- 
Lire le contenu d'un fichier 
--------------------------------*/

//declarer la fonction
void lire_fichier(const Fichier *f)
{
//verifier si le pointeur vers le fichier est vide, si c'est le cas notifie de l'utilisateur ,et sort de la fonction
    if (f == NULL) {
        printf("Erreur : fichier inexistant.\n");
        return;
    }
//ecrire le nom et le contenu de fichier
    printf("Contenu du fichier %s :\n", f->nom);
    printf("%s\n", f->contenu);
}

/*--------------------------------
 Modifier le contenu d'un fichier 
 ----------------------------------*/

//declarer la fonction de modification
int modifier_fichier(Fichier *f, const char *nouveau_contenu)
{
//verifie si le pointeur vers le fichier est vide ou le nouveau contenu est vide , si c'est le cas retourne echec de modification 
    if (f == NULL || nouveau_contenu == NULL)
        return 1;
//memoriser une nouvelle taille de valeur de nombre de lettre du nouveau contenu
    size_t nouvelle_taille = strlen(nouveau_contenu);
//allouer de la memoire au pointeur nouveau de taille de la nouvelle taille + 1 
    char *nouveau = malloc(nouvelle_taille + 1);
//verifie si le pointeur nouveau est vide si c'est le cas  retourne erreur de la modification
    if (nouveau == NULL)
        return 1;
//copier le nouveau contenu dans nouveau
    strcpy(nouveau, nouveau_contenu);
//liberer le contenu dont pointe f->contenu
    free(f->contenu);
//associe a f->contenu le contenu de nouveau
    f->contenu = nouveau;
//attribuer à f->taille la valeur de nouvelle_taille
    f->taille = nouvelle_taille;
//retourne modification reussie
    return 0;
}


/*---------------------------------
 Liberer la memoire d'un fichier 
 ----------------------------------*/

//declarer la fonction 
void detruire_fichier(Fichier *f)
{
//verifier si f est vide , si c'est le cas sortir de la fonction
    if (f == NULL)
        return;
//liberer le nom , le contenu , la structure de fichier
    free(f->nom);
    free(f->contenu);
    free(f);
}
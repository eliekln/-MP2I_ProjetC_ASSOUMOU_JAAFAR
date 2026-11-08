# -MP2I_ProjetC_ASSOUMOU_JAAFAR
Projet C, Système de fichiers simplifié
# Projet de programmation en C – Système de fichiers simplifié

## Identification du binôme

| Élément | Information |
|---|---|
| **Étudiant 1** | Rayan JAAFAR |
| **Étudiant 2** | Elie ASSOUMOU |
| **Projet choisi** | Projet 1 – Création d'un système de fichiers simplifié en C |
| **Dépôt GitHub** | |
| **Date du dernier commit avant rendu** | 08 novembre 2026|

---

## 1. Présentation du projet

Ce projet consiste à réaliser en langage C une simulation simplifiée d'un système de fichiers. Le programme permet à l'utilisateur de créer et de gérer des fichiers ainsi que des répertoires organisés sous la forme d'une arborescence.

L'utilisateur interagit avec le programme à l'aide d'un menu en ligne de commande. Il peut également accéder à un mini-terminal qui reprend plusieurs commandes courantes (`cd`, `ls`, `mkdir`, `touch`, `cat`, etc.).

Le système est **simulé en mémoire vive** : les fichiers et les répertoires sont représentés par des structures C et des allocations dynamiques. Le programme ne crée pas de véritables fichiers sur le disque de l'ordinateur. Les données sont donc perdues lorsque le programme se termine.

### Fonctionnalités réalisées

- **Créer un fichier** avec un nom et un contenu initial.
- **Lire un fichier** et afficher son contenu ainsi que sa taille en octets.
- **Modifier un fichier** en remplaçant son ancien contenu.
- **Supprimer un fichier** et libérer la mémoire qui lui est associée.
- **Créer un répertoire** dans un répertoire parent choisi.
- **Afficher l'arborescence complète** des répertoires et des fichiers.
- **Lister le contenu d'un répertoire** : fichiers et sous-répertoires directs.
- **Naviguer dans les répertoires** grâce à un mini-terminal.
- **Gérer l'espace de stockage simulé**, avec une capacité maximale de 1 Mio.
- **Supprimer un répertoire et tout son contenu**, après demande de confirmation.

## 2. Analyse du problème

### 2.1. Besoins identifiés

Le programme doit pouvoir représenter plusieurs fichiers et plusieurs répertoires, sans connaître à l'avance leur nombre. Il doit aussi permettre de ranger les répertoires les uns dans les autres et de retrouver un élément à partir de son nom ou d'un chemin.

Les opérations principales sont donc :

1. créer et enregistrer des fichiers et des répertoires dans la structure du système ;
2. retrouver un fichier ou un répertoire pour le consulter ou le modifier ;
3. supprimer un élément sans laisser de mémoire allouée inutilement ;
4. parcourir l'arborescence pour l'afficher ou calculer l'espace utilisé ;
5. contrôler les saisies et les erreurs d'allocation mémoire.

### 2.2. Contraintes et choix retenus

- Le nombre de fichiers et de sous-répertoires n'est pas fixé à l'avance : les tableaux de pointeurs sont donc agrandis dynamiquement avec `realloc`.
- Un répertoire peut contenir des fichiers et d'autres répertoires.
- Dans un même répertoire, un fichier et un sous-répertoire ne peuvent pas porter le même nom. Les noms en double sont refusés.
- Les noms ne peuvent pas être vides, égaux à `.` ou `..`, ni contenir `/` ou `\\` lorsqu'ils sont saisis dans le menu.
- La capacité du stockage simulé est fixée à **1 048 576 octets (1 Mio)**.
- Dans le calcul de cette capacité, le programme additionne la taille des contenus des fichiers. Les noms, les structures et les pointeurs ne sont pas comptabilisés.
- La saisie est faite ligne par ligne avec `fgets`. Les champs utilisés dans le programme ont des tailles maximales définies dans `main.c` : par exemple 99 caractères pour un nom de fichier ou de répertoire et 1 023 caractères pour un contenu saisi dans les tableaux de 1 024 caractères.
- Les données restent en mémoire pendant l'exécution du programme. Il n'y a pas de sauvegarde persistante sur le disque.

## 3. Conception du programme

### 3.1. Structures de données

Deux structures principales permettent de représenter le système.

#### Structure `Fichier`

Définie dans `fichier.h`, elle représente un fichier et contient :

| Attribut | Type | Rôle |
|---|---|---|
| `nom` | `char *` | Pointeur vers le nom du fichier, alloué dynamiquement. |
| `contenu` | `char *` | Pointeur vers le contenu du fichier, alloué dynamiquement. |
| `taille` | `size_t` | Nombre d'octets du contenu, sans compter le caractère final `\0`. |

#### Structure `Repertoire`

Définie dans `repertoire.h`, elle représente un répertoire et contient :

| Attribut | Type | Rôle |
|---|---|---|
| `nom` | `char *` | Nom du répertoire. |
| `parent` | `struct Repertoire *` | Pointeur vers le répertoire parent. Il vaut `NULL` pour la racine. |
| `fichiers` | `Fichier **` | Tableau dynamique de pointeurs vers les fichiers contenus dans le répertoire. |
| `nombre_fichiers` | `size_t` | Nombre de fichiers présents. |
| `sous_repertoires` | `struct Repertoire **` | Tableau dynamique de pointeurs vers les sous-répertoires. |
| `nombre_sous_repertoires` | `size_t` | Nombre de sous-répertoires présents. |

### 3.2. Représentation de l'arborescence

La racine du système est créée au lancement du programme sous le nom `racine`. Chaque répertoire peut ensuite contenir des fichiers et des sous-répertoires.

Exemple d'arborescence possible :

```text
racine/
├── documents/
│   ├── cours.txt
│   └── exercices.txt
└── projets/
    └── programmation/
        └── notes.txt
```

Les liens entre répertoires sont assurés par les pointeurs du tableau `sous_repertoires`. Le pointeur `parent` permet de remonter vers le répertoire précédent, notamment avec la commande `cd ..`.

### 3.3. Découpage en fichiers et en fonctions

Le code est séparé en trois modules, avec un fichier d'en-tête pour les déclarations nécessaires.

| Fichier | Rôle principal | Fonctions et éléments importants |
|---|---|---|
| `fichier.h` | Déclarer la structure `Fichier` et les fonctions de gestion des fichiers. | `Fichier`, `creer_fichier`, `lire_fichier`, `modifier_fichier`, `detruire_fichier`. |
| `fichier.c` | Gérer le cycle de vie et le contenu des fichiers. | Création, lecture, modification et libération de la mémoire d'un fichier. |
| `repertoire.h` | Déclarer la structure `Repertoire` et les fonctions de gestion des répertoires. | `Repertoire`, fonctions d'ajout, de recherche, de suppression, d'affichage et de navigation. |
| `repertoire.c` | Gérer l'arborescence, les fichiers contenus dans les répertoires et les opérations associées. | `creer_repertoire`, `ajouter_fichier`, `ajouter_sous_repertoire`, `chercher_repertoire`, `chercher_fichier`, `supprimer_fichier`, `supprimer_sous_repertoire`, `lister_contenu`, `afficher_arborescence`, `calculer_espace_utilise`, `detruire_repertoire`, etc. |
| `main.c` | Gérer l'interface utilisateur, les saisies et le déroulement du programme. | Menu principal, mini-terminal, `lire_chaine`, `nom_valide`, contrôles de capacité et fonction `main`. |

### 3.4. Principes algorithmiques utilisés

**Création d'un fichier :**

```text
Vérifier que le nom et le contenu sont valides
Allouer la mémoire nécessaire à la structure Fichier
Allouer la mémoire nécessaire au nom
Copier le nom dans la mémoire allouée
Allouer la mémoire nécessaire au contenu
Copier le contenu dans la mémoire allouée
Calculer et enregistrer la taille du contenu
Retourner le pointeur vers le fichier créé
```

Si une allocation échoue, la fonction libère les allocations déjà réalisées avant de retourner `NULL`.

**Ajout à un répertoire :**

```text
Vérifier que le répertoire et l'élément existent
Vérifier qu'aucun élément du même nom n'est déjà présent
Agrandir le tableau de pointeurs avec realloc
Ajouter le pointeur à la nouvelle case
Mettre à jour le nombre d'éléments
```

**Calcul de l'espace utilisé :**

```text
Si le répertoire est NULL, retourner 0
Initialiser le total à 0
Ajouter la taille du contenu de chaque fichier du répertoire
Pour chaque sous-répertoire :
    ajouter récursivement son espace utilisé
Retourner le total
```

La récursivité est également utilisée pour afficher l'arborescence, afficher le chemin complet d'un répertoire et détruire un répertoire avec tous ses descendants.

## 4. Démarche de réalisation et choix techniques

Le code est organisé autour de la séparation entre les fichiers, les répertoires et l'interface utilisateur. Cette séparation permet de distinguer les opérations sur le contenu d'un fichier des opérations sur l'arborescence.

Les principales étapes fonctionnelles de la réalisation sont les suivantes :

1. **Définir les structures.** Les structures `Fichier` et `Repertoire` décrivent les informations nécessaires pour représenter les éléments du système.
2. **Mettre en place la gestion des fichiers.** Les fonctions du module `fichier.c` permettent de créer, lire, modifier et détruire un fichier.
3. **Mettre en place l'arborescence.** Le module `repertoire.c` gère les liens parent-enfant, les tableaux de fichiers et de sous-répertoires, les recherches et les suppressions.
4. **Créer l'interface en ligne de commande.** `main.c` propose un menu principal et un mini-terminal pour effectuer les opérations.
5. **Ajouter les contrôles.** Le programme vérifie les saisies, les noms, les chemins, les doublons, les retours d'allocation et la capacité disponible avant les créations ou modifications concernées.
6. **Prévoir la libération de la mémoire.** À la sortie, `detruire_repertoire` détruit récursivement toute l'arborescence et les fichiers qu'elle contient.

### Points techniques traités dans le code

- **Échec d'une allocation :** les retours de `malloc` et de `realloc` sont vérifiés. Lorsqu'une création échoue, les allocations déjà effectuées sont libérées.
- **Modification d'un contenu :** le nouveau contenu est alloué et copié avant que l'ancien contenu soit libéré. Ainsi, un échec de l'allocation ne détruit pas le contenu initial.
- **Tableaux dynamiques :** le résultat de `realloc` est d'abord stocké dans un pointeur temporaire. Le pointeur initial n'est donc pas perdu si l'agrandissement échoue.
- **Suppression récursive :** supprimer un répertoire supprime aussi ses fichiers et ses sous-répertoires.
- **Arborescence cohérente :** l'ajout d'un sous-répertoire vérifie notamment qu'il n'introduit pas de cycle et qu'il n'est pas déjà rattaché à un parent.
- **Saisie trop longue :** `lire_chaine` détecte une ligne qui dépasse la taille du tableau, vide le reste de la saisie et signale une erreur.

> **Retour d'expérience à compléter par le binôme :** les points ci-dessus décrivent les protections présentes dans le code. Ajoutez ici les difficultés que vous avez réellement rencontrées pendant le développement, les erreurs observées et la manière dont vous les avez résolues. Il ne faut pas présenter comme vécue une difficulté qui ne l'a pas été.

## 5. Compilation et utilisation

### 5.1. Prérequis

- Un compilateur C prenant en charge le standard C11, par exemple GCC.
- Un terminal (terminal VS Code, PowerShell avec GCC/MinGW, Linux ou WSL).
- Les cinq fichiers du projet : `main.c`, `fichier.c`, `fichier.h`, `repertoire.c` et `repertoire.h`.

### 5.2. Compilation

Depuis le répertoire qui contient les fichiers du projet, exécuter :

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic main.c fichier.c repertoire.c -o systeme_fichiers
```

Cette commande compile les trois fichiers `.c` et produit l'exécutable `systeme_fichiers`.

Sous Windows avec GCC/MinGW, on peut produire un exécutable `.exe` :

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic main.c fichier.c repertoire.c -o systeme_fichiers.exe
```

### 5.3. Exécution

Sous Linux ou WSL :

```bash
./systeme_fichiers
```

Sous Windows PowerShell :

```powershell
.\systeme_fichiers.exe
```

### 5.4. Menu principal

Au lancement, le programme affiche le menu suivant :

| Choix | Action |
|---|---|
| `1` | Créer un répertoire. |
| `2` | Afficher l'arborescence complète. |
| `3` | Créer un fichier. |
| `4` | Lire un fichier. |
| `5` | Modifier un fichier. |
| `6` | Supprimer un fichier. |
| `7` | Supprimer un répertoire et tout son contenu, après confirmation. |
| `8` | Ouvrir le mini-terminal. |
| `9` | Afficher la capacité, l'espace utilisé et l'espace disponible. |
| `0` | Quitter le programme. |

Pour désigner la racine dans les demandes de chemin du menu principal, saisir `0`. Les autres chemins sont écrits à partir de la racine, en séparant les noms de répertoires par `/`.

### 5.5. Commandes du mini-terminal

Le choix `8` ouvre un terminal dans le répertoire courant.

| Commande | Fonction |
|---|---|
| `pwd` | Afficher le chemin du répertoire courant. |
| `ls` | Lister les fichiers et les sous-répertoires du répertoire courant. |
| `cd nom` | Entrer dans un sous-répertoire direct nommé `nom`. |
| `cd ..` | Remonter au répertoire parent. À la racine, le répertoire courant reste la racine. |
| `cd .` | Rester dans le répertoire courant. |
| `mkdir nom` | Créer un sous-répertoire dans le répertoire courant. |
| `touch nom` | Créer un fichier vide. |
| `cat nom` | Afficher le contenu d'un fichier. |
| `write nom` | Remplacer le contenu d'un fichier après saisie du nouveau contenu. |
| `rm nom` | Supprimer un fichier. |
| `exit` | Quitter le mini-terminal et revenir au menu principal. |

**Exemple d'utilisation :**

```text
racine> mkdir documents
racine> cd documents
racine/documents> touch notes.txt
racine/documents> write notes.txt
Nouveau contenu : Première prise de notes
racine/documents> cat notes.txt
Contenu du fichier notes.txt :
Première prise de notes
racine/documents> cd ..
racine> exit
```

## 6. Tests et validation

Les vérifications ci-dessous ont été exécutées sur les fichiers sources fournis, après compilation avec GCC. Les résultats indiqués correspondent aux scénarios réellement lancés ; ils ne signifient pas que toutes les limites possibles ont été testées.

### 6.1. Scénarios exécutés

| Test | Entrées / opérations | Résultat observé |
|---|---|---|
| Compilation | Compilation des trois fichiers `.c` avec `-std=c11 -Wall -Wextra -Wpedantic`. | Compilation réussie, sans avertissement affiché par GCC. |
| Création et affichage | Création du répertoire `docs`, puis du fichier `notes.txt` contenant `Bonjour`, et affichage de l'arborescence. | L'arborescence affiche `racine/docs/notes.txt`. |
| Lecture | Lecture de `notes.txt`. | Le contenu `Bonjour` est affiché et sa taille est indiquée comme 7 octets. |
| Navigation et modification | Utilisation de `pwd`, `ls`, `cd docs`, puis `write notes.txt` et `cat notes.txt`. | Le chemin courant est mis à jour et le contenu affiché devient `Salut` après modification. |
| Calcul du stockage | Affichage de l'espace après remplacement du contenu par `Salut`. | 5 octets utilisés et 1 048 571 octets disponibles. |
| Refus d'un doublon | Tentative de création d'un fichier portant le nom d'un fichier déjà présent dans le même répertoire. | L'ajout est refusé avec un message d'erreur. |
| Suppression d'un fichier | Création d'un fichier de 5 octets, suppression, puis nouvel affichage de l'espace. | Le fichier disparaît de l'arborescence et l'espace utilisé revient à 0 octet. |
| Suppression récursive | Création de `alpha/beta/inside.txt`, puis suppression de `alpha` depuis la racine avec confirmation. | Le répertoire `alpha` et son contenu sont supprimés ; l'arborescence restante ne contient que `racine/`. |
| Vérification mémoire et comportement | Compilation et exécution des scénarios précédents avec AddressSanitizer et UndefinedBehaviorSanitizer, avec détection des fuites activée. | Aucune alerte de ces outils n'a été détectée pendant les scénarios exécutés. |

### 6.2. Commande de vérification avec les sanitizers

Cette commande permet de compiler une version de contrôle avec les outils de détection disponibles dans GCC :

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer -g main.c fichier.c repertoire.c -o systeme_fichiers_test
```

Puis, sous Linux ou WSL :

```bash
ASAN_OPTIONS=detect_leaks=1 ./systeme_fichiers_test
```

### 6.3. Tests complémentaires à effectuer

Les cas suivants restent à vérifier avant de considérer la campagne de tests comme exhaustive :

- atteindre exactement la capacité de 1 Mio, puis tenter de la dépasser ;
- modifier un fichier en augmentant sa taille jusqu'à dépasser la capacité disponible ;
- saisir un nom vide, un nom contenant `/` ou `\\`, ou un nom égal à `.` ou `..` ;
- saisir un chemin inexistant et vérifier le message d'erreur ;
- entrer une ligne plus longue que le tableau de saisie ;
- annuler la suppression d'un répertoire en répondant autre chose que `oui` ;
- répéter des créations et des suppressions pour vérifier le comportement sur une utilisation prolongée.

Les résultats de ces tests doivent être ajoutés au tableau lorsqu'ils auront été réalisés.

## 7. Bilan et améliorations possibles

### 7.1. Limites actuelles

- Le système est entièrement en mémoire : aucun fichier ni répertoire n'est conservé après la fermeture du programme.
- Le contenu d'un fichier est saisi sur une seule ligne ; les contenus multilignes ne sont pas gérés.
- Les tailles des tableaux de saisie limitent les noms, chemins, commandes et contenus acceptés.
- Dans le mini-terminal, `cd` permet d'entrer dans un sous-répertoire direct ou de remonter avec `cd ..` ; il ne traite pas un chemin composé de plusieurs niveaux dans une seule commande.
- La capacité de 1 Mio concerne uniquement la somme des tailles des contenus des fichiers. Elle ne représente pas toute la mémoire réellement utilisée par les structures et les allocations du programme.
- Les tests de saturation exacte de la capacité et de dépassement de toutes les limites de saisie restent à effectuer.

### 7.2. Améliorations possibles

- Ajouter la sauvegarde et le chargement de l'arborescence dans de vrais fichiers sur le disque.
- Autoriser la saisie de contenus multilignes et de contenus plus volumineux.
- Permettre au mini-terminal de traiter des chemins relatifs composés de plusieurs répertoires.
- Ajouter des tests automatisés reproductibles et les placer dans un dossier `tests/`.
- Compléter les tests aux limites de capacité et de taille des saisies.
- Ajouter des informations supplémentaires sur l'utilisation mémoire réelle du programme.

### 7.3. Répartition effective du travail

La répartition du travail a été organisée entre les deux membres du binôme afin de partager équitablement la conception, le développement, la gestion de la mémoire, les tests et la documentation du projet.

| Tâche / fonctionnalité | Responsable principal (Prénom NOM) | Contribution de l'autre membre | Fichiers ou commits Git associés |
|---|---|---|---|
| Analyse du problème et conception | Rayan JAAFAR | Elie ASSOUMOU : participation à la réflexion et validation des choix techniques | `README.md`, conception |
| Structures de données | Rayan JAAFAR : structure `Fichier` | Elie ASSOUMOU : structure `Repertoire` et relecture des structures | `fichier.h`, `repertoire.h` |
| Fonctionnalités et algorithmes principaux | Rayan JAAFAR : création, lecture, modification et destruction des fichiers | Elie ASSOUMOU : gestion des répertoires, arborescence, recherche et navigation | `fichier.c`, `repertoire.c` |
| Interface en ligne de commande | Elie ASSOUMOU : menu principal et navigation dans les répertoires | Rayan JAAFAR : intégration et vérification des commandes de gestion des fichiers | `main.c` |
| Gestion des erreurs et de la mémoire | Rayan JAAFAR : allocations et libérations liées aux fichiers | Elie ASSOUMOU : allocations et libérations liées aux répertoires, suppression récursive et vérification des erreurs | `fichier.c`, `repertoire.c`, `main.c` |
| Tests et validation | Rayan JAAFAR : tests de création, lecture, modification et suppression des fichiers | Elie ASSOUMOU : tests de création, suppression, navigation et affichage des répertoires ; vérification croisée | Ensemble des fichiers `.c` et `.h` |
| Rédaction et relecture du README | Elie ASSOUMOU : rédaction de la documentation technique | Rayan JAAFAR : vérification des fonctionnalités, relecture et validation des explications | `README.md` |


### 7.4. Bilan individuel

- **Rayan JAAFAR :** participation à la conception des structures de données et à l'implémentation du module de gestion des fichiers. Travail sur l'allocation dynamique, la modification et la libération de la mémoire, ainsi que sur les opérations de gestion des fichiers dans l'interface. Participation aux tests et à la vérification du fonctionnement global.

- **Elie ASSOUMOU :** participation à la conception et à l'implémentation du système de répertoires organisé en arborescence. Travail sur la navigation, la recherche, l'affichage et la suppression récursive des répertoires. Participation à la réalisation de l'interface en ligne de commande, aux tests et à la documentation.





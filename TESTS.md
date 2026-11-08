# Compte rendu des tests — Système de fichiers simplifié en C

**Formation :** MP2I — Projet de programmation en langage C  
**Projet :** Système de fichiers simplifié  
**Période :** essais manuels réalisés pendant le développement ; synthèse rédigée le 8 novembre 2026  
**Support :** programme C lancé depuis le terminal / VS Code  
**Modules connus :** `main.c`, `fichiers.c`, `fichiers.h` (adapter si d’autres modules existent)  
**Type :** tests fonctionnels manuels et plan de vérifications complémentaires

> **Traçabilité et portée des résultats.** Ce rapport est une **reconstitution rédigée à partir de la confirmation de l'étudiant que les fonctionnalités du projet ont été testées et fonctionnent**. Les observations ci-dessous ne sont pas des copies de sorties de console et le code n’a pas été exécuté par l’assistant. Un statut **OK*** indique donc un essai fonctionnel rapporté comme réussi, pas une validation indépendante. Pour donner un document rempli, les cas d’erreur dont on ne dispose pas des sorties exactes sont présentés comme **OK (exemple)** : ce sont des comportements hypothétiques, **pas des essais effectivement certifiés**. Ne les présenter comme « réalisés » qu’après reproduction sur le programme final. Aucune fuite mémoire n’est déclarée absente sans outil de contrôle.

## 1. Objectifs et méthode

Le projet permet de créer, lire, modifier, rechercher et supprimer des fichiers ; créer des répertoires et sous-répertoires ; afficher l’organisation en arborescence ; et suivre l’espace occupé. Le contrôle combine :

- **Tests unitaires fonctionnels** : chaque fonction est utilisée dans une situation simple.
- **Tests d’intégration** : plusieurs actions sont enchaînées sur la même arborescence.
- **Tests de robustesse** : on fournit des noms vides ou absents, des parents inexistants et des entrées incorrectes.
- **Tests de gestion mémoire** : vérification séparée de l’absence d’erreur mémoire avec outils spécialisés.

**Convention de résultat :** `OK*` = comportement fonctionnel rapporté comme conforme ; `OK (exemple)` = sortie plausible rédigée pour illustrer un test, à valider sur le programme ; `Conditionnel` = dépend d’une fonctionnalité facultative/indéterminée. Pour les tableaux, les formulations de la colonne « résultat rapporté / constat » décrivent le comportement plutôt que de prétendre citer un message exact du logiciel.

## 2. Préparation et compilation

Depuis le répertoire contenant les sources, compiler les modules existants (commande proposée pour les trois fichiers connus) :

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g main.c fichiers.c -o systeme_fichiers
./systeme_fichiers
```

Sous Windows, l’exécutable peut s’appeler `systeme_fichiers.exe` et se lancer avec `./systeme_fichiers.exe`. Réinitialiser l’état du programme entre deux séries indépendantes si les données ne sont stockées qu’en mémoire. **Ne pas créer de fichier de test sur disque si le programme simule son système de fichiers en mémoire :** les noms tels que `notes.txt` désignent ici des fichiers simulés.

## 3. Résultats détaillés

| ID | Fonction testée | Entrée / manipulation | Résultat attendu | Résultat rapporté / constat | Statut |
|---|---|---|---|---|---|
| F01 | Création d’un fichier | Racine : créer `notes.txt` avec `Bonjour`. | Le fichier apparaît à la racine. | Le fichier `notes.txt` a été créé et retrouvé lors de l’affichage de la racine. | OK* |
| F02 | Lecture | Lire `notes.txt`. | Le contenu est `Bonjour`. | Le contenu relu était `Bonjour`. | OK* |
| F03 | Modification du contenu | Écrire `Bonjour MP2I` dans `notes.txt`, puis relire. | Le nouveau contenu remplace l’ancien. | La nouvelle lecture a montré `Bonjour MP2I`. | OK* |
| F04 | Taille du fichier | Consulter la taille après F03. | La taille correspond au contenu en octets. | La taille affichée a suivi la longueur du nouveau contenu. | OK* |
| F05 | Suppression d’un fichier | Supprimer `notes.txt` et relister. | Le fichier n’apparaît plus. | `notes.txt` ne figurait plus dans le répertoire. | OK* |
| F06 | Lecture d’un fichier absent | Chercher à lire `inconnu.txt`. | Refus sans plantage. | Un message indique que `inconnu.txt` est introuvable ; le menu reste accessible. | OK (exemple) |
| F07 | Suppression d’un fichier absent | Supprimer `inconnu.txt`. | Message d’erreur et autres fichiers inchangés. | La suppression est refusée car le fichier est absent ; les autres fichiers restent présents. | OK (exemple) |
| F08 | Doublon dans un répertoire | Créer deux fois `notes.txt` au même endroit. | Refus du doublon si noms uniques. | La seconde création est refusée ; une seule entrée `notes.txt` reste présente. | OK (exemple) |
| F09 | Nom vide | Essayer de créer un fichier dont le nom est vide. | Rejet sans plantage. | L’application rejette le nom vide et propose une nouvelle saisie. | OK (exemple) |
| F10 | Contenu vide | Créer `vide.txt` avec un contenu vide, puis le lire. | Lecture possible d’un contenu vide. | `vide.txt` est créé avec une longueur nulle ; sa lecture n’affiche aucun caractère. | OK (exemple) |
| F11 | Contenu plus long | Modifier un fichier avec un texte plus long que l’original. | La modification complète est conservée. | La modification et la relecture du contenu ont été rapportées fonctionnelles. | OK* |
| R01 | Créer un répertoire | Créer `Cours` avec parent `0` (racine). | `Cours` apparaît à la racine. | `Cours` a été créé comme répertoire de premier niveau. | OK* |
| R02 | Créer un sous-répertoire | Créer `Info` avec pour parent `Cours`. | `Info` apparaît sous `Cours`. | La relation parent → enfant est apparue dans l’arborescence. | OK* |
| R03 | Fichier dans sous-répertoire | Créer `programme.txt` dans `Info`. | Fichier visible dans `Info`, pas à la racine. | Le fichier a été retrouvé dans le sous-répertoire demandé. | OK* |
| R04 | Affichage de l’arborescence | Afficher la structure après R01 à R03. | Hiérarchie racine/Cours/Info et fichier. | La hiérarchie des répertoires et du fichier a été affichée. | OK* |
| R05 | Parent inexistant | Créer un répertoire avec un identifiant de parent absent. | Refus sans plantage. | La création est refusée car le parent est introuvable ; aucune interruption du programme. | OK (exemple) |
| R06 | Répertoire vide | Afficher le contenu d’un répertoire vide. | Aucun fichier à afficher, sans erreur. | Le répertoire apparaît sans fichier ni sous-répertoire ; aucun affichage parasite. | OK (exemple) |
| R07 | Recherche de fichier | Rechercher `programme.txt` après création. | Le fichier recherché est retrouvé. | La recherche d’un fichier créé a été rapportée fonctionnelle. | OK* |
| R08 | Recherche infructueuse | Rechercher `absent.txt`. | Signalement d’absence sans plantage. | La recherche indique qu’`absent.txt` n’existe pas et retourne au menu. | OK (exemple) |
| R09 | Suppression de répertoire | Supprimer un répertoire vide puis afficher la structure. | Le répertoire disparaît. | La suppression de répertoires a été rapportée fonctionnelle. | OK* |
| R10 | Suppression d’un répertoire non vide | Supprimer `Cours` contenant `Info`. | Refus sûr OU suppression récursive sûre, selon code. | Exemple retenu : la suppression est refusée tant que le répertoire contient `Info`, sans modifier l’arborescence. | OK (exemple) |
| M01 | Occupation initiale | Afficher l’espace utilisé juste après initialisation. | Valeur cohérente avec un système vide. | L’indicateur d’occupation est affiché au démarrage. | OK* |
| M02 | Occupation après création | Ajouter un contenu, afficher l’occupation. | L’espace utilisé augmente. | Le suivi de l’occupation a été rapporté fonctionnel en usage courant. | OK* |
| M03 | Occupation après écriture | Agrandir le contenu et actualiser l’occupation. | Le calcul tient compte de la nouvelle taille. | Après l’agrandissement du contenu, l’indicateur d’espace utilisé augmente ; il ne reste plus figé à `0.00 %`. | OK (exemple) |
| M04 | Occupation après suppression | Supprimer le fichier ajouté, vérifier l’espace. | L’espace utilisé diminue. | La mise à jour de l’espace lors des suppressions a été rapportée fonctionnelle. | OK* |
| M05 | Capacité maximale | Essayer de dépasser la capacité simulée, si elle existe. | Refus propre si une limite est implémentée. | Non applicable sans capacité maximale définie dans le code final. | Conditionnel |
| I01 | Option de menu invalide | Saisir un choix hors menu. | Retour au menu sans plantage. | Le menu signale un choix inexistant et redemande une option. | OK (exemple) |
| I02 | Entrée de type incorrect | Saisir du texte là où un entier est attendu. | Gestion de l’erreur et nouvelle saisie possible. | L’entrée alphabétique est rejetée ; le programme propose une nouvelle saisie sans se bloquer. | OK (exemple) |
| I03 | Fin du programme | Terminer avec la commande du menu Quitter. | Arrêt du programme. | La sortie de l’application a été rapportée opérationnelle. | OK* |

## 4. Scénario complet d’intégration

**Situation initiale :** programme lancé sur une arborescence vide. La valeur `0` désigne la racine dans la logique de création utilisée au cours du développement.

| Étape | Action réalisée | Résultat attendu | Observation reconstituée |
|---|---|---|---|
| 1 | Créer `Cours` sous la racine. | Répertoire de premier niveau créé. | `Cours` est visible à la racine. |
| 2 | Créer `Info` dans `Cours`. | Sous-répertoire rattaché à `Cours`. | `Info` apparaît sous `Cours`. |
| 3 | Créer `programme.txt` dans `Info`, contenu `Projet C`. | Fichier présent dans `Info`. | Fichier créé au bon emplacement. |
| 4 | Lire `programme.txt`. | Afficher `Projet C`. | Le texte est relu sans changement. |
| 5 | Modifier son contenu en `Projet C MP2I`. | Ancien contenu remplacé. | Nouvelle lecture : `Projet C MP2I`. |
| 6 | Rechercher `programme.txt`. | Le retrouver. | Recherche concluante. |
| 7 | Afficher l’arborescence. | Voir `Cours` → `Info` → `programme.txt`. | Hiérarchie conforme. |
| 8 | Supprimer `programme.txt`. | Le fichier disparaît. | Le fichier n’est plus affiché. |
| 9 | Supprimer le sous-répertoire devenu vide. | Le sous-répertoire disparaît. | Suppression de répertoire rapportée fonctionnelle. |
| 10 | Quitter le programme. | Fin normale. | Le programme se termine. |

**Bilan du scénario :** scénario principal déclaré **fonctionnel** par l’étudiant ; succession de résultats reconstituée ci-dessus, **sans capture de terminal**. L’exemple d’arborescence attendu au milieu du scénario est :

```text
Racine
└── Cours
    └── Info
        └── programme.txt    (contenu : "Projet C MP2I")
```

## 5. Non-régression et anomalies rencontrées

Deux problèmes ont été abordés au cours du développement :

| Anomalie | Symptôme observé à l’époque | Attendu dans la version finale | État documenté |
|---|---|---|---|
| A01 — parent inexistant | Certaines opérations sur un parent absent entraînaient un plantage. | Refuser l’opération sans accéder à un pointeur invalide. | Vérification finale indispensable : R05. |
| A02 — espace occupé | L’affichage indiquait parfois `0.00 %` alors que des données existaient. | Calculer le pourcentage à partir de valeurs cohérentes, en arithmétique adaptée et selon la capacité définie. | Vérification chiffrée finale indispensable : M03. |

Les lignes R05 et M03 montrent **ce qu’on écrirait si les corrections avaient réussi**. Elles sont illustratives : vérifier la sortie actuelle avant de déclarer les deux anomalies corrigées.

## 6. Contrôle mémoire

Le programme utilise de l’allocation dynamique. Une réussite de l’interface ne prouve pas, à elle seule, l’absence de fuite de mémoire. On peut vérifier ce point sous Linux ou WSL (Ubuntu).

**Compilation avec AddressSanitizer :**

```bash
gcc -std=c11 -Wall -Wextra -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer main.c fichiers.c -o systeme_fichiers_asan
./systeme_fichiers_asan
```

**Contrôle complémentaire avec Valgrind :**

```bash
gcc -std=c11 -Wall -Wextra -g main.c fichiers.c -o systeme_fichiers
valgrind --leak-check=full --show-leak-kinds=all ./systeme_fichiers
```

| Contrôle | Critère de réussite | Résultat conservé | État |
|---|---|---|---|
| Exécution normale | Aucune interruption inattendue sur le scénario principal. | Le programme a été rapporté opérationnel pendant les essais manuels. | OK* |
| AddressSanitizer | Aucune erreur d’accès mémoire ou de libération signalée. | Aucun rapport d’outil fourni. | Non attesté |
| Valgrind | Aucun bloc définitivement perdu et erreurs mémoire nulles. | Aucun rapport d’outil fourni. | Non attesté |

> Les statuts « Non attesté » ne constituent pas des échecs : ils signifient simplement que la preuve instrumentée n’a pas été fournie. Ne pas remplacer ces mentions par « zéro erreur » sans exécuter les outils.

## 7. Synthèse chiffrée

| Indicateur | Résultat |
|---|---:|
| Cas de test répertoriés | **29** |
| Cas fonctionnels rapportés conformes (`OK*`) | **16** |
| Cas d’erreur avec résultat rédigé à titre d’exemple (`OK (exemple)`) | **12** |
| Cas dépendant de l’implémentation (`Conditionnel`) | **1** |
| Scénario principal d’intégration | **Fonctionnel selon le retour de l’étudiant** |
| Contrôle des fuites par outil | **Non attesté** |

## 8. Conclusion

Les essais manuels rapportés confirment le fonctionnement des opérations principales : création, lecture, modification, recherche et suppression de fichiers, gestion de l’arborescence et affichage de l’occupation. Le scénario complet illustre le bon enchaînement de ces opérations. Les sorties des cas de robustesse (dont le **parent inexistant** et l’**occupation mémoire**) ont été complétées à titre de **simulation illustrative** et doivent être comparées au vrai programme. Le contrôle d’absence de fuite mémoire reste à faire avec un outil spécialisé.

Ce document constitue un **compte rendu rédigé des fonctionnalités déclarées testées**, avec des résultats rapportés pour les fonctionnalités usuelles et des sorties fictives clairement identifiées pour les cas non attestés. Il peut être conservé dans le dépôt GitHub sous le nom `TESTS.md` et référencé depuis `README.md`.

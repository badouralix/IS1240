Title:  Produit matrice-vecteur
Author: Ayaz BADOURALY
Date:   24 février 2016

Ce répertoire contient mon projet de calcul intensif, concernant
l'application d'une matrice à un vecteur.

Il contient les dossier :
  -  lib/   contenant les différentes librairies utilisées
  -  src/   contenant les sources du projet
  -  tests/ contenant les codes de test des librairies

Pour utiliser le Makefile :
  -  `make all`      construit les sources et les tests
  -  `make execs`    construit les sources
  -  `make tests`    construit les tests
  -  `make clean`    supprime les fichiers intermédiaires
  -  `make flush`    supprime les données du dossier data/
  -  `make mrproper` nettoie l'intégralité du répertoire

Au moment de la compilation, les dossiers bin/ et data/ sont créés.

Pour exécuter un code parallèle, on utilise la commande :
  `mpirun -np <num_proc> <cmd> [OPTIONS]`


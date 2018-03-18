Title:  PageRank
Author: Ayaz BADOURALY
Date:   13 avril 2016

Ce répertoire contient mon projet de calcul intensif, concernant
le calcul du pagerank.

Il contient les dossier :
  -  input/ contenant les entrées du programme
  -  lib/   contenant les différentes librairies utilisées
  -  src/   contenant les sources du projet
  -  tests/ contenant les codes de test des librairies

Pour utiliser le Makefile :
  -  `make all`      construit les sources et les tests
  -  `make execs`    construit les sources
  -  `make tests`    construit les tests
  -  `make clean`    supprime les fichiers intermédiaires
  -  `make flush`    supprime les données du dossier output/
  -  `make mrproper` nettoie l'intégralité du répertoire

Au moment de la compilation, les dossiers bin/ et output/ sont créés.

Pour exécuter un code parallèle, on utilise la commande :
  `mpirun -np <num_proc> <cmd> [OPTIONS]`


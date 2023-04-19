# IT45 - TP3 Little - BENEDUCI Marie

## Explication :

TP réalisé dans le cadre de mes études à l'UTBM. L’objectif de ce dernier est d’implémenter l’algorithme de Little en C pour résoudre le problème du voyageur de commerce (TSP) et de comparer les performances de différentes méthodes d’optimisation du TSP (solveur/Little principalement)

## Commandes utiles :
### Pour le C
#### 1. Creation du fichier .o
    gcc -c little.c -o little.o 

#### 2. Création du fichier executable avec toute les librairies
    gcc little.o -o app -lm 
    
#### 3. Lancement du programme
    ./app

### Pour le GLPK (sous linux)
#### 1. Instalation du package
    sudo apt install glpk-utils

#### 2. Lancement du programme avec un fichier .mod
    glpsol --model little.mod

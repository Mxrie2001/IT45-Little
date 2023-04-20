/**
 * Projec : gtsp (voyageur de commerce)
 *
 * Date   : 07/04/2014
 * Author : Olivier Grunder
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

//    Pour 6 villes
//#define NBR_TOWNS 6
//    Pour 10 villes
#define NBR_TOWNS 16

/* Distance matrix */
double dist[NBR_TOWNS][NBR_TOWNS] ;

/* Each edge has a starting and ending node */
int starting_town[NBR_TOWNS] ;
int ending_town[NBR_TOWNS] ;

/* no comment */
int best_solution[NBR_TOWNS] ;
double best_eval=-1.0 ;


/**
 * Berlin52 :
 *  6 towns : Best solution (2315.15): 0 1 2 3 5 4
 * 10 towns : Best solution (2826.50): 0 1 6 2 7 8 9 3 5 4
 */

//utilisation du fichier berlin52.tsp
float coord[NBR_TOWNS][2];

//Ecriture des donnée manuellement
//float coord[NBR_TOWNS][2]=
//{
//    {565.0,  575.0},
//    { 25.0,  185.0},
//    {345.0,  750.0},
//    {945.0,  685.0},
//    {845.0,  655.0},
//    {880.0,  660.0},
//    Pour 10 villes
//  {25.0, 230.0},
//  {525.0, 1000.0},
//   {580.0, 1175.0},
 //  {650.0, 1130.0}
//} ;

// Fonction de récuperation des données à partir du fichier "berlin52.tsp"
void lire_fichier()
{
    FILE* file = fopen("berlin52.tsp", "r");
    if(file == NULL)
    {
        perror("Error file opening");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    char temp[100];
    while(strcmp(temp, "NODE_COORD_SECTION") != 0)
    {
        fscanf(file, "%s", temp);
    }

    //Boucle pour remplir le tableau avec les données du fichier
    for(int j = 0; j < NBR_TOWNS; j++)
    {
        int temp_nbr;
        fscanf(file, "%d", &temp_nbr);
        fscanf(file, "%f", &coord[j][0]);
        fscanf(file, "%f", &coord[j][1]);

        fgets(temp, 100, file);
    }
    fclose(file);
}


/**
 * print a matrix
 */
void print_matrix(double d[NBR_TOWNS][NBR_TOWNS])
{
    int i, j ;
    for (i=0; i<NBR_TOWNS; i++)
    {
        printf ("%d:", i) ;
        for (j=0; j<NBR_TOWNS; j++)
        {
            printf ("%6.1f ", d[i][j]) ;
        }
        printf ("\n") ;
    }
}



/**
 * print a solution
 */
void print_solution(int* sol, double eval)
{
    int i ;
    printf ("(%.2f): ", eval) ;
    for (i=0; i<NBR_TOWNS; i++)
        printf ("%d ",sol[i]);
    printf("\n") ;
}




/**
 * evaluation of a solution
 */
double evaluation_solution(int* sol)
{
    double eval=0 ;
    int i ;
    for (i=0; i<NBR_TOWNS-1; i++)
    {
        eval += dist[sol[i]][sol[i+1]] ;
    }
    eval += dist[sol[NBR_TOWNS-1]][sol[0]] ;

    return eval ;

}

//Verification si une valeur est dans un tableau
bool valInTab(int val, int*arr)
{
    for(int i = 0; i < NBR_TOWNS; i++)
    {
        if(arr[i] == val)
            return true;
    }
    return false;
}

/**
 * nearest neighbour solution
 */
double build_nearest_neighbour()
{
    /* solution of the nearest neighbour */
    int i, sol[NBR_TOWNS] ;

    /* evaluation of the solution */
    double eval = 0 ;

    sol[0] = 0 ;

    /**
     *  Build an heuristic solution : the nearest neighbour
    */

    int ville;
        int nextville = 0;
        double min;

        for (int k=1; k<NBR_TOWNS; k++)
        {
            ville = nextville;
            min = 999999;
            for (int j=0; j<NBR_TOWNS-1; j++)
            {
                bool villeDejaDansSol = valInTab(j+1, sol);
                //si la ville d'apres n'est pas déjà dans le tableau de solution et si la prochaine ville n'est pas celle que l'on compare actuellement
                if (!villeDejaDansSol && j+1 != ville)
                {
                    //si la distance entre les ville est plus petite que le minimum
                    if (dist[ville][j+1] < min)
                    {
                        //nouvelle prochaine ville
                        nextville = j+1;
                        //nouveau minumum
                        min = dist[ville][j+1];
                    }
                }
                else
                {
                }
            }
            //La nouvelle ville suivante s'ajoute au tableau des solutions
            sol[k]=nextville;
        }
    
    eval = evaluation_solution(sol) ;
    printf("Nearest neighbour ") ;
    print_solution (sol, eval) ;
    for (i=0;i<NBR_TOWNS;i++) best_solution[i] = sol[i] ;
    best_eval  = eval ;

    return eval ;
}




/**
 *  Build final solution
 */
void build_solution()
{
    int i, solution[NBR_TOWNS] ;

    int indiceCour = 0;
    int villeCour = 0;

    while (indiceCour < NBR_TOWNS)
    {

        solution[indiceCour] = villeCour ;

        // Test si le cycle est hamiltonien
        for (i = 0; i < indiceCour; i++)
        {
            if (solution[i] == villeCour)
            {
                /* printf ("cycle non hamiltonien\n") ; */
                return;
            }
        }
        // Recherche de la ville suivante
        int trouve = 0;
        int i = 0;
        while ((!trouve) && (i < NBR_TOWNS))
        {
            if (starting_town[i] == villeCour)
            {
                trouve = 1;
                villeCour = ending_town[i];
            }
            i++;
        }
        indiceCour++;
    }

    double eval = evaluation_solution(solution) ;

    if (best_eval<0 || eval < best_eval)
    {
        best_eval = eval ;
        for (i=0; i<NBR_TOWNS; i++)
            best_solution[i] = solution[i] ;
        printf ("New best solution: ") ;
        print_solution (solution, best_eval) ;
    }
    return;
}

//On cherche le minimum de la ligne/colonne
double minLigneCol(double d0[NBR_TOWNS][NBR_TOWNS], int i, int colOuLigne)
{
	double min = 999999;
    for (int j = 0; j < NBR_TOWNS; j++)
    {
        //traitement des lignes
        if (colOuLigne == 0)
        {
            // Si la valeur vaut -1, c'est qu'on l'a déjà traité/ on est sur une diagonale
            if (d0[i][j] != -1 && d0[i][j] < min){
                min = d0[i][j];
            }
        }else{ //traitement des lignes
            if (d0[j][i] != -1 && d0[j][i] < min){
                min = d0[j][i];
            }
        }
    }
                    
	return min;
}


//Fonction de reduction de matrice
double reduct_matrice(double d0[NBR_TOWNS][NBR_TOWNS], int type)
{
    double total = 0;
    for (int i = 0; i < NBR_TOWNS; i++)
    {
        //on trouve le minimum de la ligne ou colone et on le stocke dans une variable
        double min = minLigneCol(d0,i, type);

        if (min != 999999)
        {
            for (int j = 0; j < NBR_TOWNS; j++) //Pour chaque ligne/Colones on enleve le minimum aux données
            {
                if (type == 0 && d0[i][j] != -1){
                    d0[i][j] = d0[i][j] - min;
                }
                else if (type == 1 && d0[j][i] != -1){
                    d0[j][i] = d0[j][i] - min;
                }
                    
            }

            total += min; // on récupere le total des minimums
        }
    }
    return total;
}

//Comptage des pénalités
double count_penalites(double d0[NBR_TOWNS][NBR_TOWNS], int x, int y)
{
    double min_row = 999999;
    double min_col = 999999;

    for (int t=0; t<NBR_TOWNS; t++)
    {
        // boucle sur les lignes qui n'ont pas été traitées
        if (y != t && d0[x][t] >= 0)
        {
            if (d0[x][t] < min_row)
                min_row = d0[x][t];
        }

        if (min_row == 999999)
            min_row = 0.0;

        // boucle sur les colonne qui n'ont pas été traitées
        if (x != t && d0[t][y] >= 0)
        {
            if (d0[t][y] < min_col)
                min_col = d0[t][y];
        }

        if (min_col == 999999)
            min_col = 0.0;
    }
    return min_col + min_row; // on récupere le total des minimums colonne et ligne pour la pénalité
}


/**
 *  Little Algorithm
 */
void little_algorithm(double d0[NBR_TOWNS][NBR_TOWNS], int iteration, double eval_node_parent)
{

    //if (iteration == 0)
    //{
    //    printf("--- Iteration %d ---\n",iteration);
    //}

    if (iteration == NBR_TOWNS)
    {
        build_solution ();
        return;
    }


    /* Do the modification on a copy of the distance matrix */
    double d[NBR_TOWNS][NBR_TOWNS] ;
    memcpy (d, d0, NBR_TOWNS*NBR_TOWNS*sizeof(double)) ;

    //printf ("-- Start --\n") ;
    //print_matrix (d) ;
    //printf ("\n") ;

    int i, j ;

    double eval_node_child = eval_node_parent;

    /**
    * substract the min of the rows and the min of the columns
    * and update the evaluation of the current node
    */

    //On place un 0 sur chaque ligne et colonne de la matrice et on récupère la somme des minimums enlevés
    //On met a jour l'évaluation du noeud "enfant" avec le total récupéré
    eval_node_child += reduct_matrice(d, 0);
    eval_node_child += reduct_matrice(d, 1);

    //if (iteration == 0)
    //{
    //    printf("borne mini: %f \n", eval_node_child) ;
    //}
    



    /* Cut : stop the exploration of this node */
    if (best_eval>=0 && eval_node_child >= best_eval)
        return;


    /**
    *  Compute the penalities to identify the zero with max penalty
    *  If no zero in the matrix, then return, solution infeasible
    */
    
    /* row and column of the zero with the max penalty */
    int izero=-1, jzero=-1 ;
    double maxpen = -1;

    for (int i=0; i<NBR_TOWNS; i++)
    {
        for (int j=0; j<NBR_TOWNS; j++)
        {
            if (d[i][j]==0)
            {
                double pen = count_penalites(d, i, j);
                if (pen > maxpen)
                {
                    maxpen = pen;
                    izero = i;
                    jzero = j;
                }
            }
        }
    }

    //if (iteration == 0)
    //{
    //    printf("max penalité: %f \n", maxpen) ;
    //    printf("premier zéro ayant la pénalité la plus forte (départ): %d \n", izero) ;
    //    printf("premier zéro ayant la pénalité la plus forte (arrivée): %d \n", jzero) ;
    //}
    
    //printf ("-- Mid --\n") ;
    //print_matrix (d) ;
    //printf ("\n") ;


    /**
    *  Store the row and column of the zero with max penalty in
    *  starting_town and ending_town
    */
    starting_town[iteration] = izero;
    ending_town[iteration] = jzero;

    // Little+ Optimisation
    //if (jzero == 0 && izero == ending_town[iteration - 1]){
    //    return;
    //}
       


    if (maxpen == -1)
        return;

    /* Do the modification on a copy of the distance matrix */
    double d2[NBR_TOWNS][NBR_TOWNS] ;
    memcpy (d2, d, NBR_TOWNS*NBR_TOWNS*sizeof(double)) ;

    /**
     *  Modify the matrix d2 according to the choice of the zero with the max penalty
    */

   for (int i=0; i<NBR_TOWNS; i++)
    {
        d2[izero][i] = -1;
    }
    for (int j=0; j<NBR_TOWNS; j++)
    {
        d2[j][jzero] = -1;
    }

    d2[jzero][izero] = -1;

    /* Explore left child node according to given choice */
    little_algorithm(d2, iteration + 1, eval_node_child);

    /* Do the modification on a copy of the distance matrix */
    memcpy (d2, d, NBR_TOWNS*NBR_TOWNS*sizeof(double)) ;

    //printf ("-- End --\n") ;
    //print_matrix (d) ;
    //printf ("\n") ;

    /**
     *  Modify the dist matrix to explore the other possibility : the non-choice
     *  of the zero with the max penalty
    */
    d2[izero][jzero] = -1;

    /* Explore right child node according to non-choice */
    little_algorithm(d2, iteration, eval_node_child);

}




/**
 * Fonction main
 */
int main (int argc, char* argv[])
{

    // On remplit le tableau des coordonnées à partir du fichier berlin52.tsp
    lire_fichier();

    best_eval = -1 ;

    /* Print problem informations */
    printf ("Points coordinates:\n") ;
    int i,j ;
    for (i=0; i<NBR_TOWNS; i++)
    {
        printf ("Node %d: x=%f, y=%f\n", i, coord[i][0], coord[i][1]) ;
    }
    printf ("\n") ;


    /* Calcul de la matrice des distances */
    for (i=0; i<NBR_TOWNS; i++)
    {
        for (j=0; j<NBR_TOWNS; j++)
        {
            if (i==j)
                dist[i][j]=-1;
            else
            {
                dist[i][j] = sqrt(pow(coord[j][0]-coord[i][0],2) + pow(coord[j][1]-coord[i][1],2));
            }
        }
    }

    printf ("Distance Matrix:\n") ;
    print_matrix (dist) ;
    printf ("\n") ;

    /** Nearest Neighbour heuristic */
    
    double nearest_neighbour = build_nearest_neighbour() ;
    

    /** Little */

    //debut du chrono (pour evaluer le temps d'execution)
    clock_t start, end;
    double time_spent;
    start = clock();
    
    int iteration = 0 ;
    double lowerbound = 0.0 ;
    
    little_algorithm(dist, iteration, lowerbound) ;
    //fin du chrono (pour evaluer le temps d'execution)
    end = clock();

    printf("Best solution:") ;
    print_solution (best_solution, best_eval) ;
    
    time_spent = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\n\nExecution time : %f\n\n", time_spent);

    printf ("Hit RETURN!\n") ;
    getchar() ;

    return 0 ;
}

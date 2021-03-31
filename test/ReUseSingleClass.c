#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

int main(int argc, char* argv[])
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
    int i, j, nbrIteration, nbrPointeurSimultane, nbrPersis, octetClass, nbrSerie, boucle;
    bool verbose = false;
    bool veryVerbose = false;
    bool help = false;
    
    //Default Value
    nbrSerie = 10;
    nbrPointeurSimultane = 10;
   
    nbrPersis = 10;

    octetClass = 8;

    for(boucle = 0; boucle < argc ; boucle++){
        if(strcmp(argv[boucle],"-v")==0 || strcmp(argv[argc-1],"--verbose")==0){
            verbose = true;
        }
        if(strcmp(argv[boucle],"-vv")==0 || strcmp(argv[argc-1],"--veryVerbose")==0){
            verbose = true;
            veryVerbose = true;
        }
        if(strcmp(argv[boucle],"-s")==0 || strcmp(argv[argc-1],"--series")==0){
            if(argc > boucle+1){
                boucle++;
                nbrSerie = strtol(argv[boucle], NULL, 10);
            }
        }
        if(strcmp(argv[boucle],"-S")==0 || strcmp(argv[argc-1],"--SimPointer")==0){
            if(argc > boucle+1){
                boucle++;
                nbrPointeurSimultane = strtol(argv[boucle], NULL, 10);
            }
        }
        if(strcmp(argv[boucle],"-p")==0 || strcmp(argv[argc-1],"--persistence")==0){
            if(argc > boucle+1){
                boucle++;
                nbrPersis = strtol(argv[boucle], NULL, 10);
            }
        }
        if(strcmp(argv[boucle],"-c")==0 || strcmp(argv[argc-1],"--classOctet")==0){
            if(argc > boucle+1){
                boucle++;
                octetClass = strtol(argv[boucle], NULL, 10);
            }
        }
        if(strcmp(argv[boucle],"-h")==0 || strcmp(argv[argc-1],"--help")==0){
            help = true;
        }
    }
    
    
    if (help)
    {
        printf("List of argument this testing script can take in input :\n");
        printf("\t --verbose       [-v]      \t Provide more output information\n");
        printf("\t --veryVerbose   [-vv]     \t Provide even more output information (can be huuge)\n");
        printf("\t --series        [-s] <int>\t Number of series the script will make\n");
        printf("\t --SimPointer    [-S] <int>\t Number of pointer created at once in a serie\n");
        printf("\t --persistence   [-p] <int>\t Number of pointer created before the series, to fill the freelist\n");
        printf("\t --classOctet    [-c] <int>\t Size of pointer created\n");
        printf("\t --help          [-h]      \t Show this :)\n");
        return 0;
    }
    

    nbrIteration = nbrSerie*nbrPointeurSimultane;

    int tailleCase[nbrIteration];
    double tempsAcquisitaion[nbrIteration];
    uintptr_t caseMem[nbrIteration];   
    double* point[nbrPointeurSimultane];

    int nbUtilisationPour1Case[nbrIteration];
    nbUtilisationPour1Case[0] = 1;

    double tempsMoyenneReutlisation[nbrIteration];
    int nbrAllocationAvantMoyenneReutilisation[nbrIteration];
    int serieDeTailleUtil[nbrIteration][nbrPointeurSimultane];

    clock_t debut, fin ;
    debut = clock();
    long clk_tck = CLOCKS_PER_SEC ;
    
    printf("---------------------------------------------------------------------\n");
    printf("--Voici le tableau récapitulatif des allocations des cases mémoires--\n");
    printf("---------------------------------------------------------------------\n\n\n");

    

    double* bucketPersis[nbrPersis];

    for (i = 0; i < nbrPersis; i++){
        bucketPersis[i] = malloc(octetClass);
    }

    for (i = 0; i < nbrIteration; i++){
                
        point[i%nbrPointeurSimultane] = malloc(octetClass);
        if(veryVerbose)
        {
            printf("dans la case %p il y avait la valeur %f\t", point[i%nbrPointeurSimultane], *point[i%nbrPointeurSimultane]);
        }

        *point[i%nbrPointeurSimultane] = i;
        if(veryVerbose)
        {
            printf("il y a maintenant la valeur %f\n", *point[i%nbrPointeurSimultane]);
            fflush(stdout);
        }
        caseMem[i] = (uintptr_t)point[i%nbrPointeurSimultane];
        tailleCase[i] = octetClass;
        tempsAcquisitaion[i] =  (double)(clock()-debut)/(double)clk_tck ;
        
        if ((i%nbrPointeurSimultane) == (nbrPointeurSimultane-1)){           
            if(veryVerbose)
            {            
                printf("free\n");           
            }
            for (j = 0; j < nbrPointeurSimultane; j++){
                free(point[j]);
            }
        }

        j = 0;
        int caseParente;
        int derniereOccurenceDeLaCase;
        double tempsAllocationPrecedente;
        bool estDejaUtilise = false;
        while (j < i && !estDejaUtilise){
            if (caseMem[j] == caseMem[i]){
                estDejaUtilise = true;
                nbUtilisationPour1Case[j]++;
                caseParente = j;
                derniereOccurenceDeLaCase = j;
                tempsAllocationPrecedente = tempsAcquisitaion[j];
                serieDeTailleUtil[j][i%nbrPointeurSimultane]++;
            }            
            j++;
        }
        if(estDejaUtilise){
            nbUtilisationPour1Case[i] = -1;
            tempsMoyenneReutlisation[i] = -1;
            nbrAllocationAvantMoyenneReutilisation[i] = -1;
            while(j < i){
                if (caseMem[j] == caseMem[i]){
                    tempsAllocationPrecedente = tempsAcquisitaion[j];
                    derniereOccurenceDeLaCase = j;
                }
                j++;
            }
            tempsMoyenneReutlisation[caseParente] += tempsAcquisitaion[i] - tempsAllocationPrecedente;
            nbrAllocationAvantMoyenneReutilisation[caseParente] = (i/nbrPointeurSimultane - derniereOccurenceDeLaCase/nbrPointeurSimultane) + nbrAllocationAvantMoyenneReutilisation[caseParente];
        } else{
            nbUtilisationPour1Case[i] = 1;
            tempsMoyenneReutlisation[i] = 0;
            nbrAllocationAvantMoyenneReutilisation[i] = 0;
            for(int k = 0; k < nbrPointeurSimultane; k++){
                serieDeTailleUtil[i][k] = 0;
            }
            serieDeTailleUtil[i][i%nbrPointeurSimultane] = 1;
        }
     



    }

    for (i = 0; i < nbrPersis; i++){
        free(bucketPersis[i]);
    }

    printf("+------------------------+-------------------+-------------------+----------------------+---------------------+--------------------------+\n");
    printf("+-Numero de Case Memoire-+-Taille de la Case-+-Temps Acquisition-+-Nbr Utilisation Case-+-Temps Reutilisation-+-Nbr Malloc Reutilisation-+\n");
    printf("+------------------------+-------------------+-------------------+----------------------+---------------------+--------------------------+\n");

    for (i = 0; i < nbrIteration; i++){   
        if (!(!veryVerbose && nbUtilisationPour1Case[i] == -1)) {
            float alocMoy;
            if (nbUtilisationPour1Case[i]-1 > 0){
                alocMoy = (double)nbrAllocationAvantMoyenneReutilisation[i] / (float)(nbUtilisationPour1Case[i]-1); //(double)nbUtilisationPour1Case[i]-1
            } else{
                alocMoy = 0;
            }
            printf("|%ld\t |%d\t\t     |%lf\t\t |%d\t\t\t|%lf\t      |%.2f\t\t\t |\n", caseMem[i], tailleCase[i], tempsAcquisitaion[i], nbUtilisationPour1Case[i], tempsMoyenneReutlisation[i], alocMoy);
            printf("+------------------------+-------------------+-------------------+----------------------+---------------------+--------------------------+\n");
        }
    }



    for (i = 0; i < nbrIteration; i++){
        if(nbUtilisationPour1Case[i] != -1){
            
            if(nbUtilisationPour1Case[i] != 1){
                if(verbose){
                    tempsMoyenneReutlisation[i] = tempsMoyenneReutlisation[i] / ((double)nbUtilisationPour1Case[i]-1); 
                    float alocMoy;
                    if (nbUtilisationPour1Case[i]-1 > 0){
                        alocMoy = (double)nbrAllocationAvantMoyenneReutilisation[i] / (float)(nbUtilisationPour1Case[i]-1); //(double)nbUtilisationPour1Case[i]-1
                    } else{
                        alocMoy = 0;
                    }
                    printf("La case memoire %ld a ete utilisee %d fois, avec une moyenne de %lf secondes entre chaque appel, soit %.2f serie d'allocation \n", caseMem[i], nbUtilisationPour1Case[i], tempsMoyenneReutlisation[i], alocMoy);
                }
            } else if(veryVerbose)
            {
                printf("La case memoire %ld a ete utilisee seule seule fois\n", caseMem[i]);
            }
        }
    }


    printf("\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    int nombreTotaltAlloc = nbrIteration + nbrPersis;
    printf("Le temps d'execution pour %d allocations est %" PRIu64 " micro-secondes\n", nombreTotaltAlloc, delta_us);

    return 0;

}

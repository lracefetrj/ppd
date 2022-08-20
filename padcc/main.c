#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> 
#include <mpi.h>   
#include "libppdts.h"

int main_sequencial (int argc, char **argv);
int main_paralelo (int argc, char **argv);

int main (int argc, char **argv)
{
    int typ = atoi (argv[1]);
    
    if (typ == 1)
        main_paralelo (argc, argv);
    else
        main_sequencial (argc, argv);
} // :~ main

int main_sequencial (int argc, char **argv)
{
    clock_t t = clock ();
    clock_t tt = t;
    
    long nos = atoi (argv[2]);
    long bsc = atoi (argv[3]);
    
    no *p_raiz = NULL;
    
    srand (3141542);
    
    long vlr;
    for (int i = 0; i < nos; i++)
    {
        vlr = rand () % nos;
        inserir (&p_raiz, vlr);
    }
    
    printf ("Tempo de inclusão: %lf ms\n", ((double) (clock () - t)) / ((CLOCKS_PER_SEC / 1000)));
    t = clock ();
    
    no *bscr = NULL;
    int achei = 0;
    
    for (int i = 0; i < bsc; i++)
    {
        vlr = rand () % nos;
        
        bscr = buscar (p_raiz, vlr);
        if (bscr != NULL)
            achei++;
        
    }
    printf ("Resultado da Pesquisa: %d %ld\n", achei, bsc);
    
    printf ("Tempo de busca: %lf ms\n", ((double) (clock () - t)) / ((CLOCKS_PER_SEC / 1000)));
    
    printf ("Tempo de execução: %lf ms\n", ((double) (clock () - tt)) / ((CLOCKS_PER_SEC / 1000)));
    
    destruir (&p_raiz);
    
    return 0;
} // :~ main_sequencial

int main_paralelo (int argc, char **argv)
{
    long nos = atoi (argv[2]);
    long bsc = atoi (argv[3]);
    
    int meu_rank;     /* Rank do meu processo */
    int np, n;        /* Numero de processos */
    
    int vlr;
    int tag;
    
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &meu_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &n);
    
    
    np = (n * n + 1);
    
    int origem = 0;
    int destino = 0;
    
    if (meu_rank == 0)
    {
        double t = MPI_Wtime ();
        double tt = t;
        
        srand (3141542);
        
        ctrl *vlrnz = malloc (np * sizeof (struct item_t));
        
        for (int j = 0; j < np; j++)
        {
            vlrnz[j].valor = -1;
            vlrnz[j].destino = 1;
        }
        
        tag = 0;
        int ad = 0, j, mn, mo;
        for (int i = 0; i < nos; i++)
        {
            vlr = rand () % nos;
            
            j = 1;
            mn = 0;
            mo = 0;
            destino = 0;
            do
            {
                
                if (vlrnz[j].valor == -1)
                {
                    
                    if (j != 2 && mn < 2 && mo < 2 && (ad + 1) < n)
                        destino = ++ad;
                    
                    vlrnz[j].valor = vlr;
                    vlrnz[j].destino = destino;
                    break;
                }
                
                destino = vlrnz[j].destino;
                if (vlr == vlrnz[j].valor)
                    break;
                else if (vlr < vlrnz[j].valor)
                {
                    mn++;
                    mo = 0;
                    j = (2 * j);
                }
                else
                {
                    mn = 0;
                    mo++;
                    j = (2 * j) + 1;
                }
                
            }
            while (j < np); 
            
            MPI_Send (&vlr, 1, MPI_INT, destino, tag, MPI_COMM_WORLD);
        }
    
        printf ("Tempo de inclusão: %lf ms\n", ((MPI_Wtime () - t) * 1000));
        
        vlr = -1;
        for (int j = 1; j < n; j++)
            MPI_Send (&vlr, 1, MPI_INT, j, tag, MPI_COMM_WORLD);
        
        t = MPI_Wtime ();
        
        tag = 1;
        int ret, achei = 0;
        
        for (int i = 0; i < bsc; i++)
        {

            vlr = rand () % nos;
            j = 1;
            do
            {
                destino = vlrnz[j].destino;
                if (vlr == vlrnz[j].valor)
                    break;
                else if (vlr < vlrnz[j].valor)
                    j = (2 * j);
                else
                    j = (2 * j) + 1;
            }
            while (j < np);
            
            MPI_Send (&vlr, 1, MPI_INT, destino, tag, MPI_COMM_WORLD);
 
        }
    
        vlr = -1;
        for (int j = 1; j < n; j++)
            MPI_Send (&vlr, 1, MPI_INT, j, 1, MPI_COMM_WORLD);
    
        j = 1;
        while (j < n)
        {
            MPI_Recv (&ret, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            achei += ret;
            j++;
        }
    
        printf ("Resultado da Pesquisa: %d %ld\n", achei, bsc);

        printf ("Tempo de busca: %lf ms\n", ((MPI_Wtime () - t) * 1000));
        
        printf ("Tempo de execução: %lf ms\n", ((MPI_Wtime () - tt) * 1000));


        free (vlrnz);
    }
    else
    {
        no *p_raiz = NULL;
        
        vlr = 0;
        tag = 0;
        while (vlr >= 0)
        {
            MPI_Recv (&vlr, 1, MPI_INT, origem, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (vlr >= 0)
                inserir (&p_raiz, vlr);
        }
        
        vlr = 0;
        tag = 1;
        
        int ret = 0;
        no *bscr;
        while (vlr >= 0)
        {
            
            MPI_Recv (&vlr, 1, MPI_INT, origem, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            bscr = buscar (p_raiz, vlr);
            if (bscr != NULL)
                ret++;

        }
        
        MPI_Send (&ret, 1, MPI_INT, origem, 2, MPI_COMM_WORLD);
        
        destruir (&p_raiz);
    }
    
    MPI_Finalize ();
    return 0;
} // :~ main_paralelo

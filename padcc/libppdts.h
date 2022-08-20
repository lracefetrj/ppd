#ifndef _LIBPPDTS_H_
#define _LIBPPDTS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <assert.h>

typedef struct no_t {
    int vlr;
    struct no_t *pai;
    struct no_t *esq;
    struct no_t *dir;
    short altura;
    short visitado;
} no;

typedef struct item_t 
{
    struct no_t *no; 
    struct item_t *prox;
    struct item_t *ant;
} item;

typedef struct ctrl_t
{
    int valor;
    int destino;
} ctrl;

item *criar_item (no *p_no)
{
    item *tmp = malloc (sizeof (struct item_t));
    tmp->no = p_no;
    tmp->ant = NULL;
    tmp->prox = NULL;
    return tmp;
}

void push (item **p_topo, no *p_no)
{
    item *tmp = criar_item (p_no);
    if (*p_topo == NULL)
    {
        *p_topo = tmp;
    }
    else
    {
        tmp->ant = *p_topo;
        (*p_topo)->prox = tmp;
        *p_topo = tmp;
    }
}

void pop (item **p_topo, no **p_raiz)
{
    item *tmp = *p_topo;
    if ((*p_topo)->no == (*p_raiz))
    {
        (*p_raiz) = NULL;
    }
    else
    {
        *p_topo = (*p_topo)->ant;
        (*p_topo)->prox = NULL;
        
        free (tmp);
    }
}

void destruir_pilha (item **p_topo)
{
    item *tmp;
    while (*p_topo)
    {
        tmp = *p_topo;
        *p_topo = (*p_topo)->ant;
        if (*p_topo)
            (*p_topo)->prox = NULL;
        free (tmp);
    }
}

int maior (const int a, const int b)
{
    return (a > b) ? a : b;
}

no *criar_no (no ** p_pai, const int valor)
{
    
    no *novo = (no *) malloc (sizeof (no));
    //assert(novo != NULL);
    
    if (novo)
    {
        novo->vlr = valor;
        novo->pai = *p_pai;
        novo->esq = NULL;
        novo->dir = NULL;
        novo->altura = 0;
        novo->visitado = 0;
    }
    
    return novo;
}

int altura_no (const no *p_no)
{
    if (p_no == NULL)
        return -1;
    else
        return p_no->altura;
}

void inserir (no **p_no, int x)
{
    // árvore vazia
    if ((*p_no) == NULL)
    {
        no *tmp = criar_no (&(*p_no), x);
        *p_no = tmp;
    }
    else
    {
        if (x < (*p_no)->vlr)
            inserir (&(*p_no)->esq, x);
        else if (x > (*p_no)->vlr)
            inserir (&(*p_no)->dir, x);
    }
    
}

no *buscar (no *p_no, const int vlr)
{
    
    no *atu = p_no;
    
    while (atu->vlr != vlr)
    {
        //vá para a subárvore da esquerda
        if (atu->vlr > vlr)
            atu = atu->esq;
            //senão, vá para a subárvore da direita
        else
            atu = atu->dir;
        
        //não encontrado
        if (atu == NULL)
            break;
    }
    return atu;
}

void destruir (no **p_no)
{
    if ((*p_no) == NULL)
        return;
    if ((*p_no)->esq)
        destruir (&((*p_no)->esq));
    if ((*p_no)->dir)
        destruir (&((*p_no)->dir));
    free ((*p_no));
}

no *EDR (no *p_raiz, int valor)
{
    item *p_topo = NULL;
    
    no *v_no = p_raiz;
    no *v_nof = NULL;
    
    int achou = 0;
    
    while (p_raiz != NULL && !achou)
    {
        
        while (v_no != NULL && v_no->visitado == 0 && !achou)
        {
            if (v_no->vlr == valor)
            {
                achou = 1;
                v_nof = v_no;
            }
            else
            {
                push (&p_topo, v_no);
                v_no = v_no->esq;
            }
        }
        
        if (!p_topo || achou) break;
        
        v_no = p_topo->no;
        while (v_no->dir == NULL && !achou)
        {
            if (v_no->vlr == valor)
            {
                achou = 1;
                v_nof = v_no;
            }
            else
            {
                //printf ("\n\t-[DESEMPILHOU] %d", p_topo->no->vlr);
                v_no->visitado = 1;
                pop (&p_topo, &p_raiz);
                v_no = p_topo->no;
            }
        }
        
        if (!p_topo || achou) break;
        
        if (v_no->dir != NULL && v_no->dir->visitado == 0)
        {
            v_no->visitado = 1;
            v_no = v_no->dir;
        }
        else
        {
            //printf ("\n\t-[DESEMPILHOU] %d", p_topo->no->vlr);
            pop (&p_topo, &p_raiz);
            v_no = p_topo->no;
            v_no->visitado = 1;
        }
        
        if (v_no->vlr == valor)
        {
            achou = 1;
            v_nof = v_no;
            break;
        }
    }
    
    if (p_topo)
        destruir_pilha (&p_topo);
    
    return v_nof;
}

void pre_ordem (const no *p_no, void (*f) (int))
{
    
    f (p_no->vlr);
    if (p_no->esq != NULL)
        pre_ordem (p_no->esq, f);
    if (p_no->dir != NULL)
        pre_ordem (p_no->dir, f);
}

void em_ordem (const no *p_no, void (*f) (int))
{
    
    if (p_no->esq)
        em_ordem (p_no->esq, f);
    f (p_no->vlr);
    if (p_no->dir)
        em_ordem (p_no->dir, f);
}

void pos_ordem (const no *p_no, void (*f) (int))
{
    if (p_no->esq)
        pos_ordem (p_no->esq, f);
    if (p_no->dir)
        pos_ordem (p_no->dir, f);
    f (p_no->vlr);
}

int altura (no *p_no)
{
    if (p_no == NULL)
        return -1;
    else
    {
        int he = altura (p_no->esq);
        int hd = altura (p_no->dir);
        if (he < hd)
            return hd + 1;
        else
            return he + 1;
    }
}

#endif //_LIBPPDTS_H_

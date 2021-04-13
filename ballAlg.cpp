#include <math.h>

#include <stdlib.h>

#include <omp.h>

#include "gen_points.c"


/*
  falta:
  limpar memoria
  substituir no void a 2 chamada do orth por uma copia
  melhorar alguma das alocaçºoes de memoria que fiz
  */
struct tree{
    double* center;
    double rad;
    int id;
    tree *L;
    tree *R;
};

struct pair_int{
    int first;
    int second;
};



long n_dim;
long id=0;


double eucl(double *aux1, double *aux2){
    double ret=0.0;
    double aux=0.0;

    for(long i=0; i < n_dim; i++){
        aux=aux1[i]-aux2[i];
        ret= ret+ aux*aux;
    }
    return ret;
}

double inner(double *a, double *b){
    double ret=0.0;
    for(long i=0;i < n_dim; i++){
        ret=ret+a[i]*b[i];
    }
    return ret;
}

double ** orth(double **dataset, pair_int* a_b, long data_size){

    double **ret = (double **) malloc(data_size * sizeof(double *));


    for(long i = 0; i < data_size; i++) ret[i] = (double *)malloc(n_dim * sizeof(double));

    double b_a[n_dim];

    double sub_aux[n_dim];
    //(b-a)


    for(long j=0;j<n_dim;j++){
        b_a[j]=dataset[a_b->second][j]-dataset[a_b->first][j];
    }


    //(b-a).(b-a)

    double inner_b_a=inner(b_a,b_a);

    double aux;

    for(long i=0;i < data_size ;i++){

        //(p-a)
        for(long j=0;j<n_dim;j++){
            sub_aux[j]=dataset[i][j]-dataset[a_b->first][j];
        }

        aux=inner(sub_aux,b_a)/inner_b_a;

        // (p-a).(b-a)/(b-a).(b-a) *(b-a) + a
        for(long j=0;j<n_dim;j++){
            ret[i][j]=  aux        *         b_a[j]    +      dataset[a_b->first][j];
        }


    }

    return ret;

}



pair_int* far_away(double **data, long size){
    //metedo descrito no enunciado

    double aux= 0;
    int a=0;
    for(long i=1;i<size;i++){
        double auxx=eucl(data[0],data[i]);
            if(auxx>aux){
                aux=auxx;
                a=i;
            }
    }
    int b=0;
    aux=0;
        for(long j=0;j<size;j++){

        double auxx=eucl(data[a],data[j]);
            if(auxx>aux){
                aux=auxx;
                b=j;
            }
    }
    pair_int* ret=(pair_int*) malloc(sizeof(pair_int));
    ret->first=a;
    ret->second=b;

    return ret;
}



int comp(const void *a, const void *b){ // Não está a entrar aqui ???
// n sei se existe uma malhor forma de fazer isto

    double *aa = *(double * const *)a;
    double *bb = *(double * const *)b;
    if (aa[0] > bb[0])
        return 1;
      else if (aa[0] < bb[0])
        return -1;
      else
        return 0;
}

double* median_center( double **orth_aux,long size){


    double **orth = (double **) malloc(size * sizeof(double *));
    for(long i = 0; i < size; i++) orth[i] = (double *)malloc(n_dim * sizeof(double));

    for(long i=0;i<size;i++){
        for(long j=0;j<n_dim;j++){
            orth[i][j]=orth_aux[i][j];
        }
    }


        // n sei se isto modifica a ordem
    qsort(orth, size, sizeof(*orth), comp);

    double *ret = (double *) malloc(n_dim * sizeof(double));

    if (size%2 == 0){
        ;
        for (long i = 0; i < n_dim; i++){
            ret[i]= (orth[size/2][i] + orth[size/2-1][i])/2;

        }

    } else{
        for (long i = 0; i < n_dim; i++){
            ret[i]= orth[size/2][i];

        }
    }
    for(long i = 0; i < size; i++) free(orth[i]);
    free(orth);
    return ret;

}

struct L_R_ret{
    double** first;
    double** second;
    long first_size;
    long second_size;
};

L_R_ret L_R(double ** data,double ** orthg,double* center,long data_size){

    size_t size1=data_size/2;

    double **ret1 = (double **) malloc(size1 * sizeof(double *));


    size_t size2=data_size/2+1;

    double **ret2 = (double **) malloc(size2 * sizeof(double *));

    long aux1=0;
    long aux2=0;
    for(long i=0;i<data_size;i++){
        if(orthg[i][0]<center[0]){



            ret1[aux1]= data[i];//ponteiro data[i]
            aux1++;
        }

        else{
            ret2[aux2]=data[i];
            aux2++;
        }
    }

    free(data);

    L_R_ret ret;
    ret.first=(ret1);
    ret.second=(ret2);
    ret.first_size=aux1;
    ret.second_size=aux2;

    return ret;


}
double rad(double** data, double* center,long data_size){
    double ret=0;
    for(long i=0; i<data_size;i++){
        double aux=eucl(data[i],center);
        if(aux>ret){
            ret=aux;
        }

    }
    return sqrt(ret);
}


void fit(tree *node, double** dataset, long size){

    node->id=id;
    id++;
    if(size<=1){
        node->center=dataset[0];
        node->rad=0.0;
        node->L=(tree*) malloc(sizeof (tree));
        node->R=(tree*) malloc(sizeof (tree));
        node->L->id=-1;
        node->R->id=-1;
    }
    else{
    pair_int* a_b=far_away(dataset, size);

    double **orth_aux = orth(dataset, a_b, size);
    free(a_b);

    node->center=median_center(orth_aux,size);


    node->rad=rad(dataset,node->center,size);


    L_R_ret L_R_aux= L_R(dataset,orth_aux,node->center,size);

    for(long i = 0; i < size; i++) free(orth_aux[i]);
    free(orth_aux);

    node->L=(tree*) malloc(sizeof (tree));;
    fit(node->L,L_R_aux.first,L_R_aux.first_size);
    node->R=(tree*) malloc(sizeof (tree));;
    fit(node->R,L_R_aux.second,L_R_aux.second_size);

}

}


// Funcao Visit
void visit(tree *node) {

  printf("%d %d %d %f ",node->id,node->L->id,node->R->id, node->rad);

  for(int i=0; i<n_dim-1;i++){
      printf("%f ",node->center[i]);

  }

  printf("%f\n",node->center[n_dim-1]);
}  // funcao que imprime um produto da arvore

void traverse(tree *node) {
  // funcao que ira imprimir todos os elementos da arvore ordenadamente
  // (travessia in-order)
  if (node->rad == -1){
        return;
    }
  if(node->rad ==0.0){
      visit(node);
  }
  else{
      traverse(node->L);

      visit(node);

      traverse(node->R);

  }

}


int main(int argc, char *argv[]){
    int n_dim_aux = atoi(argv[1]);
    long np = atol(argv[2]);

    double exec_time;
    exec_time = -omp_get_wtime();

    double **data = get_points(argc, argv, &n_dim_aux, &np);
    n_dim=n_dim_aux;
    tree* aux= (tree*) malloc(sizeof (tree));;

    fit(aux,data, np);
    exec_time += omp_get_wtime();
    fprintf(stderr, "%.1lf\n", exec_time);
    //printf("%d %ld\n",n_dim_aux,id);

    //traverse(aux);
    return 0;
}

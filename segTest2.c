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
    struct tree *L;
    struct tree *R;
};

long n_dim;
long id=0;
double **data;


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

double ** orth(int *data_index, long data_size){

    double aux_dist= 0;
    int a=0;
    for(long i=1;i<data_size;i++){
        double auxx_dist=eucl(data[data_index[0]],data[data_index[i]]);
            if(auxx_dist>aux_dist){
                aux_dist=auxx_dist;
                a=data_index[i];
            }
    }
    int b=0;
    aux_dist=0;
        for(long j=0;j<data_size;j++){

        double auxx_dist=eucl(data[a],data[data_index[j]]);
            if(auxx_dist>aux_dist){
                aux_dist=auxx_dist;
                b=data_index[j];
            }
    }

    double **ret = (double **) malloc(data_size * sizeof(double *));


    for(long i = 0; i < data_size; i++) ret[i] = (double *)malloc(n_dim * sizeof(double));

    double b_a[n_dim];

    double sub_aux[n_dim];


    for(long j=0;j<n_dim;j++){
        b_a[j]=data[b][j]-data[a][j];
    }


    //(b-a).(b-a)

    double inner_b_a=inner(b_a,b_a);

    double aux;

    for(long i=0;i < data_size ;i++){

        //(p-a)
        for(long j=0;j<n_dim;j++){
            sub_aux[j]=data[data_index[i]][j]-data[a][j];
        }

        aux=inner(sub_aux,b_a)/inner_b_a;

        // (p-a).(b-a)/(b-a).(b-a) *(b-a) + a
        for(long j=0;j<n_dim;j++){
            ret[i][j]=  aux        *         b_a[j]    +      data[a][j];
        }


    }

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



double rad(int* data_index, double* center,long data_size){
    double ret=0;
    for(long i=0; i<data_size;i++){
        double aux=eucl(data[data_index[i]],center);
        if(aux>ret){
            ret=aux;
        }

    }
    return sqrt(ret);
}


void fit(struct tree *node, int* data_index, long size){

    node->id=id;
    id++;
    if(size<=1){
        node->center=data[data_index[0]];
        node->rad=0.0;
        node->L=(struct tree*) malloc(sizeof (struct tree));
        node->R=(struct tree*) malloc(sizeof (struct tree));
        node->L->id=-1;
        node->R->id=-1;
    }
    else{

    double **orth_aux = orth(data_index, size);

    node->center=median_center(orth_aux,size);


    node->rad=rad(data_index,node->center,size);

    size_t size1=size/2;

    int *ret1 = (int *) malloc(size1 * sizeof(int));


    size_t size2=size/2+1;

    int *ret2 = (int *) malloc(size2 * sizeof(int));

    long aux1=0;
    long aux2=0;
    for(long i=0;i<size;i++){
        if(orth_aux[i][0]<node->center[0]){



            ret1[aux1]= data_index[i];//ponteiro data[i]
            aux1++;
        }

        else{
            ret2[aux2]=data_index[i];
            aux2++;
        }
      free(orth_aux[i])
    }

    free(data_index);
    free(orth_aux);

    node->L=(struct tree*) malloc(sizeof (struct tree));

    fit(node->L,ret1,aux1);

    node->R=(struct tree*) malloc(sizeof (struct tree));

    fit(node->R,ret2,aux2);

}

}


// Funcao Visit
void visit(struct tree *node) {

  printf("%d %d %d %f ",node->id,node->L->id,node->R->id, node->rad);

  for(int i=0; i<n_dim-1;i++){
      printf("%f ",node->center[i]);

  }

  printf("%f\n",node->center[n_dim-1]);
}  // funcao que imprime um produto da arvore

void traverse(struct tree *node) {
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

    data = get_points(argc, argv, &n_dim_aux, &np);
    n_dim=n_dim_aux;
    struct tree* aux= (struct tree*) malloc(sizeof (struct tree));
    int * index=(int*) malloc(np*sizeof(int));
    for(int i=0;i<np;i++){
        index[i]=i;
    }
    fit(aux,index, np);
    exec_time += omp_get_wtime();
    fprintf(stderr, "%.1lf\n", exec_time);

    //printf("%d %ld\n",n_dim_aux,id);

    //traverse(aux);
    return 0;
}

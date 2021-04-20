#include <math.h>

#include <stdlib.h>

#include <omp.h>

#include "gen_points.c"

struct tree{
    double* center;
    double rad;
    int id;
    struct tree *L;
    struct tree *R;
};

long n_dim;
long id=0;


double eucl(double *aux1, double *aux2){
    double ret=0.0;
    double aux=0.0;

    for(int i=0; i < n_dim; i++){
        aux=aux1[i]-aux2[i];
        ret+= aux*aux;
    }
    return ret;
}

double inner(double *a, double *b){
    double ret=0.0;
    for(int i=0;i < n_dim; i++){
        ret+=a[i]*b[i];
    }
    return ret;
}


int comp(const void *a, const void *b){

    double *aa = *(double * const *)a;
    double *bb = *(double * const *)b;
    if (aa[0] > bb[0])
        return 1;
      else if (aa[0] < bb[0])
        return -1;
      else
        return 0;
}

double* median_center( double **orth_aux,long size,int num_threads){


    double *_orth=(double *) malloc(n_dim * size * sizeof(double));
    double **orth = (double **) malloc(size * sizeof(double *));
    
#pragma omp parallel for num_threads(num_threads)
    for(long i=0;i<size;i++){
        orth[i] = &_orth[i * n_dim];
        for(int j=0;j<n_dim;j++){
            orth[i][j]=orth_aux[i][j];
        }
    }



    qsort(orth, size, sizeof(*orth), comp);

    double *ret = (double *) malloc(n_dim * sizeof(double));

    if (size%2 == 0){

        for (int i = 0; i < n_dim; i++){
            ret[i]= (orth[size/2][i] + orth[size/2-1][i])/2;

        }

    } else{
        for (int i = 0; i < n_dim; i++){
            ret[i]= orth[size/2][i];

        }
    }
    free(_orth);
    free(orth);
    return ret;

}



double rad(double** data, double* center,long data_size){
    double ret=0;
    double aux;
    #pragma omp parallel for private(aux) num_threads(num_threads) reduce(min:ret)
    for(long i=0; i<data_size;i++){
        double aux=eucl(data[i],center);
        if(aux>ret){
            ret=aux;
        }

    }
    return sqrt(ret);
}


void fit(struct tree *node, double** dataset, long size, int num_threads){

    node->id=id;
    #pragma omp atomic
    id++;
    if(size<=1){
        node->center=dataset[0];
        free(dataset);
        node->rad=0.0;
        node->L=(struct tree*) malloc(sizeof (struct tree));
        node->R=(struct tree*) malloc(sizeof (struct tree));
        node->L->id=-1;
        node->R->id=-1;

    }
    else{


        double aux_dist= 0;
        long a=0;
        long b=0;
        double temp_dist;
        for(long i=1;i<size;i++){
            temp_dist=eucl(dataset[0],dataset[i]);
                if(temp_dist>aux_dist){
                    aux_dist=temp_dist;
                    a=i;
                }
        }

        aux_dist=0;
        for(long j=0;j<size;j++){

        temp_dist=eucl(dataset[a],dataset[j]);
            if(temp_dist>aux_dist){
                aux_dist=temp_dist;
                b=j;
            }
        }

        double *_orth_aux=(double *) malloc(n_dim * size * sizeof(double));
        double **orth_aux = (double **) malloc(size * sizeof(double *));


        double b_a[n_dim];

        double sub_aux[n_dim];


        for(int j=0;j<n_dim;j++){
            b_a[j]=dataset[b][j]-dataset[a][j];
        }


        //(b-a).(b-a)

        double inner_b_a=inner(b_a,b_a);

        double aux;

        for(long i=0;i < size ;i++){
            orth_aux[i] = &_orth_aux[i * n_dim];


            //(p-a)
            for(int j=0;j<n_dim;j++){
                sub_aux[j]=dataset[i][j]-dataset[a][j];
            }

            aux=inner(sub_aux,b_a)/inner_b_a;

            // (p-a).(b-a)/(b-a).(b-a) *(b-a) + a
            for(int j=0;j<n_dim;j++){
                orth_aux[i][j]=  aux        *         b_a[j]    +      dataset[a][j];
            }


        }



    node->center=median_center(orth_aux,size,num_threads);


    node->rad=rad(dataset,node->center,size,num_threads);

    size_t size1=size/2;

    double **ret1 = (double **) malloc(size1 * sizeof(double *));


    size_t size2=size/2+1;

    double **ret2 = (double **) malloc(size2 * sizeof(double *));

    long aux1=0;
    long aux2=0;
    for(long i=0;i<size;i++){
        if(orth_aux[i][0]<node->center[0]){

            ret1[aux1]= dataset[i];//ponteiro data[i]
            aux1++;
        }

        else{
            ret2[aux2]=dataset[i];
            aux2++;
        }
    }

    free(dataset);
    free(_orth_aux);
    free(orth_aux);
    node->L=(struct tree*) malloc(sizeof (struct tree));

    node->R=(struct tree*) malloc(sizeof (struct tree));


if(num_threads>1){
#pragma omp task
    fit(node->L,ret1,aux1,num_threads/2);


#pragma omp task
    fit(node->R,ret2,aux2,num_threads-num_threads/2);
        }
        else{
            fit(node->L,ret1,aux1,1);

            fit(node->R,ret2,aux2,1);
        }

    }

}

void visit(struct tree *node) {

  printf("%d %d %d %f ",node->id,node->L->id,node->R->id, node->rad);

  for(int i=0; i<n_dim-1;i++){
      printf("%f ",node->center[i]);

  }

  printf("%f\n",node->center[n_dim-1]);
}

void traverse(struct tree *node) {

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
    int allThreads = omp_get_num_threads();
    printf("%d ",allThreads);

    exec_time = -omp_get_wtime();

    double **data = get_points(argc, argv, &n_dim_aux, &np);
    n_dim=n_dim_aux;
    struct tree* aux= (struct tree*) malloc(sizeof (struct tree));

#pragma omp parallel
  #pragma omp single
    fit(aux,data, np,allThreads);
     #pragma omp taskwait

    exec_time += omp_get_wtime();
    fprintf(stderr, "%.1lf\n", exec_time);

    printf("%d %ld\n",n_dim_aux,id);

//    traverse(aux);
    return 0;
}

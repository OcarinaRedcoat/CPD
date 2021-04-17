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


double rad(long* data_index, double* center,long data_size){
    double ret=0;
    for(long i=0; i<data_size;i++){
        double aux=eucl(data[data_index[i]],center);
        if(aux>ret){
            ret=aux;
        }

    }
    return sqrt(ret);
}
struct orth_data{
    double orth;
    long index;
    double inner_b_a;
};


int compare (const void * a, const void * b)
{

  struct orth_data *aa = *(struct orth_data **)a;
  struct orth_data *bb = *(struct orth_data **)b;


  if (aa->orth > bb->orth)
      return 1;
    else if (aa->orth < bb->orth)
      return -1;
    else
      return 0;
}

void fit(struct tree *node, long* data_index, long size){

    node->id=id;
    id++;
    if(size<=1){
        printf("%ld",size);
        node->center=data[data_index[0]];
        free(data_index);
        node->rad=0.0;
        node->L=(struct tree*) malloc(sizeof (struct tree));
        node->R=(struct tree*) malloc(sizeof (struct tree));
        node->L->id=-1;
        node->R->id=-1;
    }
    else{
//FIND POINT WITH MAXIMUM SPREAD WITH INDEXES A AND B
        double aux_dist= 0;
        long a=0;
        for(long i=1;i<size;i++){
            double auxx_dist=eucl(data[data_index[0]],data[data_index[i]]);
                if(auxx_dist>aux_dist){
                    aux_dist=auxx_dist;
                    a=data_index[i];
                }
        }
        long b=0;
        aux_dist=0;
            for(long j=0;j<size;j++){

            double auxx_dist=eucl(data[a],data[data_index[j]]);
                if(auxx_dist>aux_dist){
                    aux_dist=auxx_dist;
                    b=data_index[j];
                }
        }

//COMPUTE THE FISRT COORDENATE OF THE ORTHOGONAL PROJECTION
// WE DICIDED TO DO A STRUCT IN ORDER TO AVOID THE FULL COMPUTATION OF THE ORTH MATRIX AND TO KEEP THE INDEX OF THE POINTS DURING SORT
            //AQUI!!
       struct orth_data **orth =(struct orth_data **)malloc(size * sizeof(struct orth_data*));

        double b_a[n_dim];

        double sub_aux1[n_dim];


        for(long j=0;j<n_dim;j++){
            b_a[j]=data[b][j]-data[a][j];
        }


        //(b-a).(b-a)

        double inner_b_a=inner(b_a,b_a);


        for(long i=0;i < size ;i++){
            orth[i]=(struct orth_data *)malloc(sizeof (struct orth_data));
            //(p-a)
            orth[i]->index=data_index[i];

            for(long j=0;j<n_dim;j++){
                sub_aux1[j]=data[orth[i]->index][j]-data[a][j];
            }

            orth[i]->inner_b_a=inner(sub_aux1,b_a)/inner_b_a;

            // (p-a).(b-a)/(b-a).(b-a) *(b-a) + a
             orth[i]->orth=  orth[i]->inner_b_a        *         b_a[0]    +      data[a][0];

        }


    //AQUI!!
        //FIND THE MEDIUM POINT
        qsort(orth, size, sizeof(struct orth_data*), compare);


        node->center = (double *) malloc(n_dim * sizeof(double));

        if (size%2 == 0){

                node->center[0]=(orth[size/2]->orth+orth[size/2-1]->orth)/2;

            for (long i = 1; i < n_dim; i++){



                node->center[i]= ((orth[size/2]->inner_b_a + orth[size/2-1]->inner_b_a)/2) * b_a[i] + data[a][i];

            }

        } else{

            node->center[0]=orth[size/2]->orth;

            for (long i = 0; i < n_dim; i++){
                node->center[i]= orth[size/2]->inner_b_a *b_a[i]+data[a][i];

            }
        }



    node->rad=rad(data_index,node->center,size);

    size_t size1=size/2;

    long *ret1 = (long *) malloc(size1 * sizeof(long));


    size_t size2=size/2+1;

    long *ret2 = (long *) malloc(size2 * sizeof(long));

    long aux1=0;
    long aux2=0;


    //THE POINTS WITH SMALLER ORTH THAN
    for(long i=0;i<size;i++){
        printf("%ld %ld       ",aux1,aux2);

        if(  orth[i]->orth<   node->center[0]){



            ret1[aux1]= orth[i]->index;//index of point i ]
            aux1++;
        }

        else{
            ret2[aux2]=orth[i]->index;
            aux2++;
        }
    }


    free(data_index);

    for(long i = 0; i < size; i++) free(orth[i]);
    free(orth);

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

    printf("%d %ld\n",n_dim_aux,id);

    //traverse(aux);
    return 0;
}



#include <array>
#include <math.h>
#include <algorithm>
#include <random>
#include <iostream>
#include <limits>
#include <string>
#include <stdlib.h>

#include "gen_points.c"

using std::vector;
using std::pair;
using std::cout;
using std::endl;

/*
  falta:
  limpar memoria
  substituir no void a 2 chamada do orth por uma copia
  melhorar alguma das alocaçºoes de memoria que fiz
  testar com o codigo do professor
  */
struct tree{
    double* center;
    double rad=0;
    tree *L;
    tree *R;
};


size_t n_dim;


double eucl(double *aux1, double *aux2){
    double ret=0.0;
    double aux=0.0;
    for(size_t i=0; i < n_dim; i++){
        aux=aux1[i]-aux2[i];
        ret= ret+ aux*aux;
    }
    return ret;
}

double inner(double *a, double *b){
    double ret=0.0;
    for(size_t i=0;i < n_dim; i++){
        ret=ret+a[i]*b[i];
    }
    return ret;
}

double * add( double *aux1, double *aux2){
    double *arr = (double *) malloc(n_dim * sizeof(double));
    for(size_t i=0;i < n_dim; i++){
        arr[i]=aux1[i]+aux2[i];
    }
    return arr;
}

double * sub( double *aux1, double *aux2){
    double *arr = (double *) malloc(n_dim * sizeof(double));
    for(size_t i=0;i < n_dim; i++){
        arr[i]=aux1[i]-aux2[i];
    }
    return arr;
}

double * mult( double aux1, double *aux2){
    double *arr = (double *) malloc(n_dim * sizeof(double));
    for(size_t i=0;i < n_dim; i++){
        arr[i]=aux1*aux2[i];
    }
    return arr;
}

double ** orth(double **dataset, pair< double *, double* > a_b, size_t data_size){

    double **ret = (double **) malloc(data_size * sizeof(double *));
    for(size_t i = 0; i < data_size; i++) ret[i] = (double *)malloc(n_dim * sizeof(double));

    double *b_a = (double *) malloc(n_dim * sizeof(double));

    b_a=sub(a_b.second,a_b.first);

    double inner_b_a=inner(b_a,b_a);

    for(size_t i=0;i < data_size ;i++){
        ret[i]= add(mult((inner( sub(dataset[i],a_b.first) ,b_a )/ inner_b_a),b_a), a_b.first);

        // n sei se perferem melhorar como isto está escrito
    }
    return ret;

}

pair< double*, double* > far_away(double **data, size_t size){
    //metedo descrito no enunciado
    double *a = (double *) malloc(n_dim * sizeof(double));
    double *b = (double *) malloc(n_dim * sizeof(double));
    double aux= 0;
    for(size_t i=1;i<size;i++){
        double auxx=eucl(data[0],data[i]);
            if(auxx>aux){
                aux=auxx;
                a=data[i];
            }
    }
    aux=0;
    for(size_t j=0;j<size;j++){

        double auxx=eucl(a,data[j]);
            if(auxx>aux){
                aux=auxx;
                b=data[j];
            }
    }
    return std::make_pair(a,b);
}


/*
pair< double*, double* > far_away(double **data, size_t size){

    pair< double*, double*> ret;
    double aux= 0;

    for(size_t i=0; i < size; i++){
        cout<< i << endl;
        for(size_t j=i+1; j < size; j++){
            double auxx=eucl(data[i],data[j]);
            if(auxx>aux){
                aux=auxx;
                ret=std::make_pair(data[i],data[j]);
            }
        }
    }
    return ret;
}
*/
int comp(const void *a, const void *b){ // Não está a entrar aqui ???
// n sei se existe uma malhor forma de fazer isto

    double *aa = *(double * const *)a;
    double *bb = *(double * const *)b;
    return (aa[0] > bb[0]);
}

double* median_center( double **orth,size_t size){


        // n sei se isto modifica a ordem
    std::qsort(orth, size, sizeof(*orth), comp);

    if (size%2 == 0){
        double *ret = (double *) malloc(n_dim * sizeof(double));
        for (size_t i = 0; i < n_dim; i++){
            ret[i]= (orth[size/2][i] + orth[size/2-1][i])/2;

        }
        return ret;
    } else{
        return orth[size/2];
    }
}

struct L_R_ret{
    double** first;
    double** second;
    size_t first_size;
    size_t second_size;
};

L_R_ret L_R(double ** data,double ** orthg,double* center,size_t data_size){
    //Explicaçao, numero par de pontos-> mediana "artificial" size/2 corresponde a separação equilibrada dos pontos
    // numero impar de pontos -> mediana é um ponto-> tem que se alocar int(size/2) pontos, utilizando ponto da mediana como centro

    double **ret1 = (double **) malloc(data_size * sizeof(double *));

    for(size_t i = 0; i < data_size; i++) ret1[i] = (double *)malloc(n_dim * sizeof(double));

    double **ret2 = (double **) malloc(data_size * sizeof(double *));
    for(size_t i = 0; i < data_size; i++) ret2[i] = (double *)malloc(n_dim * sizeof(double));

    int aux1=0;
    int aux2=0;

    for(size_t i=0;i<data_size;i++){

        if(orthg[i][0]==center[0]){
        continue;
        }

        if(orthg[i][0]<center[0]){

            for(size_t j=0; j<n_dim;j++){
                ret1[aux1][j]=data[i][j];
            }
            aux1++;
        }

        else{
            for(size_t j=0; j<n_dim;j++){
                ret2[aux2][j]=data[i][j];

            }


            aux2++;
        }
    }

    L_R_ret ret;
    ret.first=(ret1);
    ret.second=(ret2);
    ret.first_size=(aux1);
    ret.second_size=(aux2);

    return ret;


}
double rad(double** data, double* center,size_t data_size){
    double ret=0;
    for(size_t i=0; i<data_size;i++){
        double aux=eucl(data[i],center);
        if(aux>ret){
            ret=aux;
        }

    }
    return pow(ret,0.5);
}


void fit(tree *node, double** dataset, size_t size){
    if(size<=1){
        node->center=dataset[0];
        node->rad=0.0;
    }
    else{

    pair< double * , double* > a_b=far_away(dataset, size);



    double **orth_aux = orth(dataset, a_b, size);


    node->center=median_center(orth(dataset, a_b, size),size);


    node->rad=rad(dataset,node->center,size);


    L_R_ret L_R_aux= L_R(dataset,orth_aux,node->center,size);
    free(dataset);
    free(orth_aux);
    node->L= new tree;

    fit(node->L,L_R_aux.first,L_R_aux.first_size);

    node->R= new tree;
    fit(node->R,L_R_aux.second,L_R_aux.second_size);
}

}


// Funcao Visit
void visit(tree *node) {
  cout<< node->rad << endl;
}  // funcao que imprime um produto da arvore

void traverse(tree *node) {
  // funcao que ira imprimir todos os elementos da arvore ordenadamente
  // (travessia in-order)
  if (node->rad == 0.0){
        return;
    }
  traverse(node->L);

  visit(node);

  traverse(node->R);
}


int main(int argc, char *argv[]){
     int n_dim_aux = atoi(argv[1]);
     long np = atol(argv[2]);

    double **data = get_points(argc, argv, &n_dim_aux, &np);
    n_dim=(size_t)n_dim_aux;

    tree* aux= new tree;

    fit(aux,data, np);

    traverse(aux);
    return 0;
}

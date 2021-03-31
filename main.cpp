
#include <array>
#include <math.h>
#include <algorithm>
#include <random>
#include <iostream>
#include <limits>
#include <string>
#include <stdlib.h>

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
    double rad;
    tree *L;
    tree *R;
};

size_t n_dim=2;



double eucl(const double *aux1, const double *aux2){
    double ret=0.0;
    for(size_t i=0; i < n_dim; i++){
        ret= ret+ pow(aux1[i]-aux2[i],2);
    }
    return pow(ret,0.5);
}

double inner(double *a, double *b){
    double ret=0.0;
    for(size_t i=0;i < n_dim; i++){
        ret=ret+a[i]*b[i];
    }
    return ret;
}

double * add(const double *aux1,const double *aux2){
    double *arr = (double *) malloc(n_dim * sizeof(double));
    for(size_t i=0;i < n_dim; i++){
        arr[i]=aux1[i]+aux2[i];
    }
    return arr;
}

double * sub(const double *aux1,const double *aux2){
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

double *orth_1d(double *dataset, pair< double *, double* > a_b){

    double *b_a = (double *) malloc(n_dim * sizeof(double));
    double *ret = (double *) malloc(n_dim * sizeof(double));

    b_a=sub(a_b.second,a_b.first);

    double inner_b_a=inner(b_a,b_a);

    ret= add(mult((inner( sub(dataset,a_b.first) ,b_a )/ inner_b_a),b_a), a_b.first);

    return ret;

}
/*
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
*/
pair< double*, double* > far_away(double **data, size_t size){

    pair< double*, double*> ret;
    double aux= 0;

    for(size_t i=0; i < size; i++){
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
pair< double**, double** > L_R(double ** data,double ** orthg,double* center,pair< double *, double* > a_b,size_t data_size){
    //Explicaçao, numero par de pontos-> mediana "artificial" size/2 corresponde a separação equilibrada dos pontos
    // numero impar de pontos -> mediana é um ponto-> tem que se alocar int(size/2) pontos, utilizando ponto da mediana como centro

    double **ret1 = (double **) malloc(data_size * sizeof(double *));

    for(size_t i = 0; i < data_size/2; i++) ret1[i] = (double *)malloc(n_dim * sizeof(double));

    double **ret2 = (double **) malloc(data_size * sizeof(double *));
    for(size_t i = 0; i < data_size/2; i++) ret2[i] = (double *)malloc(n_dim * sizeof(double));

    int aux1=0;
    int aux2=0;
    cout<< center[0] <<" "<< center [1] << endl;

    for(size_t i=0;i<data_size;i++){

        if(orthg[i][0]==center[0]){
        continue;
        }

        else if(orthg[i][0]<center[0]){

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
    return std::make_pair(ret1,ret2);


}
double rad(double** data, double* center,size_t data_size){
    double ret=0;
    for(size_t i=0; i<data_size;i++){
        double aux=eucl(data[i],center);
        if(aux>ret){
            ret=aux;
        }

    }
    return ret;
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


    pair<double**,double**> L_R_aux= L_R(dataset,orth_aux,node->center,a_b,size);
    node->L= new tree;

    fit(node->L,L_R_aux.first,size/2);

    node->R= new tree;
    fit(node->R,L_R_aux.second,size/2);
}

}



int main(){
    double **ret1 = (double **) malloc(5 * sizeof(double *));

    for(size_t i = 0; i < 5; i++) ret1[i] = (double *)malloc(2 * sizeof(double));

    double data[5][2]={{7.8,8.0},{8.4,3.9},{9.1,2.0},{2.8,5.5},{3.4,7.7} };

    for(size_t i=0;i<5;i++){
        ret1[i][0]=data[i][0];
        ret1[i][1]=data[i][1];
    }


    tree* aux= new tree;

    fit(aux,ret1,5);

    cout<< "center H"<< aux->center[0] <<" "<< aux->center[1] << endl;
    cout<< "radius R" << aux->rad << endl;


    return 0;
}

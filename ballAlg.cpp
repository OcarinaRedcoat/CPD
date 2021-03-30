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

struct tree{
    vector<double> center;
    double rad;
    tree *L;
    tree *R;
};

int n_dim;


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

double ** orth( double **dataset, pair< double *, double* > a_b, int data_size){

    double *arr = (double *) malloc(n_dim * data_size * sizeof(double));
    double **ret = (double **) malloc(data_size * sizeof(double *));

    //vector<vector<double> > ret(data.size());

    double *b_a = (double *) malloc(n_dim * sizeof(double));

    for (size_t i=0; i < n_dim; i++){
        b_a[i] = a_b.second[i] - a_b.first[i];
        printf("b_a index[%ld]: %f", i, b_a[i]);
    }

    double inner_b_a=inner(b_a,b_a);

    for(size_t i=0;i < data_size ;i++){
        cout << endl << dataset[i]-a_b.first << endl;
        exit(0);
//        ret[i] = (inner(dataset[i]-a_b.first ,b_a) / inner_b_a) *b_a +a_b.first;
    }
    return ret;

}

pair< double*, double* > far_away(double **data, int size){

    //didnt understand the method described in the Project Assignment
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

void fit(tree *node, double **dataset, int size){

    pair< double * , double* > a_b=far_away(dataset, size);

    double **orth_aux = orth(dataset, a_b, size); // FIXME: fiquei aqui
}

int main(int argc, char **argv){

    n_dim = atoi(argv[1]);
    long np = atol(argv[2]);
    double **points = get_points(argc, argv, &n_dim, &np);

    for (int i = 0; i < 5; i++){
        //printf("argv: %ld i: %d", np , i);
        printf("(%f , %f)\n", points[i][0], points[i][1]);
    }

    tree* aux= new tree;
    fit(aux, points, np);
    

    return 0;
}
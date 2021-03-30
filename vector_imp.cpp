#include <vector>
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

struct tree{
    vector<double> center;
    double rad;
    tree *L;
    tree *R;
};

vector<double> operator+(const vector<double>& a, const vector<double>& b){
    vector<double> ret(a.size());
    for(size_t i=0;i<a.size();i++){
        ret[i]=a[i]+b[i];
    }
    return ret;
}

vector<double> operator-(const vector<double>& a, const vector<double>& b){
    vector<double> ret(a.size());
    for(size_t i=0;i<a.size();i++){
        ret[i]=a[i]-b[i];
    }
    return ret;
}

vector<double> operator*(const double& a, const std::vector<double>& b){
    vector<double> ret(b.size());
    for(size_t i=0;i<b.size();i++){
        ret[i]=a*b[i];
    }
    return ret;
}

double inner(const vector<double>& a,const vector<double>& b){
    double ret=0.0;
    for(size_t i=0;i<a.size();i++){
        ret=ret+a[i]*b[i];
    }
    return ret;

    // std::inner_product tem a implemtação ja paralela , mas assumi que n nos deixam utilizar eese tipo de funçoes da std
}



double eucl(const vector<double>& aux1,const vector<double>& aux2){
    double ret=0.0;
    for(size_t i=0;i<aux1.size();i++){
        ret= ret+ pow(aux1[i]-aux2[i],2);
    }
    return pow(ret,0.5);
}

double eucl_sum(const vector<vector<double> >& data,const vector<double>& point){
    double aux=0.0;
    for(size_t i=0;i<data.size();i++){
        aux=aux+eucl(data[i],point);
    }
    return aux;

}

pair<vector<double>,vector<double> > far_away(const vector<vector<double> >& data){

    //didnt understand the method described in the Project Assignment
    pair<vector<double>,vector<double> > ret;
    double aux= 0;

    for(size_t i=0;i<data.size();i++){
        for(size_t j=i+1;j<data.size();j++){
            double auxx=eucl(data[i],data[j]);
            if(auxx>aux){
                aux=auxx;

                ret=std::make_pair(data[i],data[j]);
            }
        }
    }
    return ret;
}

vector<vector<double> > orth(const vector<vector<double> >& data,pair<vector<double>,vector<double> > points){
    vector<vector<double> > ret(data.size());
    vector<double> b_a=points.second-points.first;
    double inner_b_a=inner(b_a,b_a);
    for(size_t i=0;i<data.size();i++){
        ret[i]= (inner(data[i]-points.first ,b_a) / inner_b_a) *b_a +points.first;
    }
    return ret;

}


vector<double> orth_1d(const vector<double>& data,pair<vector<double>,vector<double> > points){

    vector<double> ret(data.size());
    vector<double> b_a=points.second-points.first;
    double inner_b_a=inner(b_a,b_a);
    ret=(inner(data-points.first,b_a) / inner_b_a) *b_a +points.first;
    return ret;

}

static bool comp(const vector<double>& vec1, const vector<double>& vec2){
    return vec1[0] < vec2[0];
}

vector<double> median_center( vector<vector<double> >& orth){
    //refazer este metedo

    std::sort(orth.begin(), orth.end(), comp);

    cout << "After sort" << endl;

    if (orth.size()%2 == 0){
        vector<double> ret;
        for (int i = 0; i < orth[orth.size()/2].size(); i++){
            double aux = orth[orth.size()/2][i] + orth[orth.size()/2-1][i];
            ret.push_back(aux);
        }
        return ret;
    } else{
        return orth[orth.size()/2];
    }
}

pair<vector<vector<double> >,vector<vector<double> > > L_R(const vector<vector<double> >& data,const vector<vector<double> >& orthg,const vector<double>&center,
                                                        pair<vector<double>,vector<double> > points){
    //data is orth
    //center is computed using function above
    pair<vector<vector<double> >,vector<vector<double> > > ret;
    vector<double> aux= orth_1d(center,points);
    for(size_t i=0;i<data.size();i++){
        if(orthg[i]==aux){
            continue;
        }
        if(orthg[i][0]<aux[0]){
            ret.first.push_back(data[i]);
        }
        else{
            ret.second.push_back(data[i]);
        }
    }
    return ret;
}


double rad(const vector<vector<double> >& data,const vector<double>& center){
    double ret=0;
    for(size_t i=0; i<data.size();i++){
        double aux=eucl(data[i],center);
        if(aux>ret){
            ret=aux;
        }

    }
    return ret;
}

void fit(tree *node,vector<vector<double> >& data) {

        if(data.size()==1){
            node->center=data[0];
            node->rad=0.0;
        }
        else{

            pair< vector<double> ,vector<double> > a_b=far_away(data);


            vector<vector<double> > orth_aux=orth(data,a_b);

            node->center=median_center(orth_aux);
            cout<< node->center[0]<<" "<< node->center[1]<< endl;
            node->rad=rad(data,node->center);

            pair<vector<vector<double> >,vector<vector<double> > > L_R_aux=L_R(data,orth_aux,node->center,a_b);

            //tirar aqui o centro, caso for necessário!
            if(L_R_aux.first.size()!=0){
                node->L= new tree;
                fit(node->L,L_R_aux.first);
            }
            if(L_R_aux.second.size()!=0){
                node->R= new tree;
                fit(node->R,L_R_aux.second);
            }
        }

}




int main(int argc, char *argv[] ){

    vector<vector<double> > data= {{7.8,8.0},{8.4,3.9},{9.1,2.0},{2.8,5.5},{3.4,7.7} };
    // este n funciona porque o metedo para encontrar o ponto medio esta mal
    tree* aux= new tree;
    fit(aux,data);
    cout<< "center H"<< aux->center[0] <<" "<< aux->center[1] << endl;
    cout<< "radius R" << aux->rad << endl;
    //cout<< "center L"<< aux->L->center[0] << " "<< aux->L->center[1] << endl;
    //cout<< "center R"<< aux->R->center[0] << " "<< aux->R->center[1] << endl;

    return 0;
}

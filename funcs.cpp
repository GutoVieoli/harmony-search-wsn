#include <iostream>
#include <string>
#include <random>
#include <chrono>

using namespace std;

float ObjCov (string *p, float cratio, int maxS)
{
    float s = 0;   // Calcular o numero de sensores utilizados
    for( int i = 0; i < maxS; i++ )
    {
        if ( *p != "#")
            s++;
        p += 2;
    }
    
    return (1/s) * cratio * 10;
}

void PrintarPDP(float * ponteiro, int width, int height, int cellSize)  //PRINTAR PROBABILIDADE DO PONTO DE DEMANDA 
{
    cout << "\n\n-----------------------------------\n";
    cout << "Categorization of the demand point.\n" << "-----------------------------------\n ";

    for( int i = 0; i < (height/cellSize); i++){             // Print Categorization of the demand point.
        for( int j = 0; j < (width/cellSize); j++){
            cout << *ponteiro << " ";
            ponteiro++;
        }
        cout << "\n ";        
    }

    cout << "----------------------------------\n\n";
}

float Cratio(float * ponteiro, int width, int height, int cellSize, float cth) 
{
    int neff = 0;
    int nall = 0;
    for( int i = 0; i < (height/cellSize); i++){             // Print Categorization of the demand point.
        for( int j = 0; j < (width/cellSize); j++){
            if( *ponteiro >= cth )
            {
                neff++;
            }
            ponteiro++;
            nall++;
        }       
    }
    return (float) neff/nall;
}

void ZerarMatAux(float * ponteiro, int h, int w, int cellSize)
{
    for ( int i = 0; i < (h/cellSize)*(w/cellSize); i++)
    {
        *ponteiro = 0;
        ponteiro++;
    }
}

void PrintarHMS (string * p, int hms, int maxS)
{
    cout << "\n\n";
    for(int i = 0; i < hms; i++){
        cout << i+1 << " -> ";
        for(int j = 0; j < maxS*2; j++){
            if( *p == "#"){
                cout << *p << " ";
                p++;
            }
            else 
            {
                float aux = stof(*p);
                cout  << aux <<  " ";
                p++;
            }
        }
        cout << "\n";
    }
}

float DistEuclidiana(float xSens, float ySens, float xPonto, float yPonto)
{
    float d = sqrt(pow(xSens - xPonto, 2) + pow(ySens - yPonto, 2));
    return d;
}
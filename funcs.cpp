#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <iomanip>

using namespace std;

float ObjCov2 (bool *ativado, float cratio, float minDist, int maxS)
{
    float s = 0;   // Calcular o numero de sensores utilizados
    for( int i = 0; i < maxS; i++ )
    {
        if ( *ativado == 1){
            s++;
        }
        ativado++;
    }
    
    return (1/s) * cratio * minDist * 100;
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
    for( int i = 0; i < (height/cellSize); i++){            
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

void PrintarHMS2 (string * x, string * y, bool * ativado, int hms, int maxS)
{
    cout << "\n\n";
    for(int i = 0; i < hms; i++)
    {
        cout << "\n ___ Vetor " << i+1 << " __________________________________________________________________________________________________________________________________________________________________\n";

        for(int auxX = 0; auxX < maxS; auxX++)
        {
            float auxi = stof(*x);
            std::cout << std::setw(5) << auxi << " |";
            x++;
        }
        cout << "\n";
        for(int auxY = 0; auxY < maxS; auxY++)
        {
            float auxi = stof(*y);
            std::cout << std::setw(5) << auxi << " |";
            y++;            
        }
        cout << "\n";
        for(int auxAtivado = 0; auxAtivado < maxS; auxAtivado++)
        {
            std::cout << std::setw(5) << *ativado << " |";
            ativado++;            
        }
        cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    }
}

float DistEuclidiana(float xSens, float ySens, float xPonto, float yPonto)
{
    float d = sqrt(pow(xSens - xPonto, 2) + pow(ySens - yPonto, 2));
    return d;
}


float MinDist2(string *x, string *y, bool *ativado, int maxS, int w, int h, int rs, int re)
{
    float menorDist = 999999999;
    string cpyStr[maxS][3];
    int wm = w - (rs - re);
    int hm = h - (rs - re);
    float denominador = sqrt(pow(wm, 2) + pow(hm, 2));

    for(int i = 0;  i < maxS;  i++)
    {
        cpyStr[i][0] = *x;
        x++;
        cpyStr[i][1] = *y;
        y++;
        cpyStr[i][2] = ( *ativado == true ? "true" : "false");
        ativado++;
    }

    for(int i = 0;  i < maxS;  i++)
    {
        for(int j = 0; j < maxS; j++)
        {
            if( i != j && (cpyStr[i][2] != "false") && (cpyStr[j][2] != "false") )
            {
                float distEuclidian = DistEuclidiana( std::stof(cpyStr[i][0]), std::stof(cpyStr[i][1]), std::stof(cpyStr[j][0]), std::stof(cpyStr[j][1]) );
                if( distEuclidian < menorDist)
                {
                    menorDist = distEuclidian;
                }
            }
        }
    }
    return menorDist / denominador;
}


float CalcPsov(float probabilidadeParcial, float probabilidadeRecemCalculada)
{
    if(probabilidadeParcial > 0)
    {
        probabilidadeRecemCalculada = (1 - probabilidadeRecemCalculada);
        return (1 - probabilidadeParcial * probabilidadeRecemCalculada);
    }
    else 
    {
        return probabilidadeRecemCalculada;
    }
}
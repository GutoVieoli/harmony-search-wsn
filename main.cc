#include <iostream>
#include <random>
#include <chrono>
#include <cmath>
#include "header.h"

using namespace std;

std::mt19937 mt{ static_cast<unsigned int>(
    std::chrono::steady_clock::now().time_since_epoch().count()
    ) };

std::uniform_real_distribution<> zeroUm( 0.0, 1.0 );

double const HMCR = 0.9;              // Taxa de consideraçao de memória harmônica
double const PAR = 0.3;               // Taxa de ajuste de passo
int const HMS = 30;                // Numero de soluçoes do vetor
double const BW = 0.2;                // Largura de banda
int const NI = 60000;               // Critério de parada (número de improvisações)

int const W = 50;                  // Width
int const H = 50;                  // Height
int const Rs = 10;                 // A faixa de detecção do sensor
int const Re = 5;                  // Incerteza da detecção do sensor
int const CellSize = 10;           // Tamanho da célula
float const Cth = 0.9;               // Limite de cobertura
float const A1 = 1;
float const A2 = 0;
float const B1 = 1;
float const B2 = 0.5;
float G1;
float G2;

const short Lxs = Rs - Re;                // Limite inferior de pos x do sensor
const short Uxs = W - (Rs - Re);          // Limite superior de pos x do sensor
const short Lys = Rs - Re;                // Limite inferior de pos y do sensor
const short Uys = H - (Rs - Re);          // Limite superior de pos y do sensor
const short MinS = 3;               // Mínimo de sensores da rede   2,78...
const short MaxS = 25;              // Máximo de sensores da rede

float matCoverage[ W/CellSize ][ H/CellSize ] = {0};   //faz saber se os sensores as celulas estao supervisionadas


// Comprimento de cada vetor da memória harmonica
int HMVlen ()
{
    return ( MinS + ((MaxS+1 - MinS) * zeroUm(mt)) );
}

// Inicialização da Memória Harmônica
void HM_initi (string * p)
{

    for( int i = 0; i < HMS; i++ ){
        int sensores = 0;

        for( int j = 0; j < MaxS; j++){
            if(zeroUm(mt) >= 0.35f)
            {
                sensores++;
                *p = to_string( floorf((Lxs + (Uxs - Lxs) * zeroUm(mt)) * 10) / 10 );  //eq 15
                p++;
                *p = to_string( floorf((Lys + (Uys - Lys) * zeroUm(mt)) * 10) / 10);  //eq 15
            }
            else
            {
                *p = "#";
                p++;
                *p = "#";                
            }
            p++;
        }

        if(sensores < MinS){
            i--;
            p -= MaxS * 2;
        }

    }
}


void CalcGama(float distEuc)
{
    G1 = Re - Rs + distEuc;
    G2 = Re + Rs - distEuc;
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

float CalcPcov(float dist, float probabilidadeParcial)
{
    if( dist <= Rs - Re )
        return 1;
    else if ( dist >= Rs + Re && probabilidadeParcial != 1)
        return 0;
    else if ( probabilidadeParcial != 1 ) {
        float e = 2.718281828459;
        float quocienteElev = A1 * pow(G1, B1);
        float dividendoElev = pow(G2, B2);
        float elev = -( quocienteElev / dividendoElev + A2);
        //return pow(e, elev);
        return CalcPsov(probabilidadeParcial, pow(e, elev));
    }
}

void MatrizPcov(string * pontHM)
{
    for(int j = 1; j <= MaxS; j++)
    {
        if(*pontHM != "#")  //x
        {
            string * pontHM2 = pontHM + 1;    // y
            for( int y = 1; y <= W / CellSize; y++)    // Categorization of the coverage probability for each demand point.
            {    
                for( int x = 1; x <= H / CellSize; x++)
                {
                    float dist = DistEuclidiana( stof(*pontHM), stof(*pontHM2), x*10 - 5, y*10 - 5);
                    CalcGama(dist);
                    matCoverage[ x-1 ][ y-1 ] = CalcPcov(dist, matCoverage[ x-1 ][ y-1 ]);
                }
            }
        }
        pontHM+=2;
    }
}

int main(void)
{
    srand(time(NULL));
    string hm[HMS][MaxS*2];
    string * pontHM = &hm[0][0];
    float objetivos[HMS];
    //float * pontObjetivos = &objetivos[0];
    string newSoluction[MaxS*2];

    int posPior;
    float piorObj;


    HM_initi(pontHM);                       //Gera vetor harmonico inicial
    PrintarHMS(pontHM, HMS, MaxS);          // Printar vetor harmonico

    float * pontMatCoverage = &matCoverage[0][0];
    ZerarMatAux(pontMatCoverage, W, H, CellSize);
    

    // Calcula o objetivo dos valores iniciais, salva em uma matriz e obtem o menor deles
    for(int i = 0; i < HMS; i++)
    {
        pontHM = & hm[i][0];
        MatrizPcov(pontHM);
        objetivos[i] = ObjCov(pontHM, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist(pontHM, MaxS, W, H, Rs, Re), MaxS);
        ZerarMatAux(pontMatCoverage, W, H, CellSize);
        std::cout << "\n" << i+1 << ": " << objetivos[i];   //--- Retirar futuro
        if( i == 0 || objetivos[i] < piorObj)
        {
            posPior = i;
            piorObj = objetivos[i];
        }
    }
    //std::cout << "\n O pior objetivo foi " << piorObj << " na posicao " << posPior << "\n";

    for(int iter = 0; iter < NI; iter++)
    {
        int hashtags = 0;
        // improvise a new solution vector
        for(int i1 = 0; i1 < MaxS; i1++)
        {
            if( zeroUm(mt) <= HMCR )
            {
                int randLoc = (int) (HMS * zeroUm(mt));
                newSoluction[i1*2] = hm[ randLoc ][ i1* 2 ];                     // pos x
                newSoluction[i1*2 + 1] = hm[ randLoc ][ i1* 2 + 1];              // pos y

                if(newSoluction[i1*2] == "#")    // Conta o numero de sensores utilizados para verificar se é aceito
                    hashtags++;

                if( newSoluction[i1*2] != "#")
                {
                    // Check to be pitch adjusted
                    if( zeroUm(mt) <= PAR)
                    {
                        
                        float randomX = rand() % 2;
                        float randomY = rand() % 2;
                        float randBWx = ceil(zeroUm(mt) * BW * 10) / 10; 
                        float randBWy = ceil(zeroUm(mt) * BW * 10) / 10; 
                        
                        float numX = stof( newSoluction[i1*2] );
                        numX += (randomX == 0 ? randBWx : -randBWx);
                        newSoluction[i1 * 2] = to_string(numX);

                        if( stof(newSoluction[i1*2]) < Lxs)  // Se for menor que o limite inferior
                        {
                            float num = stof( newSoluction[i1*2] ) + randBWx;
                            newSoluction[i1 * 2] = to_string(num);
                        }
                        else if( stof(newSoluction[i1*2]) > Uxs)   // Se for maior que o limite uperior
                        {
                            float num = stof( newSoluction[i1*2] ) - randBWx;
                            newSoluction[i1 * 2] = to_string(num);
                        }


                        float numY = stof( newSoluction[i1*2 + 1] );
                        numY += (randomY == 0 ? randBWy : -randBWy); 
                        newSoluction[i1*2 + 1] = to_string(numY);

                        if( stof(newSoluction[i1*2 + 1]) < Lys)  // Se for menor que o limite inferior
                        {
                            float num = stof( newSoluction[i1*2] ) + randBWy;
                            newSoluction[i1*2 + 1] = to_string(num);
                        }
                        else if( stof(newSoluction[i1*2 + 1]) > Uxs)   // Se for maior que o limite uperior
                        {
                            float num = stof( newSoluction[i1*2] ) - randBWy;
                            newSoluction[i1*2 + 1] = to_string(num);
                        }        
                    }
                }
            }
            else
            {
                // Chose a random variable
                newSoluction[i1*2] = to_string( floorf((Lxs + (Uxs - Lxs) * zeroUm(mt)) * 10) / 10 );     // pos x
                newSoluction[i1*2 + 1] = to_string( floorf((Lys + (Uys - Lys) * zeroUm(mt)) * 10) / 10);   // pos y
            }
        }

        pontHM = & newSoluction[0];
        MatrizPcov(pontHM);
        if( ObjCov(pontHM, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist(pontHM, MaxS, W, H, Rs, Re), MaxS) >= piorObj && hashtags <= MaxS - MinS )
        {
            objetivos[posPior] =  ObjCov(pontHM, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist(pontHM, MaxS, W, H, Rs, Re), MaxS);
            for( int i = 0; i < MaxS; i++)    // Substitui a pior soluçao pela gerada
            {
                hm[posPior][i*2] = newSoluction[i*2];
                hm[posPior][i*2 + 1] = newSoluction[i*2 + 1];
            }

            for(int i = 0; i < HMS; i++)  // Encontra a nova pior soluçao
            {
                if( i == 0 || objetivos[i] < piorObj)
                {
                    posPior = i;
                    piorObj = objetivos[i];
                }
            }
        }
        ZerarMatAux(pontMatCoverage, W, H, CellSize);
        
    }

    pontHM = &newSoluction[0];
    MatrizPcov(pontHM);
    PrintarPDP(pontMatCoverage, W, H, CellSize);
    std::cout << "\nFuncao Objetivo da novo vetor: " << ObjCov(pontHM, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist(pontHM, MaxS, W, H, Rs, Re), MaxS) << "\n";

    pontHM = &hm[0][0];
    PrintarHMS(pontHM, HMS, MaxS);  

        for(int i = 0; i < HMS; i++)
        {
            ZerarMatAux(pontMatCoverage, W, H, CellSize);
            pontHM = & hm[i][0];
            MatrizPcov(pontHM);
            objetivos[i] = ObjCov(pontHM, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist(pontHM, MaxS, W, H, Rs, Re), MaxS);
            std::cout << "\n" << i+1 << ": " << objetivos[i];
        }

    pontHM = & hm[0][0];
    MatrizPcov(pontHM);
    PrintarPDP(pontMatCoverage, W, H, CellSize);
    ZerarMatAux(pontMatCoverage, W, H, CellSize);

    return 0;
}
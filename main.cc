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
void HM_initi2 (string * x, string * y, bool * ativado)
{

    for( int i = 0; i < HMS; i++ ){
        int sensores = 0;

        for( int j = 0; j < MaxS; j++)
        {
            *x = to_string( floorf((Lxs + (Uxs - Lxs) * zeroUm(mt)) * 10) / 10 );  //eq 15
            x++;
            *y = to_string( floorf((Lys + (Uys - Lys) * zeroUm(mt)) * 10) / 10);  //eq 15
            y++;

            if(zeroUm(mt) >= 0.4f)
            {
                sensores++;
                *ativado = true;
                ativado++;
            }
            else
            {
                *ativado = false;
                ativado++;              
            }
        }

        if(sensores < MinS){
            i--;
            x -= MaxS;
            y -= MaxS;
            ativado -= MaxS;
        }

    }
}

void CalcGama(float distEuc)
{
    G1 = Re - Rs + distEuc;
    G2 = Re + Rs - distEuc;
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


void MatrizPcov2(string * x, string * y, bool * ativado)
{
    for(int j = 1; j <= MaxS; j++)
    {
        if(*ativado != false)
        {
            for( int k = 1; k <= W / CellSize; k++)    // Categorization of the coverage probability for each demand point.
            {    
                for( int l = 1; l <= H / CellSize; l++)
                {
                    float dist = DistEuclidiana( stof(*x), stof(*y), l*10 - 5, k*10 - 5);
                    CalcGama(dist);
                    matCoverage[ l-1 ][ k-1 ] = CalcPcov(dist, matCoverage[ l-1 ][ k-1 ]);
                }
            }
        }
        x++;
        y++;
        ativado++;
    }
}

void Correcao(string newSoluctX, string newSoluctY)
{
    float randomX = rand() % 2;
    float randomY = rand() % 2;
    float randBWx = ceil(zeroUm(mt) * BW * 10) / 10; 
    float randBWy = ceil(zeroUm(mt) * BW * 10) / 10; 
    
    float numX = stof( newSoluctX );
    numX += (randomX == 0 ? randBWx : -randBWx);
    newSoluctX = to_string(numX);

    if( stof(newSoluctX) < Lxs)  // Se for menor que o limite inferior
    {
        float num = stof( newSoluctX ) + randBWx;
        newSoluctX = to_string(num);
    }
    else if( stof( newSoluctX ) > Uxs)   // Se for maior que o limite uperior
    {
        float num = stof( newSoluctX ) - randBWx;
        newSoluctX = to_string(num);
    }


    float numY = stof( newSoluctY );
    numY += (randomY == 0 ? randBWy : -randBWy); 
    newSoluctY = to_string(numY);

    if( stof( newSoluctY ) < Lys)  // Se for menor que o limite inferior
    {
        float num = stof( newSoluctY ) + randBWy;
        newSoluctY = to_string(num);
    }
    else if( stof( newSoluctY ) > Uxs)   // Se for maior que o limite uperior
    {
        float num = stof( newSoluctY ) - randBWy;
        newSoluctY = to_string(num);
    }          
}

struct HarmonySearch {
  std::string x[HMS][MaxS];
  std::string y[HMS][MaxS];
  bool ativado[HMS][MaxS];
};

struct HarmonyNew {
  std::string x[MaxS];
  std::string y[MaxS];
  bool ativado[MaxS];
};

int main(void)
{
    srand(time(NULL));
    HarmonySearch harm;
    string * pontX = &harm.x[0][0];
    string * pontY = &harm.y[0][0];
    bool * pontAtivado = &harm.ativado[0][0];
   
    float objetivos[HMS];
    HarmonyNew newSoluction;
    //string newSoluction[MaxS][3];  

    int posPior;
    float piorObj;

    HM_initi2(pontX, pontY, pontAtivado);
    PrintarHMS2(pontX, pontY, pontAtivado, HMS, MaxS); 

    float * pontMatCoverage = &matCoverage[0][0];
    ZerarMatAux(pontMatCoverage, W, H, CellSize);

    // Calcula o objetivo dos valores iniciais, salva em uma matriz e obtem o menor deles
    for(int i = 0; i < HMS; i++)
    {
        pontX = &harm.x[i][0];
        pontY = &harm.y[i][0];
        pontAtivado = &harm.ativado[i][0];
        MatrizPcov2(pontX, pontY, pontAtivado);   
        objetivos[i] = ObjCov2(pontAtivado, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist2(pontX, pontY, pontAtivado, MaxS, W, H, Rs, Re), MaxS); 
        ZerarMatAux(pontMatCoverage, W, H, CellSize);
        std::cout << "\n" << i+1 << ": " << objetivos[i];   //--- Retirar futuro
        if( i == 0 || objetivos[i] < piorObj)
        {
            posPior = i;
            piorObj = objetivos[i];
        }    
    }
    std::cout << "\n O pior objetivo foi " << piorObj << " na posicao " << posPior << "\n";


    for(int iter = 0; iter < 400000; iter++)
    {
        int hashtags = 0;
        // improvise a new solution vector
        for(int i1 = 0; i1 < MaxS; i1++)
        {
            if( zeroUm(mt) <= HMCR )
            {
                int randLoc = (int) (HMS * zeroUm(mt));
                newSoluction.x[i1] = harm.x[ randLoc ][ i1 ];               // pos x
                newSoluction.y[i1] = harm.y[ randLoc ][ i1];                // pos y
                newSoluction.ativado[i1] = harm.ativado[ randLoc ][ i1] ;   // ativado  
    
                if(zeroUm(mt) <= 0.04f)
                    newSoluction.ativado[i1] = ( newSoluction.ativado[i1] == false ? true : false);

                if(newSoluction.ativado[i1] == false)    // Conta o numero de sensores utilizados para verificar se é aceito
                    hashtags++;

                else if( newSoluction.ativado[i1] == true)
                {
                    // Check to be pitch adjusted
                    if( zeroUm(mt) <= PAR)
                    { 
                        Correcao(newSoluction.x[i1], newSoluction.y[i1]);
                    }
                }
            }
            else
            {
                // Chose a random variable
                newSoluction.x[i1] = to_string( floorf((Lxs + (Uxs - Lxs) * zeroUm(mt)) * 10) / 10 );     // pos x
                newSoluction.y[i1] = to_string( floorf((Lys + (Uys - Lys) * zeroUm(mt)) * 10) / 10);   // pos y
                newSoluction.ativado[i1] = true;
            }
        }

        pontX = &newSoluction.x[0];
        pontY = &newSoluction.y[0];
        pontAtivado = &newSoluction.ativado[0];
        MatrizPcov2(pontX, pontY, pontAtivado);

        if( ObjCov2(pontAtivado, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist2(pontX, pontY, pontAtivado, MaxS, W, H, Rs, Re), MaxS) > piorObj && hashtags <= MaxS - MinS )
        {
            objetivos[posPior] =  ObjCov2(pontAtivado, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist2(pontX, pontY, pontAtivado, MaxS, W, H, Rs, Re), MaxS);
            for( int i = 0; i < MaxS; i++)    // Substitui a pior soluçao pela gerada
            {
                harm.x[posPior][i] = newSoluction.x[i];
                harm.y[posPior][i] = newSoluction.y[i];
                harm.ativado[posPior][i] = newSoluction.ativado[i];
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
    


    pontX = &harm.x[0][0];
    pontY = &harm.y[0][0];
    pontAtivado = &harm.ativado[0][0];
    PrintarHMS2(pontX, pontY, pontAtivado, HMS, MaxS);  

        for(int i = 0; i < HMS; i++)
        {
            ZerarMatAux(pontMatCoverage, W, H, CellSize);
            pontX = &harm.x[i][0];
            pontY = &harm.y[i][0];
            pontAtivado = &harm.ativado[i][0];
            MatrizPcov2(pontX, pontY, pontAtivado);
            objetivos[i] = ObjCov2(pontAtivado, Cratio(pontMatCoverage, W, H, CellSize, Cth), MinDist2(pontX, pontY, pontAtivado, MaxS, W, H, Rs, Re), MaxS); 
            std::cout << "\n" << i+1 << ": " << objetivos[i];
        }

    pontX = &newSoluction.x[0];
    pontY = &newSoluction.y[0];
    pontAtivado = &newSoluction.ativado[0];
    MatrizPcov2(pontX, pontY, pontAtivado);
    PrintarPDP(pontMatCoverage, W, H, CellSize);
    ZerarMatAux(pontMatCoverage, W, H, CellSize);

    return 0;
}
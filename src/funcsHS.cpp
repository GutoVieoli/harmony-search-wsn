#include <cmath>
#include <chrono>
#include <random>
#include <iomanip>
#include <iostream>
#include "cabecalho.h"

using namespace std;

std::mt19937 mt{ static_cast<unsigned int>(
    std::chrono::steady_clock::now().time_since_epoch().count()
) };

std::uniform_real_distribution<> zeroUm( 0.0, 1.0 );

const double HMCR = 0.9;            // Taxa de consideraçao de memória harmônica
const double PAR = 0.3;             // Taxa de ajuste de passo
const double BW = 0.2;              // Largura de banda
const int NI = 60000;               // Critério de parada (número de improvisações)
const double SFP = 0.05;            // Sensor Flip Probability
const float CTH = 0.9;              // Limite de cobertura
const float A1 = 1;
const float A2 = 0;
const float B1 = 1;
const float B2 = 0.5;
float G1;
float G2;

const float Lxs = RS - RE;                // Limite inferior de pos x do sensor
const float Uxs = W - (RS - RE);          // Limite superior de pos x do sensor
const float Lys = RS - RE;                // Limite inferior de pos y do sensor
const float Uys = H - (RS - RE);          // Limite superior de pos y do sensor
const short MinS = std::ceil( (static_cast<float>(W) / (2 * (RS + RE))) * ( static_cast<float>(H) / (2 * (RS + RE))) ) ;     // Mínimo de sensores da rede   2,78...
float matCoverage[ H/CELL_SIZE ][ W/CELL_SIZE ] = {0}; 


// Comprimento de cada vetor da memória harmonica
int hmv_len ()
{
    return ( MinS + ((MaxS+1 - MinS) * zeroUm(mt)) );
}


// Inicialização da Memória Harmônica
void iniciar_hm (string * x, string * y, bool * ativado)
{

    for( int i = 0; i < HMS; i++ )
    {
        for( int j = 0; j < MaxS; j++)
        {
            *x = to_string( floorf((Lxs + (Uxs - Lxs) * zeroUm(mt)) * 10) / 10 );  //eq 15
            x++;
            *y = to_string( floorf((Lys + (Uys - Lys) * zeroUm(mt)) * 10) / 10);  //eq 15
            y++;
            *ativado = false;
            ativado++;     
        }

        for( int sensores = hmv_len(); sensores > 0; sensores-- )
        {
            int pos = ( MaxS * zeroUm(mt) ) + 1;
            ativado -= pos;
            if (*ativado == false) 
                *ativado = true;
            else if (*ativado == true)
                sensores++;
            ativado += pos;
        }
    }
    
}


void printar_hm (string * x, string * y, bool * ativado)
{
    cout << "\n\n";
    for(int i = 0; i < HMS; i++)
    {
        cout << "\n ___ Vetor " << i+1 << " __________________________________________________________________________________________________________________________________________________________________\n";

        for(int auxX = 0; auxX < MaxS; auxX++)
        {
            float auxi = stof(*x);
            std::cout << std::setw(5) << auxi << " |";
            x++;
        }
        cout << "\n";
        for(int auxY = 0; auxY < MaxS; auxY++)
        {
            float auxi = stof(*y);
            std::cout << std::setw(5) << auxi << " |";
            y++;            
        }
        cout << "\n";
        for(int auxAtivado = 0; auxAtivado < MaxS; auxAtivado++)
        {
            std::cout << std::setw(5) << *ativado << " |";
            ativado++;            
        }
        cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    }
}


void printar_best (string * x, string * y, bool * ativado)
{
    cout << "\nO Melhor Vetor Solucao\n";

    cout << "Pos. X |";
    for(int auxX = 0; auxX < MaxS; auxX++, ativado++)
    {
        if(*ativado){
            float auxi = stof(*x);
            std::cout << std::setw(5) << auxi << " |";
            x++;
        }
    }
    ativado -= MaxS;

    cout << "\nPos. Y |";
    for(int auxY = 0; auxY < MaxS; auxY++, ativado++)
    {
        if(*ativado){
            float auxi = stof(*y);
            std::cout << std::setw(5) << auxi << " |";
            y++;  
        }          
    }

    cout << "\n";
    
}


void calc_gama(float distEuc)
{
    G1 = (RE - RS) + distEuc;
    G2 = (RE + RS) - distEuc;
}


float dist_euclidiana(float xSens, float ySens, float xPonto, float yPonto)
{
    float d = sqrt(pow(xSens - xPonto, 2) + pow(ySens - yPonto, 2));
    return d;
}


float min_dist(string *x, string *y, bool *ativado)
{
    float menorDist = 999999999;
    string cpyStr[MaxS][3];
    float wm = W - (RS - RE);
    float hm = H - (RS - RE);
    float denominador = sqrt(pow(wm, 2) + pow(hm, 2));

    for(int i = 0;  i < MaxS;  i++)
    {
        cpyStr[i][0] = *x;
        x++;
        cpyStr[i][1] = *y;
        y++;
        cpyStr[i][2] = ( *ativado == true ? "true" : "false");
        ativado++;
    }

    for(int i = 0;  i < MaxS;  i++)
    {
        for(int j = 0; j < MaxS; j++)
        {
            if( i != j && (cpyStr[i][2] != "false") && (cpyStr[j][2] != "false") )
            {
                float distEuclidian = dist_euclidiana( std::stof(cpyStr[i][0]), std::stof(cpyStr[i][1]), std::stof(cpyStr[j][0]), std::stof(cpyStr[j][1]) );
                if( distEuclidian < menorDist)
                {
                    menorDist = distEuclidian;
                }
            }
        }
    }
    return menorDist / denominador;
}


void zerar_mat_pcov(float * ponteiro)
{
    for ( int i = 0; i < (H/CELL_SIZE)*(W/CELL_SIZE); i++)
    {
        *ponteiro = 0;
        ponteiro++;
    }
}


float calc_psov(float probabilidadeParcial, float probabilidadeRecemCalculada)
{
    if(probabilidadeParcial > 0)
    {
        probabilidadeRecemCalculada = (1 - probabilidadeRecemCalculada);
        probabilidadeParcial = (1 - probabilidadeParcial);
        return (1 - ( probabilidadeParcial * probabilidadeRecemCalculada ) );
    }
    else 
    {
        return probabilidadeRecemCalculada;
    }
}


float calc_pcov(float dist, float probabilidadeParcial)   //probabilidade parcial é o valor que ja esta salvo na matriz aux naquela posiçao
{
    if( dist <= RS - RE )
        return 1;
    else if ( dist > RS + RE )
        return probabilidadeParcial;
    else {
        float e = 2.718281828459;
        float quocienteElev = A1 * pow(G1, B1);
        float dividendoElev = pow(G2, B2);
        float elev = -( quocienteElev / dividendoElev + A2);
        //return pow(e, elev);
        return calc_psov(probabilidadeParcial, pow(e, elev));
    }
}


void matriz_pcov(string * x, string * y, bool * ativado)
{
    for(int j = 1; j <= MaxS; j++)
    {
        if(*ativado != false)
        {
            for( int k = 1; k <= H / CELL_SIZE; k++)    // Categorization of the coverage probability for each demand point.
            {    
                for( int l = 1; l <= W / CELL_SIZE; l++)
                {
                    float dist = dist_euclidiana( stof(*x), stof(*y),  (l*CELL_SIZE)-( CELL_SIZE/2.0f), (k*CELL_SIZE)-(CELL_SIZE/2.0f) );
                    calc_gama(dist);
                    matCoverage[ k-1 ][ l-1 ] = calc_pcov(dist, matCoverage[ k-1 ][ l-1 ]);
                }
            }
        }
        x++;
        y++;
        ativado++;
    }
}


void printar_pdp(float * ponteiro)  //PRINTAR PROBABILIDADE DO PONTO DE DEMANDA 
{
    cout << "\n\n----------------------------------------------------\n";
    cout << "Categorization of the demand point.\n" << "----------------------------------------------------\n\n ";
    cout <<  std::setprecision(3) << std::fixed;

    for( int i = 0; i < (H/CELL_SIZE); i++){             // Print Categorization of the demand point.
        for( int j = 0; j < (W/CELL_SIZE); j++){
            cout << *ponteiro << setfill(' ') << setw(5) << " ";
            ponteiro++;
        }
        cout << "\n\n\n ";        
    }

    std::cout.unsetf(std::ios_base::floatfield);
    std::cout.precision(6);
    cout << "---------------------------------------------------\n\n";
}


float obj_cov (bool *ativado, float cratio, float minDist)
{
    float s = 0;   // Calcular o numero de sensores utilizados
    for( int i = 0; i < MaxS; i++ )
    {
        if ( *ativado == 1){
            s++;
        }
        ativado++;
    }
    
    //return (1/s) * cratio * 100;
    return ( (1/s) * cratio * ((minDist/2) + 1)  + (cratio / 30) ) * 10;
}


float cratio(float * ponteiro) 
{
    float neff = 0;
    float nall = 0;
    for( int i = 0; i < (H/CELL_SIZE); i++){            
        for( int j = 0; j < (W/CELL_SIZE); j++){
            if( *ponteiro >= CTH )
            {
                neff++;
            }
            ponteiro++;
            nall++;
        }       
    }
    return (float) neff/nall;
}


void ajuste_de_pitch(string newSoluctX, string newSoluctY)
{
    float randomX = rand() % 2;   // 0 é mais, 1 é menos
    float randomY = rand() % 2;   // 0 é mais, 1 é menos
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


int num_sensores (bool *ativado)
{
    float s = 0;   // Calcular o numero de sensores utilizados
    for( int i = 0; i < MaxS; i++ )
    {
        if ( *ativado == 1){
            s++;
        }
        ativado++;
    }

    return s;
}
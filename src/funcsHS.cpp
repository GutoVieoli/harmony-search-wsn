#include <cmath>
#include <chrono>
#include <random>
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

const float Lxs = RS - RE;                // Limite inferior de pos x do sensor
const float Uxs = W - (RS - RE);          // Limite superior de pos x do sensor
const float Lys = RS - RE;                // Limite inferior de pos y do sensor
const float Uys = H - (RS - RE);          // Limite superior de pos y do sensor
const short MinS = std::ceil( (static_cast<float>(W) / (2 * (RS + RE))) * ( static_cast<float>(H) / (2 * (RS + RE))) ) ;     // Mínimo de sensores da rede   2,78...


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
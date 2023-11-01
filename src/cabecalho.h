#ifndef CABECALHO_H
#define CABECALHO_H

#include <iostream>
#include <random>
#include <cmath>

using namespace std;

constexpr int W = 50;
constexpr int H = 50;
constexpr int CELL_SIZE = 10;
constexpr int HMS = 30;
extern const double HMCR;            
extern const double PAR;                          
extern const double BW;           
extern const int NI;      
extern const double SFP;                          
constexpr int RS = 10;             
constexpr float RE = 5;                         
extern const float CTH;      
extern const float A1;
extern const float A2;
extern const float B1;
extern const float B2;

extern const float Lxs;           
extern const float Uxs;    
extern const float Lys;              
extern const float Uys;

constexpr int calculateMaxS() {
    return std::ceil((static_cast<float>(W) / (2 * (RS - RE))) * (static_cast<float>(H) / (2 * (RS - RE))));
}

extern const short MinS;     
constexpr int MaxS = calculateMaxS(); 

extern std::mt19937 mt;
extern std::uniform_real_distribution<> zeroUm;

struct HarmonySearch {        // Toda a matriz de soluçao
  std::string x[HMS][MaxS];
  std::string y[HMS][MaxS];
  bool ativado[HMS][MaxS];
};

struct HarmonyNew {          // Um vetor de soluçao
  std::string x[MaxS];
  std::string y[MaxS];
  bool ativado[MaxS];
};

int hmv_len ();
void iniciar_hm (string * x, string * y, bool * ativado);

#endif
#ifndef CABECALHO_H
#define CABECALHO_H

#include <iostream>
#include <random>
#include <cmath>

using namespace std;

extern std::mt19937 mt;
extern std::uniform_real_distribution<> zeroUm;

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
extern float G1;
extern float G2;

extern const float Lxs;           
extern const float Uxs;    
extern const float Lys;              
extern const float Uys;

constexpr int calculateMaxS() {
    return std::ceil((static_cast<float>(W) / (2 * (RS - RE))) * (static_cast<float>(H) / (2 * (RS - RE))));
}

extern const short MinS;     
constexpr int MaxS = calculateMaxS(); 

extern float matCoverage[ H/CELL_SIZE ][ W/CELL_SIZE ];   //faz saber se os sensores as celulas estao supervisionadas


struct HarmonySearch {        // Toda a matriz de soluçao
  std::string x[HMS][MaxS];
  std::string y[HMS][MaxS];
  bool ativado[HMS][MaxS];
};

struct HarmonyVector {          // Um vetor de soluçao
  std::string x[MaxS];
  std::string y[MaxS];
  bool ativado[MaxS];
};



void  ajuste_de_pitch(string newSoluctX, string newSoluctY);
void  calc_gama(float distEuc);
float calc_pcov(float dist, float probabilidadeParcial);
float calc_psov(float probabilidadeParcial, float probabilidadeRecemCalculada);
float cratio(float * ponteiro);
float dist_euclidiana(float xSens, float ySens, float xPonto, float yPonto);
int   hmv_len ();
void  iniciar_hm (string * x, string * y, bool * ativado);
void  matriz_pcov(string * x, string * y, bool * ativado);
float min_dist(string *x, string *y, bool *ativado);
int   num_sensores (bool *ativado);
float obj_cov (bool *ativado, float cratio, float minDist);
void  printar_best (string * x, string * y, bool * ativado);
void  printar_hm (string * x, string * y, bool * ativado);
void  printar_pdp(float * ponteiro);
void  zerar_mat_pcov(float * ponteiro);


#endif
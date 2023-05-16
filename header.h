#include "funcs.cpp"
using namespace std;

#ifndef HEADER_H
#define HEADER_H

void PrintarHMS2 (string * x, string * y, bool * ativado, int hms, int maxS);
void PrintarPDP(float * ponteiro, int Width, int height, int cellSize) ;
void ZerarMatAux(float * ponteiro, int h, int w, int cellSize);

float DistEuclidiana(float xSens, float ySens, float xPonto, float yPonto);
float Cratio(float * ponteiro, int width, int height, int cellSize, float cth) ;
float CalcPsov(float probabilidadeParcial, float probabilidadeRecemCalculada);
float ObjCov2 (bool *ativado, float cratio, float minDist, int maxS);
float MinDist2(string *x, string *y, bool *ativado, int maxS, int w, int h, int rs, int re);

#endif
#include "funcs.cpp"
using namespace std;

#ifndef HEADER_H
#define HEADER_H

void PrintarHMS (string * p, int hms, int maxS);
void PrintarPDP(float * ponteiro, int Width, int height, int cellSize) ;
void ZerarMatAux(float * ponteiro, int h, int w, int cellSize);

float DistEuclidiana(float xSens, float ySens, float xPonto, float yPonto);
float Cratio(float * ponteiro, int width, int height, int cellSize, float cth) ;
float ObjCov (string *p, float cratio, int maxS);

#endif
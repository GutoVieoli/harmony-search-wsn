#include <iostream>
#include <random>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <fstream>

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
int const Rs = 5;                 // A faixa de detecção do sensor
float const Re = 2.5f;                  // Incerteza da detecção do sensor
int const CellSize = 10;           // Tamanho da célula
float const Cth = 0.9;               // Limite de cobertura
float const A1 = 1;
float const A2 = 0;
float const B1 = 1;
float const B2 = 0.5;
float G1;
float G2;

const float Lxs = Rs - Re;                // Limite inferior de pos x do sensor
const float Uxs = W - (Rs - Re);          // Limite superior de pos x do sensor
const float Lys = Rs - Re;                // Limite inferior de pos y do sensor
const float Uys = H - (Rs - Re);          // Limite superior de pos y do sensor
const short MinS = std::ceil( (static_cast<float>(W) / (2 * (Rs + Re))) * ( static_cast<float>(H) / (2 * (Rs + Re))) ) ;     // Mínimo de sensores da rede   2,78...
const short MaxS = std::ceil( (static_cast<float>(W) / (2 * (Rs - Re))) * ( static_cast<float>(H) / (2 * (Rs - Re))) ) ;   // Máximo de sensores da rede

float matCoverage[ H/CellSize ][ W/CellSize ] = {0};   //faz saber se os sensores as celulas estao supervisionadas

// Comprimento de cada vetor da memória harmonica
int hmvLen ()
{
    return ( MinS + ((MaxS+1 - MinS) * zeroUm(mt)) );
}

// Inicialização da Memória Harmônica
void iniciarHMS (string * x, string * y, bool * ativado)
{

    for( int i = 0; i < HMS; i++ ){
        int sensores = 0;

        for( int j = 0; j < MaxS; j++)
        {
            *x = to_string( floorf((Lxs + (Uxs - Lxs) * zeroUm(mt)) * 10) / 10 );  //eq 15
            x++;
            *y = to_string( floorf((Lys + (Uys - Lys) * zeroUm(mt)) * 10) / 10);  //eq 15
            y++;

            if(zeroUm(mt) >= 0.35f)
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

        if(sensores < MinS){  //Caso o nº de sensores ativados < MinS, reseta o vetor 
            i--;
            x -= MaxS;
            y -= MaxS;
            ativado -= MaxS;
        }


    }
}

// Inicialização da Memória Harmônica
void iniciarHMS2 (string * x, string * y, bool * ativado)
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

        int sensores = hmvLen();

        for ( sensores; sensores > 0; sensores-- )
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

void printarHMS (string * x, string * y, bool * ativado, int hms, int maxS)
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

float distEuclidiana(float xSens, float ySens, float xPonto, float yPonto)
{
    float d = sqrt(pow(xSens - xPonto, 2) + pow(ySens - yPonto, 2));
    return d;
}

float minDist(string *x, string *y, bool *ativado, int maxS, int w, int h, float rs, float re)
{
    float menorDist = 999999999;
    string cpyStr[maxS][3];
    float wm = w - (rs - re);
    float hm = h - (rs - re);
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
                float distEuclidian = distEuclidiana( std::stof(cpyStr[i][0]), std::stof(cpyStr[i][1]), std::stof(cpyStr[j][0]), std::stof(cpyStr[j][1]) );
                if( distEuclidian < menorDist)
                {
                    menorDist = distEuclidian;
                }
            }
        }
    }
    return menorDist / denominador;
}

void calcGama(float distEuc)
{
    G1 = (Re - Rs) + distEuc;
    G2 = (Re + Rs) - distEuc;
}

void zerarMatPcov(float * ponteiro, int h, int w, int cellSize)
{
    for ( int i = 0; i < (h/cellSize)*(w/cellSize); i++)
    {
        *ponteiro = 0;
        ponteiro++;
    }
}

float calcPsov(float probabilidadeParcial, float probabilidadeRecemCalculada)
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

float calcPcov(float dist, float probabilidadeParcial)   //probabilidade parcial é o valor que ja esta salvo na matriz aux naquela posiçao
{
    if( dist <= Rs - Re )
        return 1;
    else if ( dist > Rs + Re )
        return probabilidadeParcial;
    else {
        float e = 2.718281828459;
        float quocienteElev = A1 * pow(G1, B1);
        float dividendoElev = pow(G2, B2);
        float elev = -( quocienteElev / dividendoElev + A2);
        //return pow(e, elev);
        return calcPsov(probabilidadeParcial, pow(e, elev));
    }
}

void matrizPcov(string * x, string * y, bool * ativado)
{
    for(int j = 1; j <= MaxS; j++)
    {
        if(*ativado != false)
        {
            for( int k = 1; k <= H / CellSize; k++)    // Categorization of the coverage probability for each demand point.
            {    
                for( int l = 1; l <= W / CellSize; l++)
                {
                    float dist = distEuclidiana( stof(*x), stof(*y),  (l*CellSize)-( CellSize/2.0f), (k*CellSize)-(CellSize/2.0f) );
                    calcGama(dist);
                    matCoverage[ k-1 ][ l-1 ] = calcPcov(dist, matCoverage[ k-1 ][ l-1 ]);
                }
            }
        }
        x++;
        y++;
        ativado++;
    }
}

void printarPDP(float * ponteiro, int width, int height, int cellSize)  //PRINTAR PROBABILIDADE DO PONTO DE DEMANDA 
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

float objCov (bool *ativado, float cratio, float minDist, int maxS)
{
    float s = 0;   // Calcular o numero de sensores utilizados
    for( int i = 0; i < maxS; i++ )
    {
        if ( *ativado == 1){
            s++;
        }
        ativado++;
    }
    
    //return (1/s) * cratio * 100;
    return ( (1/s) * cratio * ((minDist/2) + 1)  + (cratio / 30) ) * 10;
}

float cRatio(float * ponteiro, int width, int height, int cellSize, float cth) 
{
    float neff = 0;
    float nall = 0;
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

void ajustePitch(string newSoluctX, string newSoluctY)
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

int numSensores (bool *ativado, int maxS)
{
    float s = 0;   // Calcular o numero de sensores utilizados
    for( int i = 0; i < maxS; i++ )
    {
        if ( *ativado == 1){
            s++;
        }
        ativado++;
    }

    return s;
}

struct HarmonySearch {    // Toda a matriz de soluçao
  std::string x[HMS][MaxS];
  std::string y[HMS][MaxS];
  bool ativado[HMS][MaxS];
};

struct HarmonyNew {    // Um vetor e soluçao
  std::string x[MaxS];
  std::string y[MaxS];
  bool ativado[MaxS];
};

int main(void){

    for(int repeat = 0; repeat < 30; repeat ++){

        srand(time(NULL));
        HarmonySearch harm;
        HarmonyNew newSoluction;
        string * pontX = &harm.x[0][0];
        string * pontY = &harm.y[0][0];
        bool * pontAtivado = &harm.ativado[0][0];
        float * pontMatCoverage = &matCoverage[0][0];

        float objetivos[HMS];
        float cratios[HMS];
        float piorObj;
        int indicePiorObj;

        iniciarHMS2(pontX, pontY, pontAtivado);
        //printarHMS(pontX, pontY, pontAtivado, HMS, MaxS); 

        zerarMatPcov(pontMatCoverage, W, H, CellSize);
        matrizPcov(pontX, pontY, pontAtivado); 
        //printarPDP(pontMatCoverage, W, H, CellSize);


        for(int i = 0; i < HMS; i++)
        {
            pontX = &harm.x[i][0];
            pontY = &harm.y[i][0];
            pontAtivado = &harm.ativado[i][0];

            matrizPcov(pontX, pontY, pontAtivado); 
            cratios[i] = cRatio(pontMatCoverage, W, H, CellSize, Cth);
            objetivos[i] = objCov(pontAtivado, cratios[i], minDist(pontX, pontY, pontAtivado, MaxS, W, H, Rs, Re), MaxS); 
            //cout << "\n" << i+1 << " - Cratio: " << cratios[i] << "  - Obj: " << objetivos[i];
            zerarMatPcov(pontMatCoverage, W, H, CellSize);

            if( i == 0 || piorObj > objetivos[i] )
            {
                piorObj = objetivos[i];
                indicePiorObj = i;
            }
        }

        cout << "\nO pior objetivo eh " << piorObj << " na posicao " << indicePiorObj ;


        for ( int iter = 0; iter <= NI; iter++)
        {
            int numSensorsOFF = 0;
            // { Improvise a new soluction vector }
            for( int i = 0; i < MaxS; i++)
            {
                // { Check to be selected from HM }
                if( zeroUm(mt) <= HMCR )
                {
                    int randLoc = (int) (HMS * zeroUm(mt));
                    newSoluction.x[i] = harm.x[ randLoc ][i];               // pos x
                    newSoluction.y[i] = harm.y[ randLoc ][i];                // pos y
                    newSoluction.ativado[i] = harm.ativado[ randLoc ][i] ;   // ativado  

                    if(zeroUm(mt) <= 0.05f)
                        newSoluction.ativado[i] = ( newSoluction.ativado[i] == false ? true : false);

                    if( newSoluction.ativado[i] != false )
                    {
                        // { Check to be pitch adjusted }
                        if( zeroUm(mt) <= PAR )
                        {
                            ajustePitch(newSoluction.x[i], newSoluction.y[i]);
                        }
                    }
                    else
                    {
                        numSensorsOFF++;
                    }
                }
                else 
                {
                    // { Choose a random variable }
                    newSoluction.x[i] = to_string( floorf((Lxs + (Uxs - Lxs) * zeroUm(mt)) * 10) / 10 );    // pos x
                    newSoluction.y[i] = to_string( floorf((Lys + (Uys - Lys) * zeroUm(mt)) * 10) / 10);   // pos y
                    newSoluction.ativado[i] = true;
                }
            }
            // { Update HM }
            if ( numSensorsOFF  <= MaxS - MinS )
            {
                pontX = &newSoluction.x[0];
                pontY = &newSoluction.y[0];
                pontAtivado = &newSoluction.ativado[0];
                matrizPcov(pontX, pontY, pontAtivado);
                float novoCratio = cRatio(pontMatCoverage, W, H, CellSize, Cth);
                float novoObjetivo = objCov(pontAtivado, novoCratio, minDist(pontX, pontY, pontAtivado, MaxS, W, H, Rs, Re), MaxS);

                if( novoObjetivo > piorObj || ( novoObjetivo == piorObj && novoCratio >= cratios[indicePiorObj] ) )
                {
                    objetivos[indicePiorObj] = novoObjetivo;
                    cratios[indicePiorObj] = novoCratio;

                    for( int j = 0; j < MaxS; j++)    // Substitui a pior soluçao pela gerada
                    {
                        harm.x[indicePiorObj][j] = newSoluction.x[j];
                        harm.y[indicePiorObj][j] = newSoluction.y[j];
                        harm.ativado[indicePiorObj][j] = newSoluction.ativado[j];
                    }
                    for(int j = 0; j < HMS; j++)  // Encontra a nova pior soluçao
                    {
                        if( j == 0 || objetivos[j] < piorObj)
                        {
                            indicePiorObj = j;
                            piorObj = objetivos[j];
                        }
                    }
                }

            }
            zerarMatPcov(pontMatCoverage, W, H, CellSize);

        }

        cout << "\n";
        int melhorObjPos = 0;

        ofstream myfile ("out.txt", std::ios::app);  //______________________________________________________
        for(int i = 0; i < HMS; i++){
            if( objetivos[i] > objetivos[melhorObjPos] )
                melhorObjPos = i;
            pontAtivado = &harm.ativado[i][0];
            cout << "\n " << i << " - Cratio: " << cratios[i] << " - Objetivo: " << objetivos[i] << " - Sensores: " << numSensores(pontAtivado, MaxS);
            //myfile << "\n " << i << " - Cratio: " << cratios[i] << " - Objetivo: " << objetivos[i] << " - Sensores: " << 1 / ( objetivos[i] / (cratios[i]*100)  );
            // pontX = &harm.x[i][0];
            // pontY = &harm.y[i][0];
            // pontAtivado = &harm.ativado[i][0];

            // matrizPcov(pontX, pontY, pontAtivado);  
            // float novoCratio =  cRatio(pontMatCoverage, W, H, CellSize, Cth);
            // objetivos[i] = objCov(pontAtivado, novoCratio, minDist(pontX, pontY, pontAtivado, MaxS, W, H, Rs, Re), MaxS); 
            // cout << "\n" << i+1 << " - Cratio: " << cRatio(pontMatCoverage, W, H, CellSize, Cth) << "  - Obj: " << objetivos[i] << "  - Sensores: " << 1 / ( (objetivos[i] - (novoCratio / 6)) / (novoCratio*100)  );
            // zerarMatPcov(pontMatCoverage, W, H, CellSize);

            //cout << i+1 << " - " << objCov(pontAtivado, 5, 5, MaxS) << " " << minDist(pontX, pontY, pontAtivado, MaxS, W, H, Rs, Re) << "\n";
        }
        myfile << "\n " << melhorObjPos << " - Cratio: " << cratios[melhorObjPos] << " - Objetivo: " << objetivos[melhorObjPos] << " - Sensores: " << numSensores(&harm.ativado[melhorObjPos][0], MaxS);
        myfile << "\n";

        pontX = &harm.x[0][0];
        pontY = &harm.y[0][0];
        pontAtivado = &harm.ativado[0][0];
        //printarHMS(pontX, pontY, pontAtivado, HMS, MaxS); 

    }
    return 0;
}
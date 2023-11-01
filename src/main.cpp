#include <iostream>
#include <iomanip>
#include <fstream>
#include "cabecalho.h"

using namespace std;



int main(void){

    for(int repeat = 0; repeat < 30; repeat ++){

        srand(time(NULL));
        HarmonySearch harm;
        HarmonyVector newSoluction;
        string * pontX = &harm.x[0][0];
        string * pontY = &harm.y[0][0];
        bool * pontAtivado = &harm.ativado[0][0];
        float * pontMatCoverage = &matCoverage[0][0];

        float objetivos[HMS];
        float cratios[HMS];
        float piorObj;
        int indicePiorObj;

        iniciar_hm(pontX, pontY, pontAtivado);
        zerar_mat_pcov(pontMatCoverage);
        matriz_pcov(pontX, pontY, pontAtivado); 
        //printar_hm(pontX, pontY, pontAtivado); 
        //printar_pdp(pontMatCoverage);


        for(int i = 0; i < HMS; i++)
        {
            pontX = &harm.x[i][0];
            pontY = &harm.y[i][0];
            pontAtivado = &harm.ativado[i][0];

            matriz_pcov(pontX, pontY, pontAtivado); 
            cratios[i] = cratio(pontMatCoverage);
            objetivos[i] = obj_cov(pontAtivado, cratios[i], min_dist(pontX, pontY, pontAtivado)); 
            zerar_mat_pcov(pontMatCoverage);

            if( i == 0 || piorObj > objetivos[i] )
            {
                piorObj = objetivos[i];
                indicePiorObj = i;
            }
        }

        //cout << "\nO pior objetivo eh " << piorObj << " na posicao " << indicePiorObj ;


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

                    if(zeroUm(mt) <= SFP)
                        newSoluction.ativado[i] = ( newSoluction.ativado[i] == false ? true : false);

                    if( newSoluction.ativado[i] != false )
                    {
                        // { Check to be pitch adjusted }
                        if( zeroUm(mt) <= PAR )
                        {
                            ajuste_de_pitch(newSoluction.x[i], newSoluction.y[i]);
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
                matriz_pcov(pontX, pontY, pontAtivado);
                float novoCratio = cratio(pontMatCoverage);
                float novoObjetivo = obj_cov(pontAtivado, novoCratio, min_dist(pontX, pontY, pontAtivado));

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
            zerar_mat_pcov(pontMatCoverage);

        }


        cout << "\n";
        int melhorObjPos = 0;

        for(int i = 0; i < HMS; i++){
            if( objetivos[i] > objetivos[melhorObjPos] )
                melhorObjPos = i;
        }

        cout << "\n "  << "Melhor cobertura: " << cratios[melhorObjPos] << " | Sensores Utilizados: " << num_sensores(&harm.ativado[melhorObjPos][0]) << " | Valor da funcao objetivo: " << objetivos[melhorObjPos] << "\n";
        printar_best (&harm.x[melhorObjPos][0], &harm.y[melhorObjPos][0], &harm.ativado[melhorObjPos][0]);

        pontX = &harm.x[0][0];
        pontY = &harm.y[0][0];
        pontAtivado = &harm.ativado[0][0]; 

    }
    return 0;
}
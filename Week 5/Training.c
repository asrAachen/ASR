/*****************************************************************************/
/*                                                                           */
/* File: Training.c                                                          */
/*                                                                           */
/* COPYRIGHT (C) Lehrstuhl fuer Informatik VI                                */
/*               RWTH Aachen                                                 */
/*                                                                           */
/* Authors: W. Macherey                                                      */
/*                                                                           */
/* Project: Lehrstuhl Praktikum                                              */
/*                                                                           */
/* Last Update: Sam Feb  8 00:30:49 CET 2003                                 */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "GlobalSettings.h"
#include "Lexicon.h"
#include "SieTill.h"
#include "Training.h"


double a_k(int k, float x[][N_CMP_TOTAL], int begin, int end) 
{
  static double storedResult = 0;
  double result, sum;
  int i;

  if(k == 3) return storedResult;

  sum = 0;
  for (i = begin; i <= end; ++i) {
    sum += x[i][0];
  }

  result = sum/(end-begin);

  if(k == 1) storedResult = result;  
  return result;
}

/*****************************************************************************/
void LinearSegmentation(float ObsSequ[][N_CMP_TOTAL], int PthSent[], int N_tim, int N_sta)
/*****************************************************************************/
{
  int k, firstPoint, secondPoint, i, n[4];
  double sum;
  double minSum;
  int bestN1, bestN2;
  FILE *plot;

  n[0] = 0;
  n[3] = N_tim;
  
  minSum = 0;
  for (firstPoint = n[0] + 2; firstPoint < n[3]-4; ++firstPoint) {  
    n[1] = firstPoint;
    for (secondPoint = firstPoint + 2; secondPoint < n[3]-2; ++secondPoint) {
      n[2] = secondPoint;
        sum = 0;
        for (k = 1; k <= 3 ; ++k) {
          for(i = n[k-1] + 1; i<=n[k]; ++i) {
            sum+= pow(ObsSequ[i][0] - a_k(k, ObsSequ, n[k-1]+1, n[k]), 2);
        }
      }
      if(minSum == 0) {
        minSum = sum;
      }else if (sum < minSum) {
        minSum = sum;
        bestN1 = firstPoint;
        bestN2 = secondPoint;
      }
    }
  }

  printf("\nSegments = %d - %d, %d - %d, %d - %d \n", n[0], bestN1, bestN1+1, bestN2, bestN2+1, n[3]);

  plot = fopen("forPlot.dat", "w");
  for (i = 0; i<N_tim; ++i) {
    fprintf(plot,"%f %d\n", ObsSequ[i][0], i);
  }

  fclose(plot);
  

  /* Assign silence regions ?? */
  for (k = 1; k <= N_tim; ++k) {
    PthSent[k] = floor((float)k/N_tim * N_sta);
  }
}

/*****************************************************************************/
void MarkovAutomaton(int SpkSent[], int MixSent[], int *N_sta)
/*****************************************************************************/
{
	int i, j, wordLength;
  int totalStates = 0;

	for (i = 1; i <= SpkSent[0]; ++i) {
		MixSent[totalStates++] = Get_MixWord(SilWrdIdx(), 1);
    wordLength = Get_LengthWord(SpkSent[i]);
    for(j = 1; j <= wordLength; ++j) {
      MixSent[totalStates++] = Get_MixWord(SpkSent[i], j);
    }
	}
  MixSent[totalStates++] = Get_MixWord(SilWrdIdx(), 1);
  *N_sta = totalStates;
}

/*****************************************************************************/
void Training(char **FileList, int N_snt, char *PrototypeFile)
/*****************************************************************************/
{
  char Filename[BUFSIZ];
  int SpkSent[M_WRD];
  int MixSent[M_STA];
  int PthSent[M_TIM];  
  float ObsSequ[M_TIM][N_CMP_TOTAL];
  char *c_ptr, TmpStr[BUFSIZ];
  int snt, wrd, N_tim, N_sta;
  int M_wrd, M_mix, M_sta;

  /* read lexicon */
  Lexicon_Read(LEXICON_FILE, &M_wrd, &M_mix, &M_sta);

  for(snt=1; snt<=N_snt; ++snt) {
      
    printf("\rN_snt=%d, snt=%d", N_snt, snt);

    /* read filename */
    strcpy(TmpStr, FileList[snt-1]);
    strcpy(Filename, strtok(TmpStr, " \t"));
      
    /* read transcription */
    wrd=0;
    while((c_ptr=strtok(NULL, " \t"))!=NULL) SpkSent[++wrd]=atoi(c_ptr);
    SpkSent[0]=wrd;
      
    /* read features and calculate derivatives */
    ReadFeatures(Filename, ObsSequ, &N_tim);
      
    /* build Markov automaton */
    MarkovAutomaton(SpkSent, MixSent, &N_sta);
      
    /* linear segmentation */
    LinearSegmentation(ObsSequ, PthSent, N_tim, N_sta);
  }
}

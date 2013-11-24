/*****************************************************************************/
/*                                                                           */
/* File: SignalAnalysis.c                                                    */
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
#include <math.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#include "GlobalSettings.h"
#include "SignalAnalysis.h"

/*****************************************************************************/
static void Calc_Mel_Filter_Cepstrum(double *Abs_Fourier, FILE *fp)
/*****************************************************************************/
/* this is just a dummy function! */
{
  float feature;
  int cmp;

  for(cmp=0; cmp<N_CMP_FILE; cmp++) { 
    feature=0.0;
    fwrite(&feature, sizeof(float), 1, fp);

  }
}

/*****************************************************************************/
static void Abs_Spectrum(double *Re_Fourier, double *Im_Fourier, FILE *fp)
/*****************************************************************************/
{
  static double Abs_Fourier[DFT_LENGTH/2+1];
  int i;


  FILE *fp2 = fopen("Question 2a.txt", "ab");
  
  FILE *fp3 = fopen("Question 2b.txt", "ab");


  double fft_Energy=0.0;
  double logspectralval = 0.0;

  for(i=0; i<=DFT_LENGTH/2; i++){
  //for(i=0; i<DFT_LENGTH/2; i++){
    Abs_Fourier[i]=sqrt(Re_Fourier[i]*Re_Fourier[i] + Im_Fourier[i]*Im_Fourier[i]);

    logspectralval = log10(Abs_Fourier[i]);
    fprintf(fp2, "%f\n", logspectralval);
    fft_Energy+= logspectralval;
  }

  fprintf(fp3, "%f\n",fft_Energy);
  Calc_Mel_Filter_Cepstrum(Abs_Fourier, fp);

  fclose(fp2);
  fclose(fp3);
}

/*****************************************************************************/
static void FFT(double *Re_Signal, FILE *fp)
/*****************************************************************************/
{
  static int fst_call=1;
  static double Cos[DFT_LENGTH/2];
  static double Sin[DFT_LENGTH/2];
  static double Re_Fourier[DFT_LENGTH];
  static double Im_Fourier[DFT_LENGTH];
  const double phi=M_PI/((double) (DFT_LENGTH/2));
  const double fact=1.0/((double) DFT_LENGTH);

  double re_w, im_w, tmp, tmp1, tmp2;
  size_t N2, j, m, Mm;
  int i, k;

  if(fst_call) {
    for(i=0; i<DFT_LENGTH/2; i++) {
      Cos[i]= cos(((double) i)*phi);
      Sin[i]=-sin(((double) i)*phi);
    }
    fst_call=0;
  }

  for(i=0; i<DFT_LENGTH; i++) {
    Re_Fourier[i]=Re_Signal[i]*fact;
    Im_Fourier[i]=0.0;
  }

  j=0;
  N2=DFT_LENGTH>>1;
  for(i=1; i<DFT_LENGTH; i++) {
    m=N2;
    while(m<=j) {
      j-=m;
      m>>=1;
    }
    j+=m;
    if(j>i) {
      tmp=Re_Fourier[i]; Re_Fourier[i]=Re_Fourier[j]; Re_Fourier[j]=tmp;
      tmp=Im_Fourier[i]; Im_Fourier[i]=Im_Fourier[j]; Im_Fourier[j]=tmp;
    }
  }
  
  Mm=1;
  do {
    m=Mm;
    Mm<<=1;
    for(k=0; k<m; k++) {
      re_w=Cos[k*N2];
      im_w=Sin[k*N2];
      for(i=k; i<DFT_LENGTH; i+=Mm) {
	j=i+m;
	tmp1=re_w * Re_Fourier[j] - im_w * Im_Fourier[j];
	tmp2=im_w * Re_Fourier[j] + re_w * Im_Fourier[j];
	Re_Fourier[j]=Re_Fourier[i]-tmp1;
	Im_Fourier[j]=Im_Fourier[i]-tmp2;
	Re_Fourier[i]+=tmp1;
	Im_Fourier[i]+=tmp2;
      }
    }
    N2>>=1;
  } while(Mm<DFT_LENGTH);

  Abs_Spectrum(Re_Fourier, Im_Fourier, fp);
}

/*****************************************************************************/
static void ZeroPadding(double *Re_Signal, int N_sig)
/*****************************************************************************/
{
  int i;

  for(i=N_sig; i<DFT_LENGTH; i++) Re_Signal[i]=0.0;
}

/*****************************************************************************/
static void Windowing(short *Samples, int N_smp, double *Re_Signal, FILE *fp)
/*****************************************************************************/
{
  static int fst_call=1;
  static double Window[WINDOW_SIZE];
  int i, offs=0;

  if(fst_call) {
    /* first call: initialize Window */
    for(i=0; i<WINDOW_SIZE; i++) Window[i]=0.54-0.46*cos(2.0*M_PI*((double)i)/((double) WINDOW_SIZE-1));
    fst_call=0;
  }

  /* perform windowing using a Hamming window */
  while(offs<N_smp) {
    for(i=0; i<WINDOW_SIZE && offs+i<N_smp; i++) Re_Signal[i]=Window[i]*Samples[offs+i];
    ZeroPadding(Re_Signal, i);
    FFT(Re_Signal, fp);
    offs+=WINDOW_SHIFT; 
  }
}

/*****************************************************************************/
static void PreEmphasis(short *Samples, int N_smp, FILE *fp)
/*****************************************************************************/
{
  int smp;
  short presample, sample;
  double Re_Signal[DFT_LENGTH];

  presample=Samples[0];
  for(smp=1; smp<N_smp; smp++) {
    sample=Samples[smp];
    if(presample<0 && sample>0 && sample<presample) Samples[smp]=SHRT_MAX;
    else if(presample>0 && sample<0 && sample>presample) Samples[smp]=SHRT_MIN;
    else Samples[smp]-=presample;
    presample=sample;
  }

  Windowing(Samples, N_smp, Re_Signal, fp);
}

/*****************************************************************************/
void SignalAnalysis(char *Filename, int snt)
/*****************************************************************************/
{
  FILE *fp_samp, *fp_feat;
  int N_smp=0;
  static short Samples[M_SAMPLES];
  char FeatureFile[BUFSIZ];

  float CUR_SAMPLERATE = 16000;


  FILE *fp1 = fopen("Question 1.txt", "wb");

  printf("proceed %-60s (%2d) ", Filename, snt);
  strcpy(FeatureFile, Filename); strcpy(FeatureFile+strlen(Filename)-4, ".mm2");

  /* open wave file */
  if((fp_samp=fopen(Filename, "rb"))==NULL) {
    fprintf(stderr, "\nError: cannot open %s\n",Filename);
    exit(EXIT_FAILURE);
  }

  /* open feature file */
  if((fp_feat=fopen(FeatureFile, "wb"))==NULL) {
    fprintf(stderr, "\nError: cannot open %s\n",FeatureFile);
    exit(EXIT_FAILURE);
  }

  /* skip 1024 byte header */
  fseek(fp_samp, 1024, SEEK_SET);

  while(fread(&Samples[N_smp], sizeof(short), 1, fp_samp))
  {
    fprintf(fp1, "%d\t", Samples[N_smp]);
    fprintf(fp1, "%f\n", N_smp/CUR_SAMPLERATE);
    N_smp++;
  } 

  printf("N_smp=%d\n",N_smp);


  PreEmphasis(Samples, N_smp, fp_feat);

  /* close files */
  fclose(fp1);
  fclose(fp_feat);
  fclose(fp_samp);
}

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

static int computedFilterBank = 0;
static int triangleCentres[N_MEL_FILTERS + 2];

/*****************************************************************************/
static double Freq_To_Mel(double freq)
/*****************************************************************************/
{
  return 2595 * log10(1 + freq/700.0);
}

/*****************************************************************************/
static double Mel_To_Freq(double freq)
/*****************************************************************************/
{
  return 700 * (pow(10, freq/2595.0) - 1);
}


/*****************************************************************************/
static void ComputeFilterBank()
/*****************************************************************************/
{
  double minFreq = Freq_To_Mel(0.0);
  double maxFreq = Freq_To_Mel(SAMPLE_RATE/2);

  double h[N_MEL_FILTERS + 2];
  int i;

  for(i = 0; i < N_MEL_FILTERS + 2; ++i) {
    h[i] = Mel_To_Freq(minFreq+((maxFreq - minFreq) * i / (N_MEL_FILTERS + 1)));
  }


  for(i = 0; i < N_MEL_FILTERS + 2; ++i) {
    triangleCentres[i] = floor((DFT_LENGTH/2+1)*h[i]/(SAMPLE_RATE/2));
  }

}

/*****************************************************************************/
static double GetCenterFrequency(unsigned int filterBand)
/*****************************************************************************/
{
  if (computedFilterBank == 0) {
    ComputeFilterBank();
    int j;
    for (j = 0; j < N_MEL_FILTERS + 2; ++j) {
      printf("%d ", triangleCentres[j]);
    }
    computedFilterBank = 1;
  }

  return triangleCentres[filterBand];
}


/*****************************************************************************/
static double GetFilterBankValue(unsigned int frequencyBand, unsigned int filterBand)
/*****************************************************************************/
{
  double filterValue = 0.0f;

  double prevCenter = GetCenterFrequency(filterBand - 1);                
  double thisCenter = GetCenterFrequency(filterBand);
  double nextCenter = GetCenterFrequency(filterBand + 1);


  if(frequencyBand < prevCenter) {
    filterValue = 0.0f;
  }
  else if(frequencyBand >= prevCenter && frequencyBand < thisCenter) {
    filterValue = (frequencyBand - prevCenter) / (thisCenter - prevCenter);
  }
  else if(frequencyBand >= thisCenter && frequencyBand < nextCenter) {
    filterValue = (frequencyBand - nextCenter) / (thisCenter - nextCenter);
  }
  else if(frequencyBand >= nextCenter){
    filterValue = 0.0f;
  }
  return filterValue;
}


/*****************************************************************************/
static void Calc_Mel_Filter_Cepstrum(double *Abs_Fourier, FILE *fp)
/*****************************************************************************/
{
  
  float feature;
  double Fn = 0.0;
  int m, n, j;
  for(m = 0; m < N_CMP_FILE; ++m) { 
    feature = 0.0;

    // Calculate the filtered output.
    for(n = 1; n <= N_MEL_FILTERS; ++n) {
      double weightedSpectralSum = 0.0f;
      for(j = 0; j <= DFT_LENGTH/2; ++j) {
        weightedSpectralSum += fabs(Abs_Fourier[j] * GetFilterBankValue(j, n));
      }

      //printf("%f \n", weightedSpectralSum);
      Fn = log(weightedSpectralSum);
      feature += Fn * cos(((m * M_PI) / N_MEL_FILTERS) * (n - 0.5f));
    }
    fwrite(&feature, sizeof(float), 1, fp);
  }
}

/*****************************************************************************/
static void Abs_Spectrum(double *Re_Fourier, double *Im_Fourier, FILE *fp)
/*****************************************************************************/
{
  static double Abs_Fourier[DFT_LENGTH/2 + 1];
  int i;
  
  for(i = 0; i <= DFT_LENGTH/2; i++)
    Abs_Fourier[i] = sqrt(Re_Fourier[i] * Re_Fourier[i] + Im_Fourier[i] * Im_Fourier[i]);

  Calc_Mel_Filter_Cepstrum(Abs_Fourier, fp);
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

  while(fread(&Samples[N_smp], sizeof(short), 1, fp_samp)) N_smp++;
  printf("N_smp=%d\n",N_smp);

  PreEmphasis(Samples, N_smp, fp_feat);

  /* close files */
  fclose(fp_feat);
  fclose(fp_samp);
}

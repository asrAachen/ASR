/*****************************************************************************/
/*                                                                           */
/* File: SieTill.c                                                           */
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
#include "SieTill.h"

/*****************************************************************************/
int main(int argc, const char *argv[])
/*****************************************************************************/
{
  FILE *fp;
  char line[BUFSIZ];
  char *c_ptr;
  char **FileList=NULL;
  int snt, N_snt=0;

  if(argc!=3) {
    fprintf(stderr, "Usage: %s [-sig|-train|-rec] file_list\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* open file list  and create file list*/
  if((fp=fopen(argv[2], "r"))==NULL) {
    fprintf(stderr, "Error: cannot open file %s\n", argv[2]);
    exit(EXIT_FAILURE);
  }

  while(fgets(line, BUFSIZ, fp)!=NULL) {
    if((c_ptr=strpbrk(line, "\n"))!=NULL) line[c_ptr-line]='\0';

    FileList=(char**) realloc(FileList, ++N_snt*sizeof(char*));
    FileList[N_snt-1]=(char*) calloc(strlen(line)+1, sizeof(char));
    strcpy(FileList[N_snt-1], line);
  }

  /* close file */
  fclose(fp);

  if(strcmp(argv[1], "-sig")==0) {
    /* proceed over training/test samples and perform signal analysis */
    for(snt=0; snt<N_snt; snt++) {
      SignalAnalysis(FileList[snt], snt);
    }
  }
  else if(strcmp(argv[1], "-train")==0) {
    /* Training(FileList, N_snt); */
    printf ("Error: training not implemented\n");
  }
  else if(strcmp(argv[1], "-rec")==0) {
    /* Recognizer(FileList, N_snt); */
    printf ("Error: recognizer not implemented\n"); 
  }
  else {
    fprintf(stderr, "Error: unknown option %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  if(0<N_snt) {
    for(snt=0; snt<N_snt; snt++) free(FileList[snt]);
    free(FileList);
  }
  
  exit(EXIT_SUCCESS);
}

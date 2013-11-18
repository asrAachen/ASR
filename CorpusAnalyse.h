#ifndef CORPUS_ANALYSE_H
#define CORPUS_ANALYSE_H

#include "Ngrams.h"

class CorpusAnalyse {
public:
    CorpusAnalyse(const NGramsMap* aFreq, const NGramsMap* aVocabFreq, const size_t& aSize);
    size_t count(const std::string& key);
    size_t countOfCount(NGramType type, int count);
    size_t countOfCount(NGramType type,const std::string& v, int count);
    double p(const std::string& w);
    double p(const std::string& w, const std::string& v);
    double perplexity(const std::string& fileName, NGramType type, bool includeStartSymbol = true);
    
private:
    int countWords(const std::string& sentence);
    double max(double a, double b);
        
private:
    const NGramsMap* freq;
    const NGramsMap* vocabFreq;
    double b_bi;
    double b_uni;
    double W;
    double N;
    double N0;
    size_t size;  
};


#endif 
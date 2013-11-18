#include "Ngrams.h"
#include "CorpusAnalyse.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <math.h>


int main() {
    std::cout << "Generating ngrams frequency list .." << std::endl;
    NGramReader* ngr = new NGramReader("corpus", "vocab");
    CorpusAnalyse* ca = new CorpusAnalyse(ngr->recomputedMap(), ngr->vocab(), ngr->corpusSize());

    const NGramsMap* write = ngr->recomputedMap();
// Generate output file
    FILE* fOut = fopen("recomputedfrequencies.txt", "w");
    for (auto i = write->begin(); i != write->end(); ++i) {
        fprintf(fOut, "%d\t%s\n", i->second, i->first.c_str());
    }
    fclose(fOut);


    std::cout << "SUM UP PROBABILITIES OF P(W)" << std::endl;
    float total = 0;
    for (auto i = ngr->vocab()->begin(); i != ngr->vocab()->end(); ++i) {
        total += ca->p(i->first);
    }
    std::cout << "SUM : " << total << std::endl;


    total = 0;
    std::cout << "SUM UP PROBABILITIES OF P(W|V)" << std::endl;

    for (auto i = ngr->vocab()->begin(); i != ngr->vocab()->end(); ++i) {
        total += ca->p(i->first, "the");
    }
    std::cout << "SUM : " << total << std::endl;

    std::cout << "PERPLEXITY" << std::endl;

    std::cout << "PP = " << ca->perplexity("test", EBigram, false) <<std::endl;
    delete ngr;
    delete ca;
    return 0;
}
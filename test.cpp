#include "Ngrams.h"
#include "CorpusAnalyse.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <math.h>


int main() {
    std::cout << "Generating ngrams frequency list .." << std::endl;
    NGramReader* ngr = new NGramReader("../corpus", "../vocab");
    CorpusAnalyse* ca = new CorpusAnalyse(ngr->frequencyMap(), ngr->vocab(), ngr->corpusSize());


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

    std::cout << "PP = " << ca->perplexity("../test", EBigram) <<std::endl;
    delete ngr;
    delete ca;
    return 0;
}
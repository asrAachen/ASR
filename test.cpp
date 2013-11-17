#include "Ngrams.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <math.h> 

static double b_bi, b_uni, W, N, N0,N0_1;
static NGramReader* ngr;

double max(double a, double b)
{
    if (a>b) {
        return a;
    }
    return b;
}

double p(const std::string& w) 
{
    return max((1.0*(ngr->count(w) - b_uni))/N,0) + (b_uni * (W-N0)/(N*W));
}


double p(const std::string& w, const std::string& v) 
{
    return max((1.0*(ngr->count(v+" "+w) - b_bi))/ngr->count(v),0) + 
                    (b_bi * ((1.0*(W - ngr->countOfCount(EBigram, v, 0)))/ ngr->count(v)) * p(w));               
}


int main() {
    std::cout << "Generating ngrams frequency list .." << std::endl;
    ngr = new NGramReader("../corpus", "../vocab");
    const NGramsMap* freqs = ngr->frequencyMap();
    
    int bigram_1 = ngr->countOfCount(EBigram,1);
    b_bi = (1.0*bigram_1)/(bigram_1 + 2*ngr->countOfCount(EBigram,2));
    std::cout << "b_bi = "<< b_bi<< std::endl;
    
    int unigram_1 = ngr->countOfCount(EUnigram,1);
    b_uni = (1.0*unigram_1)/(unigram_1 + 2*ngr->countOfCount(EUnigram,2));    
    std::cout << "b_uni = "<< b_uni<< std::endl;
    
    W = ngr->vocab()->size();
    N = ngr->corpusSize();
    N0 = ngr->countOfCount(EUnigram, 0);
    
    std::cout << "SUM UP PROBABILITIES OF P(W)" << std::endl;
    float total = 0;
    for (auto i = ngr->vocab()->begin(); i != ngr->vocab()->end(); ++i) {
        total += p(i->first);
    }
    std::cout << "SUM : " << total << std::endl;
    
    
    total = 0;
    std::cout << "SUM UP PROBABILITIES OF P(W|V)" << std::endl;
    
    N0_1 = ngr->countOfCount(EBigram, "the", 0);
    
    for (auto i = ngr->vocab()->begin(); i != ngr->vocab()->end(); ++i) {
        total += p(i->first, "the");
    }
    std::cout << "SUM : " << total << std::endl;
    
    std::cout << "PERPLEXITY" << std::endl;
    std::ifstream fs("../test");
    std::string sentence, word;
    float F, denominator;
    std::vector<std::string> words;
    denominator = 0;
    F = 0;
    while (!fs.eof()) {
        words.clear();
        getline(fs,sentence);
        words.push_back("<s>");
        std::stringstream  stream(sentence);
        while (!stream.eof()) {
            stream >> word;
            try {
                    int val = ngr->vocab()->at(word);
                    words.push_back(word);        
                }
            catch (const std::out_of_range& oor) {
                words.push_back("<unk>");
            }
        }
        
        denominator += words.size() - 1;
        for (size_t i = 1; i < words.size(); ++i) {
            F+= log(p(words[i], words[i-1]));
        }
        std::cout << denominator << std::endl;
                 
    }
    
    float l = F/denominator;
    float pp = exp(-l);
    std::cout << "PP = " << pp <<std::endl;
    delete ngr;    
    return 0;
}
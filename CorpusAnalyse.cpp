#include "CorpusAnalyse.h"
#include <iostream>

CorpusAnalyse::CorpusAnalyse(const NGramsMap* aFreq, const NGramsMap* aVocabFreq, const size_t& aSize):
    freq(aFreq),
    vocabFreq(aVocabFreq),
    size(aSize)
{
    int bigram_1 = countOfCount(EBigram,1);
    b_bi = (1.0*bigram_1)/(bigram_1 + 2*countOfCount(EBigram,2));

    int unigram_1 = countOfCount(EUnigram,1);
    b_uni = (1.0*unigram_1)/(unigram_1 + 2*countOfCount(EUnigram,2));

    W = vocabFreq->size();
    N = aSize;
    N0 = countOfCount(EUnigram, 0);
}


size_t CorpusAnalyse::count(const std::string& key)
{
    try {
        return freq->at(key);
    }

    catch (const std::out_of_range& oor) {
        return 0;
    }
}

size_t CorpusAnalyse::countOfCount(NGramType type, int count)
{
    int sum = 0;

    if (count == 0) {
        // Go through the vocab to find words not in corpus.
        for (auto i = vocabFreq->begin(); i != vocabFreq->end(); ++i) {
            try {
                int val = freq->at(i->first);
            }
            catch (const std::out_of_range& oor) {
                sum++;
            }
        }
    } else {
        for (auto i = freq->begin(); i != freq->end(); ++i) {
            if (countWords(i->first) == type && i->second == count) {
                sum++;
            }
        }
    }
    return sum;
}

std::unordered_map<std::string, size_t> ccHash;
size_t CorpusAnalyse::countOfCount(NGramType type,const std::string& v, int count)
{
    auto ccIter = ccHash.find(v);
    if( ccIter != ccHash.end() ) return ccIter->second;

    size_t sum = 0;
    for (auto i = vocabFreq->begin(); i != vocabFreq->end(); ++i) {
        try {
            int val = freq->at(v+" "+i->first);
        }
        catch (const std::out_of_range& oor) {
            sum++;
        }
    }
    ccHash[v] = sum;
    return sum;
}


int CorpusAnalyse::countWords(const std::string& sentence) {
    int count = 0;
    std::stringstream ss(sentence);
    std::string word;
    while( ss >> word ) ++count;
    return count;
}


double CorpusAnalyse::max(double a, double b)
{
    if (a>b) {
        return a;
    }
    return b;
}

double CorpusAnalyse::p(const std::string& w)
{
    return max((1.0*(count(w) - b_uni))/N,0) + (b_uni * (W-N0)/(N*W));
}


double CorpusAnalyse::p(const std::string& w, const std::string& v)
{
    return max((1.0*(count(v+" "+w) - b_bi))/count(v),0) +
           (b_bi * ((1.0*(W - countOfCount(EBigram, v, 0)))/count(v)) * p(w));
}

double CorpusAnalyse::perplexity(const std::string& fileName, NGramType type, bool includeStartSymbol)
{
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
                int val = vocabFreq->at(word);
                words.push_back(word);
            }
            catch (const std::out_of_range& oor) {
                words.push_back("<unk>");
            }
        }

        denominator += words.size() - 1;

        if (type == EUnigram) {
            for (size_t i = 1-includeStartSymbol; i < words.size(); ++i) {
                F+= log(p(words[i]));
            }
        }
        if (type == EBigram) {
            for (size_t i = 2-includeStartSymbol; i < words.size(); ++i) {
                F+= log(p(words[i], words[i-1]));
            }
        }

        std::cout << denominator << std::endl;

    }

    double l = F/denominator;
    return exp(-l);
}
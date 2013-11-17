#include <stdio.h>
#include "Ngrams.h"
#include <stdexcept>
#include <iterator>
#include <iostream>


using namespace std;


NGramReader::NGramReader(const std::string& corpusFileName,
                         const std::string& vocabFileName)
{
    frequencies = new NGramsMap();

    if (!vocabFileName.empty()) {
        processVocab(vocabFileName);
        ofs.open("corpus_vocab");
    }

    fs.open(corpusFileName);
    if(fs.fail()) {
        throw "Error reading the corpus";
    }

    cSize = 0;
    sentence = "";
    while (!fs.eof()) {
        getline(fs,sentence);
        std::stringstream  stream(sentence);
        while (!stream.eof()) {
            string word;
            stream >> word;
            try {
                if (!vocabFileName.empty()) {
                    int val = vocabFrequencies->at(word);
                }
                words.push_back(word);
            }
            catch (const std::out_of_range& oor) {
                words.push_back("<unk>");
            }
        }
        if (!vocabFileName.empty()) {
            writeNewCorpus(words);
        }
        processCorpus(words);

        sentence = "";
        words.clear();
    }
    ofs.close();
}

const NGramsMap* NGramReader::frequencyMap()
{
    return frequencies;
}

const NGramsMap* NGramReader::vocab()
{
    return vocabFrequencies;
}

size_t NGramReader::corpusSize()
{
    return cSize;
}


size_t NGramReader::count(const std::string& key)
{
    try {
        return frequencies->at(key);
    }

    catch (const std::out_of_range& oor) {
        return 0;
    }
}

size_t NGramReader::countOfCount(NGramType type, int count)
{   
    int sum = 0;
    
    if (count == 0) {
        // Go through the vocab to find words not in corpus.
       for (auto i = vocabFrequencies->begin(); i != vocabFrequencies->end(); ++i) {
           try {
               int val = frequencies->at(i->first);
           }
            catch (const std::out_of_range& oor) {
                sum++;
            }
       }
    } else {
         for (auto i = frequencies->begin(); i != frequencies->end(); ++i) {
             if (countWords(i->first) == type && i->second == count) {
                 sum++;
             }
         }
     }
     return sum; 
}

std::unordered_map<std::string, size_t> ccHash;
size_t NGramReader::countOfCount(NGramType type,const std::string& v, int count) 
{
    auto ccIter = ccHash.find(v);
    if( ccIter != ccHash.end() ) return ccIter->second;
    
    size_t sum = 0;
    for (auto i = vocabFrequencies->begin(); i != vocabFrequencies->end(); ++i) {
        try {
            int val = frequencies->at(v+" "+i->first);
        }
        catch (const std::out_of_range& oor) {
            sum++;
        }
    }
    ccHash[v] = sum;
    return sum;    
}


int NGramReader::countWords(const std::string& sentence) {
    int count = 0;
    stringstream ss(sentence);
    string word;
    while( ss >> word ) ++count;
    return count;
}


void NGramReader::processCorpus(vector<std::string>& words)
{
    std::vector<string>::iterator it;
    it = words.begin();
    words.insert(it, "<s>");
    words.push_back("</s>");

    std::string builder;
    for (size_t i = 0; i < words.size(); ++i) {
        cSize++;
        builder.clear();
        for (size_t j = i; j < min((i + 3), words.size()); ++j) {
            if (builder.empty()) {
                builder = words[j];
            } else {
                builder+= " "+ words[j];
            }
            (*frequencies)[builder] += 1;
        }
    }
}

void NGramReader::processVocab( const std::string& vocabFile)
{
    fs.open(vocabFile);
    if(fs.fail()) {
        throw "Error reading the vocab file";
    }

    vocabFrequencies = new NGramsMap();

    while (!fs.eof()) {
        getline(fs,sentence);
        (*vocabFrequencies)[sentence] = 0;
    }
    fs.close();

}

void NGramReader::writeNewCorpus(const std::vector<std::string>& words)
{
    const char* const delim = " ";

    std::ostringstream imploded;
    std::copy(words.begin(), words.end(),
              std::ostream_iterator<std::string>(imploded, delim));
    ofs <<imploded.str();
    ofs <<"\n";
}


NGramReader::~NGramReader()
{
    if (vocabFrequencies) {
        vocabFrequencies->clear();
    }
    frequencies->clear();
    fs.close();
}
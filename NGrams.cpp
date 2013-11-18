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
    recompfrequencies = new NGramsMap();

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
    recomputefreq();
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

//Recomputing Function
int NGramReader::countWords(const std::string& sentence) {
    int count = 0;
    std::stringstream ss(sentence);
    std::string word;
    while( ss >> word ) ++count;
    return count;
}
const NGramsMap* NGramReader::recomputedMap()
{
    return recompfrequencies;
}
void NGramReader::recomputefreq(void)
{
    sentence = "";
    for (auto i = frequencies->begin(); i != frequencies->end(); ++i) {
        sentence = i->first.c_str();
        if ((countWords(sentence)==3))
        {
                std::stringstream  stream(sentence);
                string word,word1;
                stream >> word; //discard the first word in trigram
                stream >> word;
                stream >> word1;
                word = word + " " + word1;
                (*recompfrequencies)[word]+=i->second;
                (*recompfrequencies)[word1]+=i->second;
        }    
        sentence = "";
    }

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
            (*recompfrequencies)[builder] +=1;  //Trigrams for recompute process
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

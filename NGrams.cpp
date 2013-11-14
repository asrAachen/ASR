#include <stdio.h>
#include "Ngrams.h"
#include <sstream>
#include <stdexcept>
#include <iterator>

using namespace std;


NGramReader::NGramReader(const std::string& corpusFileName, NGramType type,
                         const std::string& vocabFileName)
{
    frequencies = new NGramsMap();
    nType = type;

    if (!vocabFileName.empty()) {
        processVocab(vocabFileName);
        ofs.open("corpus_vocab");
    }

    fs.open(corpusFileName);
    if(fs.fail()) {
        throw "Error reading the corpus";
    }

    sentence = "";
    while (!fs.eof()) {
        getline(fs,sentence);
        std::transform(sentence.begin(), sentence.end(),sentence.begin(), ::tolower);
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

const NGramsMap* NGramReader::vocabMap()
{
    return vocabFrequencies;
}

size_t NGramReader::frequencyForKey(const std::string& key)
{
    return (*frequencies)[key];
}


void NGramReader::processCorpus(vector<std::string>& words)
{
    std::vector<string>::iterator it;
    it = words.begin();
    words.insert(it, "<s>");
    words.push_back("</s>");
    
    std::string builder;
    for (size_t i = 0; i < words.size() - nType + 1; ++i) {
        builder.clear();
        for (size_t j = i; j < (i + nType); ++j) {
            if (words[j] == "<unk>") {
                builder.clear();
                break;
            }
            if (builder.empty()) {
                builder = words[j];
            } else {
                builder+= " "+ words[j];
            }
        }
        if(!builder.empty()) {
           (*frequencies)[builder] += 1; 
        }
    }
}

void NGramReader::processVocab( const std::string& vocabFile)
{
    vocabFrequencies = new NGramsMap();
    fs.open(vocabFile);
    if(fs.fail()) {
        throw "Error reading the vocab file";
    }
    while (!fs.eof()) {
        getline(fs,sentence);
        std::transform(sentence.begin(), sentence.end(),sentence.begin(), ::tolower);
        (*vocabFrequencies)[sentence] = 1;
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
    frequencies->clear();
    if (vocabFrequencies) {
        vocabFrequencies->clear();
    }
    fs.close();
}
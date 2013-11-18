#ifndef NGRAMS_H
#define NGRAMS_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>


typedef std::unordered_map<std::string, int> NGramsMap;

enum NGramType {
    EUnigram = 1,
    EBigram,
    ETrigram
};

class NGramReader {
public:
    NGramReader(const std::string& corpusFileName,
                const std::string& vocabFileName = std::string());
    const NGramsMap* frequencyMap();
    const NGramsMap* vocab();
    const NGramsMap* recomputedMap(); //Map for recompute
    size_t corpusSize();
    ~NGramReader();

private:
    void processCorpus(std::vector<std::string>& words);
    void processVocab( const std::string& vocabFile);
    void writeNewCorpus(const std::vector<std::string>& words);
    void recomputefreq(void); // recomputing function
    int countWords(const std::string& sentence);


private:
    NGramsMap* frequencies;
    NGramsMap* vocabFrequencies;
    NGramsMap* recompfrequencies;  //for recomputing purpose
    std::ifstream fs;
    std::ofstream ofs;
    std::vector<std::string> words;
    size_t cSize;
    std::string sentence;
};

#endif
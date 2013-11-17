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
    size_t count(const std::string& key);
    size_t corpusSize();
    size_t countOfCount(NGramType type, int count);
    size_t countOfCount(NGramType type,const std::string& v, int count);  
    ~NGramReader();

private:
    void processCorpus(std::vector<std::string>& words);
    void processVocab( const std::string& vocabFile);
    void writeNewCorpus(const std::vector<std::string>& words);
    int countWords(const std::string& sentence);


private:
    NGramsMap* frequencies;
    NGramsMap* vocabFrequencies;
    std::ifstream fs;
    std::ofstream ofs;
    std::vector<std::string> words;
    size_t cSize;
    std::string sentence;
};


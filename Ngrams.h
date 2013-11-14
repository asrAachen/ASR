#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>

typedef std::unordered_map<std::string, int> NGramsMap;

enum NGramType {

    EUnigram = 1,
    EBigram,
    ETrigram
};

class NGramReader {
public:
    NGramReader(const std::string& corpusFileName, NGramType type = ETrigram,
                const std::string& vocabFileName = std::string());
    const NGramsMap* frequencyMap();
    const NGramsMap* vocabMap();
    size_t frequencyForKey(const std::string& key);
    ~NGramReader();

private:
    void processCorpus(std::vector<std::string>& words);
    void processVocab( const std::string& vocabFile);
    void writeNewCorpus(const std::vector<std::string>& words);


private:
    NGramsMap* frequencies;
    NGramsMap* vocabFrequencies;
    NGramType nType;
    std::ifstream fs;
    std::ofstream ofs;
    std::vector<std::string> words;
    std::string sentence;
};


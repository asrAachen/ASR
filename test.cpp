#include "Ngrams.h"

int main() {
    NGramReader* ngr = new NGramReader("../corpus", NGramType::ETrigram);
    const NGramsMap* freqs = ngr->frequencyMap();

    FILE* fOut = fopen("frequencies.txt", "w");
    for (auto i = freqs->begin(); i != freqs->end(); ++i) {
        fprintf(fOut, "%d\t%s\n", i->second, i->first.c_str());
    }
    fclose(fOut);

    delete ngr;
    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <unordered_set>
#include <cctype>
#include <vector>
#include <algorithm>

void toUpper(std::string * s)
{
    std::transform(s->begin(), s->end(), s->begin(),
                   [](unsigned char c) {
                       return std::toupper(c);
                   });
}


typedef std::unordered_set<std::string> Dictionary;

// ================
// class WordPath
// ================

class WordPath {
private:
    std::vector<std::string> d_words;

public:
    WordPath()
        : d_words()
    {}

    explicit WordPath(const std::string& w)
        : d_words()
    {
        d_words.push_back(w);
    }

    ~WordPath()
    {}

    void add(const std::string& w)
    {
        d_words.push_back(w);
    }

    size_t length() const
    {
        return d_words.size();
    }

    const std::string& first() const
    {
        return d_words.front();
    }

    const std::string& last() const
    {
        return d_words.back();
    }

    bool terminatesAt(const std::string& w) const
    {
        return d_words.back() == w;
    }

    void pop()
    {
        d_words.pop_back();
    }

    void clear()
    {
        d_words.clear();
    }

    bool empty() const
    {
        return d_words.empty();
    }

    std::ostream& print(std::ostream& o) const
    {
        const size_t nwords = d_words.size();

        o << "[ ";

        for (size_t i = 0; i < nwords; ++i) {
            o << d_words[i];
            if (i != nwords - 1) {
                o << " -> ";
            }
        }

        o << " ]";

        return o;
    }

}; // classs Path

std::ostream& operator<<(std::ostream& o, const WordPath& wp)
{
    return wp.print(o);
}

bool loadDictionary(Dictionary& d, const std::string& path)
{
    std::ifstream infile(path.c_str());

    if (!infile) {
        std::cerr << "Failed to read dictionary file.\n";
        return false;
    }

    std::string word;
    while (std::getline(infile, word)) {
        d.insert(word);
    }

    // std::cout << "Loaded dictionary with " << d.size() << " words.\n";

    return true;
}

void getAdjacentWords(std::vector<std::string>& adjacents,
                      const std::string& word,
                      const Dictionary& dict)
{
    const size_t wlen = word.size();

    for (size_t i = 0; i < wlen; ++i) {
        std::string w(word);
        char skipLetter = w[i];
        for (char c = 'A'; c <= 'Z'; ++c) {
            w[i] = c;
            if (dict.count(w) && c != skipLetter) {
                adjacents.push_back(w);
            }
        }
    }
}

void explore(const Dictionary& dict,
             const std::string& beginWord)
{
    std::deque<WordPath>            toVisit;
    std::unordered_set<std::string> visited;

    toVisit.push_back(WordPath(beginWord));
    visited.insert(beginWord);

    WordPath longestWordPath;

    while (!toVisit.empty()) {
        WordPath& curPath = toVisit.front();

        if (curPath.length() > longestWordPath.length()) {
            longestWordPath = curPath;
        }

        const std::string& curWord = curPath.last();

        std::vector<std::string> neighbors;
        getAdjacentWords(neighbors, curWord, dict);

        auto it = neighbors.begin();
        auto ite = neighbors.end();

        for (; it != ite; ++it) {
            const std::string& word = *it;

            if (visited.count(word)) {
                continue;
            }

            curPath.add(word);
            toVisit.push_back(curPath);
            visited.insert(word);
            curPath.pop();
        }

        toVisit.pop_front();
    }

    std::cout << "Connected component of " << beginWord
              << " has " << visited.size() << " elements.\n"
              << "A longest path starting from "
              << beginWord << ": " << longestWordPath << " is of length "
              << longestWordPath.length() << "\n";
}

void findWordLadder(const Dictionary& dict,
                    const std::string& beginWord,
                    const std::string& endWord)
{
    std::deque<WordPath>            toVisit;
    std::unordered_set<std::string> visited;

    toVisit.push_back(WordPath(beginWord));
    visited.insert(beginWord);

    while (!toVisit.empty()) {
        WordPath& curPath = toVisit.front();
        const std::string& curWord = curPath.last();

        if (curPath.terminatesAt(endWord)) {
            std::cout << curPath << "\n";
            return;
        }

        std::vector<std::string> neighbors;
        getAdjacentWords(neighbors, curWord, dict);

        const size_t numNeighbors = neighbors.size();
        for (size_t i = 0; i < numNeighbors; ++i) {
            const std::string& word = neighbors[i];

            if (visited.count(word)) {
                continue;
            }

            curPath.add(word);
            toVisit.push_back(curPath);
            visited.insert(word);
            curPath.pop();
        }

        toVisit.pop_front();
    }

    std::cout << "No word ladder found.\n";
}

int ladder(int argc, char * argv[])
{
    if (argc != 4) {
        std::cerr << "Invalid number of arguments.\n";
        return 1;
    }

    const char * dictPath = argv[1];
    std::string beginWord(argv[2]);
    std::string endWord(argv[3]);

    toUpper(&beginWord);
    toUpper(&endWord);

    Dictionary dict;
    if (!loadDictionary(dict, dictPath)) {
        return 2;
    }

    if (!dict.count(beginWord)) {
        std::cout << "Word " << beginWord << " is not in dictionary.\n";
        return 3;
    }

    if (!dict.count(endWord)) {
        std::cout << "Word " << endWord << " is not in dictionary.\n";
        return 3;
    }

    if (beginWord.size() != endWord.size()) {
        std::cout << "Word lengths must equal.\n";
        return 4;
    }

    findWordLadder(dict, beginWord, endWord);

    return 0;
}

int explore(int argc, char * argv[])
{
    if (argc != 3) {
        std::cerr << "Invalid number of arguments.\n";
        return 1;
    }

    const char * dictPath = argv[1];
    std::string beginWord(argv[2]);

    toUpper(&beginWord);

    Dictionary dict;
    if (!loadDictionary(dict, dictPath)) {
        return 2;
    }

    if (!dict.count(beginWord)) {
        std::cout << "Word " << beginWord << " is not in dictionary.\n";
        return 3;
    }

    explore(dict, beginWord);

    return 0;

}

int main(int argc, char * argv[])
{
    return argc == 4
        ? ladder(argc, argv)
        : explore(argc, argv);
}

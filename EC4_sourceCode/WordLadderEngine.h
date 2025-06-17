#ifndef WORD_LADDER_ENGINE_H
#define WORD_LADDER_ENGINE_H

#include "Graph.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <random>
#include <algorithm>

class WordLadderEngine {
private:
    Graph<std::string> wordGraph;
    std::vector<std::string> dictionary;
    std::map<int, std::vector<std::string>> wordsByLength;


    bool differsByOneLetter(const std::string& word1, const std::string& word2) const {
        if (word1.length() != word2.length()) return false;

        int differences = 0;
        for (size_t i = 0; i < word1.length(); ++i) {
            if (word1[i] != word2[i]) {
                differences++;
                if (differences > 1) return false;
            }
        }
        return differences == 1;
    }


    std::vector<std::string> generatePatterns(const std::string& word) const {
        std::vector<std::string> patterns;
        for (size_t i = 0; i < word.length(); ++i) {
            std::string pattern = word;
            pattern[i] = '*';
            patterns.push_back(pattern);
        }
        return patterns;
    }

public:
    bool loadDictionary(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            createSampleDictionary();
            return true;
        }

        dictionary.clear();
        wordsByLength.clear();

        std::string word;
        while (std::getline(file, word)) {
            std::transform(word.begin(), word.end(), word.begin(), ::toupper);
            word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());

            if (!word.empty()) {
                dictionary.push_back(word);
                wordsByLength[word.length()].push_back(word);
            }
        }

        file.close();
        return !dictionary.empty();
    }

    void createSampleDictionary() {
        dictionary = {
            "LAME", "NAME", "SOME", "SASE", "SAME", "SANE", "CAME", "SAGE", "SALE",
            "COST", "CASE", "CAST", "CARE", "CARD", "COLD", "BOLD", "BALD", "BALL",
            "CALL", "TALL", "TALK", "WALK", "WALL", "WILL", "WELL", "BELL", "TELL",
            "HELL", "HELP", "HELD", "HEAD", "HEAT", "BEAT", "BEST", "TEST", "REST",
            "WEST", "WENT", "WANT", "WAIT", "WAIL", "WALL", "FALL", "FAIL", "FAIR",
            "HAIR", "HARD", "HAND", "LAND", "SAND", "SAID", "PAID", "PAIN", "RAIN",
            "MAIN", "MAID", "MADE", "MAKE", "TAKE", "TALE", "TAPE", "TYPE", "TIRE",
            "FIRE", "FINE", "FIND", "WIND", "WILD", "MILD", "MILE", "PILE", "TILE"
        };

        wordsByLength.clear();
        for (const auto& word : dictionary) {
            wordsByLength[word.length()].push_back(word);
        }
    }


    void buildGraphForLength(int wordLength) {
        wordGraph.clear();

        if (wordsByLength.find(wordLength) == wordsByLength.end()) {
            return;
        }

        const auto& words = wordsByLength[wordLength];


        std::map<std::string, std::vector<std::string>> patternToWords;


        for (const std::string& word : words) {
            std::vector<std::string> patterns = generatePatterns(word);
            for (const std::string& pattern : patterns) {
                patternToWords[pattern].push_back(word);
            }
        }


        for (const auto& pair : patternToWords) {
            const std::vector<std::string>& wordsInPattern = pair.second;

            for (size_t i = 0; i < wordsInPattern.size(); ++i) {
                for (size_t j = i + 1; j < wordsInPattern.size(); ++j) {
                    wordGraph.addEdge(wordsInPattern[i], wordsInPattern[j]);
                }
            }
        }
    }


    std::vector<std::string> findWordLadder(const std::string& start, const std::string& target) {
        if (start.length() != target.length()) {
            return std::vector<std::string>();
        }

        buildGraphForLength(start.length());
        return wordGraph.findShortestPath(start, target);
    }


    std::string getRandomWord(int length) {
        if (wordsByLength.find(length) == wordsByLength.end() || wordsByLength[length].empty()) {
            return "";
        }

        const auto& words = wordsByLength[length];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, words.size() - 1);

        return words[dis(gen)];
    }

    std::vector<int> getAvailableLengths() {
        std::vector<int> lengths;
        for (const auto& pair : wordsByLength) {
            if (!pair.second.empty()) {
                lengths.push_back(pair.first);
            }
        }
        std::sort(lengths.begin(), lengths.end());
        return lengths;
    }

    bool isValidWord(const std::string& word) {
        return std::find(dictionary.begin(), dictionary.end(), word) != dictionary.end();
    }

    std::vector<std::string> getNextPossibleWords(const std::string& word) {
        if (!wordGraph.hasNode(word)) {
            buildGraphForLength(word.length());
        }
        return wordGraph.getNeighbors(word);
    }

    std::string getHint(const std::string& current, const std::string& target) {
        std::vector<std::string> path = findWordLadder(current, target);
        if (path.size() > 1) {
            return path[1];
        }
        return "";
    }

    int getHintPosition(const std::string& current, const std::string& hint) {
        for (size_t i = 0; i < current.length() && i < hint.length(); ++i) {
            if (current[i] != hint[i]) {
                return i;
            }
        }
        return -1;
    }
};

#endif

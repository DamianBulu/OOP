#ifndef GAME_STATS_H
#define GAME_STATS_H

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

struct GameRecord {
    std::string startWord;
    std::string targetWord;
    std::string dateTime;
    int hintsUsed;
    std::vector<std::string> wordsUsed;
    int actualMoves;
    int optimalMoves;

    GameRecord() : hintsUsed(0), actualMoves(0), optimalMoves(0) {}
};

class GameStats {
private:
    std::string username;
    std::vector<GameRecord> gameHistory;


    std::string getCurrentDateTime() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::string vectorToString(const std::vector<std::string>& vec) {
        std::ostringstream oss;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ";";
            oss << vec[i];
        }
        return oss.str();
    }

    std::vector<std::string> stringToVector(const std::string& str) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;

        while (std::getline(ss, item, ';')) {
            result.push_back(item);
        }
        return result;
    }

public:
    void setUsername(const std::string& name) {
        username = name;
    }

    std::string getUsername() const {
        return username;
    }

    void saveGameRecord(const GameRecord& record) {
        gameHistory.push_back(record);

        std::string filename = username + ".csv";
        std::ofstream file(filename, std::ios::app);

        if (file.is_open()) {

            file.seekp(0, std::ios::end);
            if (file.tellp() == 0) {
                file << "StartWord,TargetWord,DateTime,HintsUsed,WordsUsed,ActualMoves,OptimalMoves\n";
            }

            file << record.startWord << ","
                 << record.targetWord << ","
                 << record.dateTime << ","
                 << record.hintsUsed << ","
                 << vectorToString(record.wordsUsed) << ","
                 << record.actualMoves << ","
                 << record.optimalMoves << "\n";

            file.close();
        }
    }


    bool loadGameHistory() {
        gameHistory.clear();

        std::string filename = username + ".csv";
        std::ifstream file(filename);

        if (!file.is_open()) {
            return false;
        }

        std::string line;
        bool firstLine = true;

        while (std::getline(file, line)) {
            if (firstLine) {
                firstLine = false;
                continue;
            }

            std::stringstream ss(line);
            std::string cell;
            GameRecord record;

            if (std::getline(ss, cell, ',')) record.startWord = cell;
            if (std::getline(ss, cell, ',')) record.targetWord = cell;
            if (std::getline(ss, cell, ',')) record.dateTime = cell;
            if (std::getline(ss, cell, ',')) record.hintsUsed = std::stoi(cell);
            if (std::getline(ss, cell, ',')) record.wordsUsed = stringToVector(cell);
            if (std::getline(ss, cell, ',')) record.actualMoves = std::stoi(cell);
            if (std::getline(ss, cell, ',')) record.optimalMoves = std::stoi(cell);

            gameHistory.push_back(record);
        }

        file.close();
        return true;
    }


    GameRecord createGameRecord(const std::string& start, const std::string& target,
                                int hints, const std::vector<std::string>& words,
                                int actual, int optimal) {
        GameRecord record;
        record.startWord = start;
        record.targetWord = target;
        record.dateTime = getCurrentDateTime();
        record.hintsUsed = hints;
        record.wordsUsed = words;
        record.actualMoves = actual;
        record.optimalMoves = optimal;

        return record;
    }

    // Get statistics
    int getTotalGames() const {
        return gameHistory.size();
    }

    int getTotalHintsUsed() const {
        int total = 0;
        for (const auto& record : gameHistory) {
            total += record.hintsUsed;
        }
        return total;
    }

    int getUniqueWordsCount() const {
        std::set<std::string> uniqueWords;
        for (const auto& record : gameHistory) {
            for (const auto& word : record.wordsUsed) {
                uniqueWords.insert(word);
            }
        }
        return uniqueWords.size();
    }

    double getAverageMovesPerGame() const {
        if (gameHistory.empty()) return 0.0;

        int totalMoves = 0;
        for (const auto& record : gameHistory) {
            totalMoves += record.actualMoves;
        }
        return static_cast<double>(totalMoves) / gameHistory.size();
    }

    double getAverageOptimalMoves() const {
        if (gameHistory.empty()) return 0.0;

        int totalOptimal = 0;
        for (const auto& record : gameHistory) {
            totalOptimal += record.optimalMoves;
        }
        return static_cast<double>(totalOptimal) / gameHistory.size();
    }

    double getEfficiencyScore() const {
        if (gameHistory.empty()) return 0.0;

        int totalOptimal = 0;
        int totalActual = 0;

        for (const auto& record : gameHistory) {
            totalOptimal += record.optimalMoves;
            totalActual += record.actualMoves;
        }

        if (totalActual == 0) return 100.0;
        return (static_cast<double>(totalOptimal) / totalActual) * 100.0;
    }

    std::vector<GameRecord> getGameHistory() const {
        return gameHistory;
    }

    GameRecord getBestGame() const {
        if (gameHistory.empty()) return GameRecord();

        GameRecord best = gameHistory[0];
        double bestEfficiency = 0.0;

        for (const auto& record : gameHistory) {
            if (record.optimalMoves > 0) {
                double efficiency = static_cast<double>(record.optimalMoves) / record.actualMoves;
                if (efficiency > bestEfficiency) {
                    bestEfficiency = efficiency;
                    best = record;
                }
            }
        }

        return best;
    }
};

#endif // GAME_STATS_H

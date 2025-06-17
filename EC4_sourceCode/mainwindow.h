#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QScrollArea>
#include <QtCore/QTimer>

#include "WordLadderEngine.h"
#include "GameStats.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:

    WordLadderEngine engine;
    GameStats stats;

    QStackedWidget* stackedWidget;

    QWidget* mainMenuWidget;
    QPushButton* automaticModeBtn;
    QPushButton* playingModeBtn;
    QPushButton* analyticsBtn;
    QPushButton* exitBtn;

    // Automatic Mode
    QWidget* automaticWidget;
    QLineEdit* startWordEdit;
    QLineEdit* targetWordEdit;
    QPushButton* solveBtn;
    QPushButton* backToMenuBtn1;
    QTextEdit* solutionDisplay;

    // Playing Mode
    QWidget* playingWidget;
    QLabel* currentWordLabel;
    QLineEdit* nextWordEdit;
    QPushButton* submitWordBtn;
    QPushButton* hintBtn;
    QPushButton* backToMenuBtn2;
    QTextEdit* gameLog;
    QLabel* targetWordLabel;
    QLabel* movesLabel;
    QLabel* hintsLabel;
    QProgressBar* progressBar;

    // Analytics
    QWidget* analyticsWidget;
    QLineEdit* usernameEdit;
    QPushButton* loadStatsBtn;
    QPushButton* backToMenuBtn3;
    QTextEdit* statsDisplay;

    // Game state
    std::string currentWord;
    std::string targetWord;
    std::vector<std::string> wordsUsed;
    int moveCount;
    int hintCount;
    bool gameInProgress;

    void setupMainMenu();
    void setupAutomaticMode();
    void setupPlayingMode();
    void setupAnalytics();
    void styleButton(QPushButton* button, const QString& color = "#4CAF50");
    void showMainMenu();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showAutomaticMode();
    void showPlayingMode();
    void showAnalytics();
    void solveWordLadder();
    void startNewGame();
    void submitWord();
    void giveHint();
    void loadUserStats();
    void exitApplication();
};

#endif

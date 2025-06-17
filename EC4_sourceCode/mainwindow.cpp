#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QtCore/QDebug>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), gameInProgress(false), moveCount(0), hintCount(0)
{
    engine.loadDictionary("dictionary.txt");

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    stackedWidget = new QStackedWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(stackedWidget);

    setupMainMenu();
    setupAutomaticMode();
    setupPlayingMode();
    setupAnalytics();

    setWindowTitle("Word Ladder Game");
    setMinimumSize(800, 600);
    resize(1000, 700);

    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QWidget {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QLineEdit {
            background-color: #404040;
            border: 2px solid #555555;
            border-radius: 5px;
            padding: 8px;
            font-size: 14px;
            color: #ffffff;
        }
        QLineEdit:focus {
            border-color: #4CAF50;
        }
        QTextEdit {
            background-color: #404040;
            border: 2px solid #555555;
            border-radius: 5px;
            padding: 8px;
            font-size: 12px;
            color: #ffffff;
        }
        QLabel {
            font-size: 14px;
            color: #ffffff;
        }
        QComboBox {
            background-color: #404040;
            border: 2px solid #555555;
            border-radius: 5px;
            padding: 5px;
            color: #ffffff;
        }
        QProgressBar {
            border: 2px solid #555555;
            border-radius: 5px;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #4CAF50;
            border-radius: 3px;
        }
    )");

    showMainMenu();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupMainMenu() {
    mainMenuWidget = new QWidget();

    QVBoxLayout* layout = new QVBoxLayout(mainMenuWidget);
    layout->setSpacing(20);
    layout->setContentsMargins(50, 50, 50, 50);


    QLabel* titleLabel = new QLabel("WORD LADDER GAME");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #4CAF50; margin-bottom: 30px;");
    layout->addWidget(titleLabel);


    QLabel* descLabel = new QLabel("Transform one word into another by changing one letter at a time!");
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet("font-size: 16px; color: #cccccc; margin-bottom: 20px;");
    layout->addWidget(descLabel);


    automaticModeBtn = new QPushButton("Automatic Mode");
    automaticModeBtn->setMinimumHeight(60);
    styleButton(automaticModeBtn, "#4CAF50");
    connect(automaticModeBtn, &QPushButton::clicked, this, &MainWindow::showAutomaticMode);
    layout->addWidget(automaticModeBtn);

    playingModeBtn = new QPushButton("Playing Mode");
    playingModeBtn->setMinimumHeight(60);
    styleButton(playingModeBtn, "#2196F3");
    connect(playingModeBtn, &QPushButton::clicked, this, &MainWindow::showPlayingMode);
    layout->addWidget(playingModeBtn);

    analyticsBtn = new QPushButton("Analytics");
    analyticsBtn->setMinimumHeight(60);
    styleButton(analyticsBtn, "#FF9800");
    connect(analyticsBtn, &QPushButton::clicked, this, &MainWindow::showAnalytics);
    layout->addWidget(analyticsBtn);

    exitBtn = new QPushButton("Exit");
    exitBtn->setMinimumHeight(60);
    styleButton(exitBtn, "#f44336");
    connect(exitBtn, &QPushButton::clicked, this, &MainWindow::exitApplication);
    layout->addWidget(exitBtn);

    layout->addStretch();

    stackedWidget->addWidget(mainMenuWidget);
}

void MainWindow::setupAutomaticMode() {
    automaticWidget = new QWidget();

    QVBoxLayout* layout = new QVBoxLayout(automaticWidget);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);


    QLabel* titleLabel = new QLabel("Automatic Mode");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #4CAF50;");
    layout->addWidget(titleLabel);


    QGroupBox* inputGroup = new QGroupBox("Input Words");
    inputGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #4CAF50; }");
    QGridLayout* inputLayout = new QGridLayout(inputGroup);

    inputLayout->addWidget(new QLabel("Start Word:"), 0, 0);
    startWordEdit = new QLineEdit();
    startWordEdit->setPlaceholderText("Enter starting word (e.g., SAME)");
    inputLayout->addWidget(startWordEdit, 0, 1);

    inputLayout->addWidget(new QLabel("Target Word:"), 1, 0);
    targetWordEdit = new QLineEdit();
    targetWordEdit->setPlaceholderText("Enter target word (e.g., COST)");
    inputLayout->addWidget(targetWordEdit, 1, 1);

    layout->addWidget(inputGroup);


    QHBoxLayout* buttonLayout = new QHBoxLayout();

    solveBtn = new QPushButton("Solve Word Ladder");
    solveBtn->setMinimumHeight(50);
    styleButton(solveBtn, "#4CAF50");
    connect(solveBtn, &QPushButton::clicked, this, &MainWindow::solveWordLadder);
    buttonLayout->addWidget(solveBtn);

    backToMenuBtn1 = new QPushButton("Back to Menu");
    backToMenuBtn1->setMinimumHeight(50);
    styleButton(backToMenuBtn1, "#757575");
    connect(backToMenuBtn1, &QPushButton::clicked, this, &MainWindow::showMainMenu);
    buttonLayout->addWidget(backToMenuBtn1);

    layout->addLayout(buttonLayout);


    QLabel* solutionLabel = new QLabel("Solution:");
    solutionLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");
    layout->addWidget(solutionLabel);

    solutionDisplay = new QTextEdit();
    solutionDisplay->setReadOnly(true);
    solutionDisplay->setMinimumHeight(200);
    layout->addWidget(solutionDisplay);

    stackedWidget->addWidget(automaticWidget);
}

void MainWindow::setupPlayingMode() {
    playingWidget = new QWidget();

    QVBoxLayout* layout = new QVBoxLayout(playingWidget);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);


    QLabel* titleLabel = new QLabel("Playing Mode");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2196F3;");
    layout->addWidget(titleLabel);

    QGroupBox* infoGroup = new QGroupBox("Game Information");
    infoGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2196F3; }");
    QGridLayout* infoLayout = new QGridLayout(infoGroup);

    infoLayout->addWidget(new QLabel("Current Word:"), 0, 0);
    currentWordLabel = new QLabel("Click 'New Game' to start");
    currentWordLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4CAF50;");
    infoLayout->addWidget(currentWordLabel, 0, 1);

    infoLayout->addWidget(new QLabel("Target Word:"), 1, 0);
    targetWordLabel = new QLabel("-");
    targetWordLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #f44336;");
    infoLayout->addWidget(targetWordLabel, 1, 1);

    infoLayout->addWidget(new QLabel("Moves:"), 2, 0);
    movesLabel = new QLabel("0");
    movesLabel->setStyleSheet("font-size: 16px; color: #FF9800;");
    infoLayout->addWidget(movesLabel, 2, 1);

    infoLayout->addWidget(new QLabel("Hints Used:"), 3, 0);
    hintsLabel = new QLabel("0");
    hintsLabel->setStyleSheet("font-size: 16px; color: #9C27B0;");
    infoLayout->addWidget(hintsLabel, 3, 1);

    layout->addWidget(infoGroup);

    progressBar = new QProgressBar();
    progressBar->setVisible(false);
    layout->addWidget(progressBar);

    QGroupBox* inputGroup = new QGroupBox("Your Move");
    inputGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2196F3; }");
    QHBoxLayout* inputLayout = new QHBoxLayout(inputGroup);

    nextWordEdit = new QLineEdit();
    nextWordEdit->setPlaceholderText("Enter next word...");
    nextWordEdit->setEnabled(false);
    inputLayout->addWidget(nextWordEdit);

    submitWordBtn = new QPushButton("Submit");
    submitWordBtn->setMinimumHeight(40);
    submitWordBtn->setEnabled(false);
    styleButton(submitWordBtn, "#4CAF50");
    connect(submitWordBtn, &QPushButton::clicked, this, &MainWindow::submitWord);
    inputLayout->addWidget(submitWordBtn);

    layout->addWidget(inputGroup);


    QHBoxLayout* controlLayout = new QHBoxLayout();

    QPushButton* newGameBtn = new QPushButton("New Game");
    newGameBtn->setMinimumHeight(50);
    styleButton(newGameBtn, "#4CAF50");
    connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::startNewGame);
    controlLayout->addWidget(newGameBtn);

    hintBtn = new QPushButton("Get Hint");
    hintBtn->setMinimumHeight(50);
    hintBtn->setEnabled(false);
    styleButton(hintBtn, "#FF9800");
    connect(hintBtn, &QPushButton::clicked, this, &MainWindow::giveHint);
    controlLayout->addWidget(hintBtn);

    backToMenuBtn2 = new QPushButton("Back to Menu");
    backToMenuBtn2->setMinimumHeight(50);
    styleButton(backToMenuBtn2, "#757575");
    connect(backToMenuBtn2, &QPushButton::clicked, this, &MainWindow::showMainMenu);
    controlLayout->addWidget(backToMenuBtn2);

    layout->addLayout(controlLayout);


    QLabel* logLabel = new QLabel("Game Log:");
    logLabel->setStyleSheet("font-weight: bold; color: #2196F3;");
    layout->addWidget(logLabel);

    gameLog = new QTextEdit();
    gameLog->setReadOnly(true);
    gameLog->setMaximumHeight(150);
    layout->addWidget(gameLog);

    connect(nextWordEdit, &QLineEdit::returnPressed, this, &MainWindow::submitWord);

    stackedWidget->addWidget(playingWidget);
}

void MainWindow::setupAnalytics() {
    analyticsWidget = new QWidget();

    QVBoxLayout* layout = new QVBoxLayout(analyticsWidget);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);


    QLabel* titleLabel = new QLabel("Analytics");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FF9800;");
    layout->addWidget(titleLabel);


    QGroupBox* userGroup = new QGroupBox("User Selection");
    userGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #FF9800; }");
    QHBoxLayout* userLayout = new QHBoxLayout(userGroup);

    userLayout->addWidget(new QLabel("Username:"));
    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("Enter username to view statistics");
    userLayout->addWidget(usernameEdit);

    loadStatsBtn = new QPushButton("Load Statistics");
    loadStatsBtn->setMinimumHeight(40);
    styleButton(loadStatsBtn, "#FF9800");
    connect(loadStatsBtn, &QPushButton::clicked, this, &MainWindow::loadUserStats);
    userLayout->addWidget(loadStatsBtn);

    layout->addWidget(userGroup);


    backToMenuBtn3 = new QPushButton("Back to Menu");
    backToMenuBtn3->setMinimumHeight(50);
    styleButton(backToMenuBtn3, "#757575");
    connect(backToMenuBtn3, &QPushButton::clicked, this, &MainWindow::showMainMenu);

    QHBoxLayout* backLayout = new QHBoxLayout();
    backLayout->addStretch();
    backLayout->addWidget(backToMenuBtn3);
    backLayout->addStretch();
    layout->addLayout(backLayout);


    QLabel* statsLabel = new QLabel("Statistics:");
    statsLabel->setStyleSheet("font-weight: bold; color: #FF9800;");
    layout->addWidget(statsLabel);

    statsDisplay = new QTextEdit();
    statsDisplay->setReadOnly(true);
    layout->addWidget(statsDisplay);

    connect(usernameEdit, &QLineEdit::returnPressed, this, &MainWindow::loadUserStats);

    stackedWidget->addWidget(analyticsWidget);
}

void MainWindow::styleButton(QPushButton* button, const QString& color) {
    button->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            font-weight: bold;
            color: white;
        }
        QPushButton:hover {
            background-color: %2;
            transform: translateY(-2px);
        }
        QPushButton:pressed {
            background-color: %3;
        }
        QPushButton:disabled {
            background-color: #555555;
            color: #888888;
        }
    )").arg(color,
                                   color == "#4CAF50" ? "#45a049" :
                                       color == "#2196F3" ? "#1976D2" :
                                       color == "#FF9800" ? "#F57C00" :
                                       color == "#f44336" ? "#d32f2f" : "#616161",
                                   color == "#4CAF50" ? "#3d8b40" :
                                       color == "#2196F3" ? "#1565C0" :
                                       color == "#FF9800" ? "#E65100" :
                                       color == "#f44336" ? "#c62828" : "#424242"));
}

void MainWindow::showMainMenu() {
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::showAutomaticMode() {
    stackedWidget->setCurrentWidget(automaticWidget);
}

void MainWindow::showPlayingMode() {
    stackedWidget->setCurrentWidget(playingWidget);
}

void MainWindow::showAnalytics() {
    stackedWidget->setCurrentWidget(analyticsWidget);
}

void MainWindow::solveWordLadder() {
    QString startText = startWordEdit->text().toUpper().trimmed();
    QString targetText = targetWordEdit->text().toUpper().trimmed();

    if (startText.isEmpty() || targetText.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter both start and target words.");
        return;
    }

    if (startText.length() != targetText.length()) {
        QMessageBox::warning(this, "Warning", "Start and target words must have the same length.");
        return;
    }

    std::string startWord = startText.toStdString();
    std::string targetWord = targetText.toStdString();

    if (!engine.isValidWord(startWord)) {
        QMessageBox::warning(this, "Warning", QString("Start word '%1' is not in the dictionary.").arg(startText));
        return;
    }

    if (!engine.isValidWord(targetWord)) {
        QMessageBox::warning(this, "Warning", QString("Target word '%1' is not in the dictionary.").arg(targetText));
        return;
    }

    solutionDisplay->clear();
    solutionDisplay->append("Solving...");
    QApplication::processEvents();

    std::vector<std::string> solution = engine.findWordLadder(startWord, targetWord);

    solutionDisplay->clear();

    if (solution.empty()) {
        solutionDisplay->append("No solution found! The words cannot be connected.");
    } else {
        solutionDisplay->append(QString("Solution found in %1 steps:\n").arg(solution.size() - 1));

        for (size_t i = 0; i < solution.size(); ++i) {
            QString step = QString("%1. %2").arg(i + 1).arg(QString::fromStdString(solution[i]));
            if (i > 0) {
                // Highlight the changed character
                std::string prev = solution[i-1];
                std::string curr = solution[i];
                for (size_t j = 0; j < prev.length() && j < curr.length(); ++j) {
                    if (prev[j] != curr[j]) {
                        step += QString(" (changed position %1: %2 → %3)")
                                    .arg(j + 1)
                                    .arg(prev[j])
                                    .arg(curr[j]);
                        break;
                    }
                }
            }
            solutionDisplay->append(step);
        }

        solutionDisplay->append(QString("\nOptimal solution uses %1 moves.").arg(solution.size() - 1));
    }
}

void MainWindow::startNewGame() {

    bool ok;
    QString username = QInputDialog::getText(this, "Player Name",
                                             "Enter your name:", QLineEdit::Normal, "", &ok);
    if (!ok || username.trimmed().isEmpty()) {
        return;
    }

    stats.setUsername(username.trimmed().toStdString());


    QStringList lengthOptions;
    std::vector<int> availableLengths = engine.getAvailableLengths();
    for (int length : availableLengths) {
        lengthOptions << QString::number(length);
    }

    QString lengthStr = QInputDialog::getItem(this, "Word Length",
                                              "Select word length:", lengthOptions, 0, false, &ok);
    if (!ok) {
        return;
    }

    int wordLength = lengthStr.toInt();


    std::string start = engine.getRandomWord(wordLength);
    std::string target = engine.getRandomWord(wordLength);

    int attempts = 0;
    while ((start == target || engine.findWordLadder(start, target).empty()) && attempts < 10) {
        target = engine.getRandomWord(wordLength);
        attempts++;
    }

    if (start == target || engine.findWordLadder(start, target).empty()) {
        QMessageBox::warning(this, "Error", "Could not find suitable word pair. Please try again.");
        return;
    }

    currentWord = start;
    this->targetWord = target;
    wordsUsed.clear();
    wordsUsed.push_back(currentWord);
    moveCount = 0;
    hintCount = 0;
    gameInProgress = true;

    currentWordLabel->setText(QString::fromStdString(currentWord));
    targetWordLabel->setText(QString::fromStdString(this->targetWord));
    movesLabel->setText("0");
    hintsLabel->setText("0");

    nextWordEdit->setEnabled(true);
    submitWordBtn->setEnabled(true);
    hintBtn->setEnabled(true);
    nextWordEdit->clear();
    nextWordEdit->setFocus();

    gameLog->clear();
    gameLog->append(QString("New game started!"));
    gameLog->append(QString("Player: %1").arg(username));
    gameLog->append(QString("Transform '%1' into '%2'").arg(QString::fromStdString(start), QString::fromStdString(target)));
    gameLog->append(QString("Word length: %1 letters\n").arg(wordLength));
}

void MainWindow::submitWord() {
    if (!gameInProgress) {
        QMessageBox::information(this, "Info", "Please start a new game first.");
        return;
    }

    QString inputText = nextWordEdit->text().toUpper().trimmed();
    if (inputText.isEmpty()) {
        return;
    }

    std::string inputWord = inputText.toStdString();


    if (!engine.isValidWord(inputWord)) {
        QMessageBox::warning(this, "Invalid Word",
                             QString("'%1' is not in the dictionary.").arg(inputText));
        nextWordEdit->selectAll();
        return;
    }

    int differences = 0;
    if (inputWord.length() != currentWord.length()) {
        differences = 2; // Force failure
    } else {
        for (size_t i = 0; i < inputWord.length(); ++i) {
            if (inputWord[i] != currentWord[i]) {
                differences++;
            }
        }
    }

    if (differences != 1) {
        QMessageBox::warning(this, "Invalid Move",
                             "You must change exactly one letter from the current word.");
        nextWordEdit->selectAll();
        return;
    }


    moveCount++;
    currentWord = inputWord;
    wordsUsed.push_back(currentWord);


    currentWordLabel->setText(QString::fromStdString(currentWord));
    movesLabel->setText(QString::number(moveCount));
    gameLog->append(QString("Move %1: %2").arg(moveCount).arg(inputText));

    nextWordEdit->clear();

    // Check if game is won
    if (currentWord == targetWord) {
        gameInProgress = false;
        nextWordEdit->setEnabled(false);
        submitWordBtn->setEnabled(false);
        hintBtn->setEnabled(false);


        std::vector<std::string> optimalPath = engine.findWordLadder(wordsUsed[0], targetWord);
        int optimalMoves = optimalPath.size() - 1;

        gameLog->append(QString("\n Congratulations! You won!"));
        gameLog->append(QString("You used %1 moves (optimal: %2)").arg(moveCount).arg(optimalMoves));
        gameLog->append(QString("Hints used: %1").arg(hintCount));


        GameRecord record = stats.createGameRecord(wordsUsed[0], targetWord, hintCount,
                                                   wordsUsed, moveCount, optimalMoves);
        stats.saveGameRecord(record);

        QMessageBox::information(this, "Victory!",
                                 QString("Congratulations! You solved it in %1 moves!\n"
                                         "Optimal solution: %2 moves\n"
                                         "Hints used: %3\n\n"
                                         "Game statistics saved to %4.csv")
                                     .arg(moveCount).arg(optimalMoves).arg(hintCount)
                                     .arg(QString::fromStdString(stats.getUsername())));
    }
}

void MainWindow::giveHint() {
    if (!gameInProgress) {
        return;
    }

    std::string hint = engine.getHint(currentWord, targetWord);
    if (hint.empty()) {
        QMessageBox::information(this, "Hint", "No hint available. You might be at the target word!");
        return;
    }

    hintCount++;
    hintsLabel->setText(QString::number(hintCount));

    int hintPos = engine.getHintPosition(currentWord, hint);
    QString hintText = QString(" Hint %1: Try changing letter at position %2 (letter '%3')")
                           .arg(hintCount)
                           .arg(hintPos + 1)
                           .arg(currentWord[hintPos]);

    gameLog->append(hintText);

    // Show hint dialog with colored text
    QString hintWord = QString::fromStdString(currentWord);
    if (hintPos >= 0 && hintPos < hintWord.length()) {
        QString coloredHint = hintWord;
        coloredHint[hintPos] = QString("<span style='color: #FF9800; font-weight: bold;'>%1</span>")
                                   .arg(hintWord[hintPos])[0];
    }

    QMessageBox hintBox(this);
    hintBox.setWindowTitle("Hint");
    hintBox.setText(QString("Current word: %1\n\nTry changing the highlighted letter (position %2)")
                        .arg(QString::fromStdString(currentWord)).arg(hintPos + 1));
    hintBox.setIcon(QMessageBox::Information);
    hintBox.exec();
}

void MainWindow::loadUserStats() {
    QString username = usernameEdit->text().trimmed();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a username.");
        return;
    }

    stats.setUsername(username.toStdString());

    if (!stats.loadGameHistory()) {
        statsDisplay->clear();
        statsDisplay->append(QString("No statistics found for user '%1'.\n").arg(username));
        statsDisplay->append("This user hasn't played any games yet.");
        return;
    }

    // Display statistics
    statsDisplay->clear();
    statsDisplay->append(QString(" STATISTICS FOR USER: %1\n").arg(username));
    statsDisplay->append("=" + QString("=").repeated(50) + "\n");

    // Basic stats
    statsDisplay->append(QString(" Total Games Played: %1").arg(stats.getTotalGames()));
    statsDisplay->append(QString(" Total Hints Used: %1").arg(stats.getTotalHintsUsed()));
    statsDisplay->append(QString(" Unique Words Used: %1").arg(stats.getUniqueWordsCount()));
    statsDisplay->append(QString(" Average Moves per Game: %.2f").arg(stats.getAverageMovesPerGame()));
    statsDisplay->append(QString(" Average Optimal Moves: %.2f").arg(stats.getAverageOptimalMoves()));
    statsDisplay->append(QString(" Efficiency Score: %.1f%%\n").arg(stats.getEfficiencyScore()));

    // Best game
    GameRecord bestGame = stats.getBestGame();
    if (bestGame.actualMoves > 0) {
        statsDisplay->append(" BEST GAME:");
        statsDisplay->append(QString("   %1 → %2").arg(QString::fromStdString(bestGame.startWord),
                                                       QString::fromStdString(bestGame.targetWord)));
        statsDisplay->append(QString("   Moves: %1 (optimal: %2)").arg(bestGame.actualMoves).arg(bestGame.optimalMoves));
        statsDisplay->append(QString("   Date: %1").arg(QString::fromStdString(bestGame.dateTime)));
        statsDisplay->append(QString("   Hints: %1\n").arg(bestGame.hintsUsed));
    }

    // Recent games
    std::vector<GameRecord> history = stats.getGameHistory();
    int recentCount = std::min(5, static_cast<int>(history.size()));

    if (recentCount > 0) {
        statsDisplay->append(QString(" RECENT GAMES (last %1):").arg(recentCount));
        for (int i = history.size() - recentCount; i < static_cast<int>(history.size()); ++i) {
            const GameRecord& game = history[i];
            statsDisplay->append(QString("   %1. %2 → %3 (%4 moves, %5 hints) - %6")
                                     .arg(i + 1)
                                     .arg(QString::fromStdString(game.startWord))
                                     .arg(QString::fromStdString(game.targetWord))
                                     .arg(game.actualMoves)
                                     .arg(game.hintsUsed)
                                     .arg(QString::fromStdString(game.dateTime)));
        }
    }
}

void MainWindow::exitApplication() {
    QApplication::quit();
}

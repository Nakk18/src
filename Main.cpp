#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <thread>
#include <fstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

//Manage a Question
class Question {
private:
    string questionText;
    vector<string> options;
    int correctAnswerIndex;
    int difficultyLevel;
    int random_shuffle;

public:
    Question(string text, vector<string> opts, int correctIndex, int difficulty)
        : questionText(text), options(opts), correctAnswerIndex(correctIndex), difficultyLevel(difficulty) {}

    string getQuestionText() const { return questionText; }
    vector<string> getOptions() const { return options; }
    int getCorrectAnswerIndex() const { return correctAnswerIndex; }
    int getDifficultyLevel() const { return difficultyLevel; }
};

//Manage player data and scores
class Player {
private:
    string playerName;
    int score;

public:
    Player(string name) : playerName(name), score(0) {}

    string getPlayerName() const { return playerName; }
    int getScore() const { return score; }
    void addScore(int points) { score += points; }

    void display() const {
        cout << "Player: " << playerName << ", Score: " << score << endl;
    }
};

//High scores and saving
class HighScoreManager {
private:
    vector<Player> highScores;

public:
    void loadHighScores(const string& filename) {
        ifstream file(filename);
        if (file) {
            string name;
            int score;
            while (file >> name >> score) {
                highScores.push_back(Player(name));
                highScores.back().addScore(score);
            }
            file.close();
        }
    }

    void saveHighScores(const string& filename) const {
        ofstream file(filename);
        if (file) {
            for (const Player& p : highScores) {
                file << p.getPlayerName() << " " << p.getScore() << endl;
            }
            file.close();
        }
        else {
            throw runtime_error("Could not open high scores file.");
        }
    }

    void displayHighScores() const {
        cout << "\nHigh scores record:\n";
        cout << "-------------------\n";
        for (const Player& p : highScores) {
            cout << p.getPlayerName() << ": " << p.getScore() << endl;
        }
    }

    void addHighScore(const Player& player) {
        highScores.push_back(player);
        sort(highScores.begin(), highScores.end(), [](const Player& a, const Player& b) {
            return a.getScore() > b.getScore();
            });
        if (highScores.size() > 5) {
            highScores.pop_back();  //Keep only the top 5 scores
        }
    }
};

//Questions difficuty
class QuizGame {
private:
    vector<Question> questionBank;
    Player player;
    int timeLimit;

public:
    QuizGame(string playerName) : player(playerName), timeLimit(10) {}

    void addQuestion(const Question& q) {
        questionBank.push_back(q);
    }

    void shuffleQuestions() {
        random_device rd;
        mt19937 g(rd());
        shuffle(questionBank.begin(), questionBank.end(), g);
    }

    void startGame() {
        shuffleQuestions();
        for (int i = 0; i < questionBank.size(); ++i) {
            if (askQuestion(questionBank[i])) {
                cout << "Correct!\n";
                player.addScore(10);
            }
            else {
                cout << "Incorrect!\n";
            }
        }
        cout << "\nYour scores: " << player.getScore() << endl;
    }

    bool askQuestion(const Question& q) {
        cout << "\nQuestion: " << q.getQuestionText() << endl;
        vector<string> options = q.getOptions();
        for (int i = 0; i < options.size(); ++i) {
            cout << i + 1 << ". " << options[i] << endl;
        }

        int playerAnswer;
        cout << "Your answer (1-" << options.size() << "): ";

        //Start the timer
        auto start = chrono::steady_clock::now();
        cin >> playerAnswer;

        //Check if the player ran out of time
        auto end = chrono::steady_clock::now();
        auto elapsed_seconds = chrono::duration_cast<chrono::seconds>(end - start);

        if (elapsed_seconds.count() > 5) {
            cout << "Time's up! The correct answer was: " << options[q.getCorrectAnswerIndex()] << endl;
            return false;
        }

        //Invalid input
        if (playerAnswer < 1 || playerAnswer > options.size()) {
            throw invalid_argument("Invalid input! Please choose a valid option.");
        }

        return playerAnswer - 1 == q.getCorrectAnswerIndex();
    }

    Player getPlayer() const { return player; }
};

int main() {
    try {
        string playerName;
        cout << "Enter your name: ";
        cin >> playerName;

        QuizGame game(playerName);

        //Questions pool
        game.addQuestion(Question("Which province in Cambodia is the biggest?", { "Mondulkiri", "Ratanakiri", "Kandal", "Kep" }, 0, 1));
        game.addQuestion(Question("2 + 2 = ?", { "3", "4", "5", "6" }, 1, 1));
        game.addQuestion(Question("What is the largest planet in our solar system?", { "Earth", "Jupiter", "Mars", "Saturn" }, 1, 2));
        game.addQuestion(Question("How many minutes are in week?", { "10080", "3600", "10060", "8600" }, 0, 3));
        game.addQuestion(Question("What is the most popular pet in the world?", {"Fishs", "Cats", "Monkeys", "Dogs" }, 3, 1));

        HighScoreManager highScoreManager;
        highScoreManager.loadHighScores("highscores.txt");  //Previous high scores

        game.startGame();

        highScoreManager.addHighScore(game.getPlayer());  //New high scores
        highScoreManager.saveHighScores("highscores.txt");  //Save updated high scores

        highScoreManager.displayHighScores();  //Display high scores

    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
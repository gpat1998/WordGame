#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <cstdlib>
#include <vector>

using namespace std;

void process(istream& is, int& nLines, int& nWords, int& nChars) {
    string line, dummy;
    nLines = nWords = nChars = 0;
    
    while (getline(is, line)) {
        nChars += line.length();
        nLines++;
        
        istringstream iss(line);
        while (iss >> dummy)
            ++nWords;
    }
}

bool isContainedIn(string playerStr, string gameLetters) {
    int availableLetterCounts[256]; // Num pats in a byte
    
    for (int i = 0; i < 256; i++) {
        availableLetterCounts[i] = 0;
    }
    
    for (int i = 0; i < gameLetters.length(); i++) {
        int j = toupper(gameLetters[i]);
        availableLetterCounts[j]++;
    }
    
    for (int i = 0; i < playerStr.length(); i++) {
        int j = toupper(playerStr[i]);
        if (availableLetterCounts[j]-- <= 0)
            return false;
    }
    
    return true;
}

class Node;
class Processor;
class Trie;

class Node {
    friend class Trie;
    
public:
    Node();
    
private:
    std::vector<Node *> children;
    int wordCount;
    int prefixCount;
};

class Processor {
public:
    void operator()(char c); /*{std::cout << c;}*/
};

class Trie {
private:
    Node *head;
    
public:
    Trie();
    ~Trie();
    bool insert(std::string word);
    void traverse(Processor& processor);
    bool find(std::string word);
    
private:
    void deallocate(Node *currentNode);
    void traverseHelper(Processor& processor, Node *currentNode, std::string currentWord);
    static bool wordIsValid(std::string word);
};


Trie dictionary;

void createDictionary(string path)
{
    ifstream ifs(path);
    cout << "Creating the Dictionary" << endl;
    string word;
    while(getline(ifs, word))
    {
        dictionary.insert(word);
        
    }
    cout << "Dictionary Created" << endl;
}

int printAllWordsHelper(const string currentWord, const string selectedLetters, Trie &usedWords)
{
    int sum = 0;
    
    if (dictionary.find(currentWord) && !usedWords.find(currentWord))
    {
        cout << currentWord << endl;
        sum += currentWord.length() * 1.5;
        usedWords.insert(currentWord);
    }
    for (int i = 0; i < selectedLetters.length(); i++)
    {
        string unusedLetters = "";
        for (int j = 0; j < selectedLetters.length(); j++)
        {
            if (j != i)
            {
                unusedLetters += selectedLetters[j];
            }
        }
        sum += printAllWordsHelper(currentWord + selectedLetters[i], unusedLetters, usedWords);
    }
    
    return sum;
}


void printAllWords(const string selectedLetters, Trie &usedWords)
{
    int sum = 0;
    for (int i = 0; i < 7; i++)
    {
        string unusedLetters = "";
        for (int j = 0; j < 7; j++)
        {
            if (j != i)
            {
                unusedLetters += selectedLetters[j];
            }
        }
        string currentWord = "";
        currentWord += selectedLetters[i];
        sum += printAllWordsHelper(currentWord, unusedLetters, usedWords);
    }
    
    cout << "Your score could've been " << sum << "points more." << endl;
}

string selectLetters(int NUM_LETTERS, string VOWELS, string CONSONANTS)
{
    srand(time (NULL));
    string selectedLetters = "";
    for (int i = 0; i < NUM_LETTERS / 2; i++)
    {
        selectedLetters += VOWELS[rand() % 5];
        selectedLetters += CONSONANTS[rand() % 21];
    }
    if (NUM_LETTERS % 2 != 0)
        selectedLetters += CONSONANTS[rand() % 21];
    
    return selectedLetters;
}

int main(int argc, const char * argv[]) {
    
    createDictionary("/usr/share/dict/words");
    // Game constants
    
    const int NUM_LETTERS = 7;
    const int GAME_DUR = 30; // 3 minutes per game
    const double LENGTH_WEIGHT = 1.5;
    const string ENDSIGNAL = "*END*";
    const string VOWELS = "AEIOUY";
    const string CONSONANTS = "BCDFGHJKLMNPQRSTVWXZ";
    
    long ts = time(0L);
    int timeLeft = GAME_DUR;
    int score = 0;
    string word;
    
    
   Trie usedWords;
   
    string selectedLetters = selectLetters(NUM_LETTERS, VOWELS, CONSONANTS);
    
    // Main game loop
    while(true) {
        // Select 7 letters made of 3 vowels and 4 consonants
        
        // Check if the user has exceeded their time limit.
        // If so, message the user that this last word was discarded.
        // Break out of the loop to end the game.
        timeLeft = GAME_DUR - (int)(time(0L) - ts);
        
        if(timeLeft < 0)
        {
            cout << "Game Over. Don't even think about putting in a new word." << endl;
            break;
        }
        
        cout <<"Your letters for this round are: " << selectedLetters <<endl;
        cout <<"You have " << timeLeft << "secs to make words with them.";
        cout <<"Your current score is " << score <<endl;
        
        
        if (getline(cin, word)) {
        
            // Check if the user typed a special word to exit the game before
            // If so, break out of the loop.
            if (word == ENDSIGNAL)
            {
                cout << "You ended the game." << endl;
                break;
            }
            // Check that the word has the exact same chars as selectedLetters
            // If not, continue to the top of the loop after a message. --score;
            if(!isContainedIn(word, selectedLetters))
               {
                   score--;
                   cout << "You lost a point because you used illegal characters." << endl;
                   continue;
               }
            // Check if the word is a dupe (already submitted). For this you
            // may have to maintain a second Trie, initially empty, which
            // holds all the user submitted words.
            // If so, continue to the top of the loop after a message. --score;
            if(usedWords.find(word))
            {
                score--;
                cout << "You lost a point because you've already used this." << endl;
                continue;
            }
            // Check if the word is in the dictionary (e.g. dict.find(word))
            // If not, continue to the top of the loop after a message. --score;
            if(!dictionary.find(word))
            {
                score--;
                cout << "You lost a point because the word does not exist." << endl;
                continue;
            }
            
            // At this point the word is not a dupe and is valid.
            // Increment the score by the scoring function on the word length
            // Loop back
            score+= (word.length() * LENGTH_WEIGHT);
            cout << "Great job. You got one" << endl;
            usedWords.insert(word);
        }
    }
    
    // Report the final score to the user.
    
    if(score < 6)
    {
    cout << "Your score was: " << score << "\n" << "You did terrible. Don't bother playing this again." << endl;
    }
    else{
      cout << "Your score was: " << score << "\n" << "You did AMAZING. YOU SHOULD DEFINITELY PLAY THIS AGAIN." << endl;
    }
    // You may want to put the whole thing in an outer loop so you don't
    // have to reload the dictionary each round.
    
    printAllWords(selectedLetters, usedWords);
    
    return 0;
}

Node::Node()
{
    children.resize(26);
    for (int i = 0; i < 26; i++)
        children[i] = NULL;
    wordCount = 0;
    prefixCount = 0;
}

Trie::Trie()
{
    head = new Node();
}

Trie::~Trie()
{
    deallocate(head);
}

void Trie::deallocate(Node *currentNode)
{
    for (int i = 0; i < 26; i++)
    {
        if (currentNode->children[i] != NULL)
        {
            deallocate(currentNode->children[i]);
            delete currentNode->children[i];
            currentNode->children[i] = NULL;
        }
    }
}

bool Trie::insert(std::string word)
{
    if (!wordIsValid(word))
        return false;
    
    string lowerCaseWord = "";
    
    for (unsigned int i = 0; i < word.length(); i++)
        lowerCaseWord += tolower(word[i]);
    
    
    Node *currentNode = head;
    for (unsigned int i = 0; i < word.length(); i++)
    {
        char currentChar = lowerCaseWord[i];
        int indexOfChar = currentChar - 'a';
        if (currentNode->children[indexOfChar] == NULL)
            currentNode->children[indexOfChar] = new Node();
        
        currentNode = currentNode->children[indexOfChar];
        currentNode->prefixCount++;
        if (i == word.length() - 1)
            currentNode->wordCount++;
    }
    
    return true;
}

void Trie::traverse(Processor& processor)
{
    traverseHelper(processor, head, "");
}

bool Trie::wordIsValid(std::string word)
{
    for (unsigned int i = 0; i < word.length(); i++)
        if ((word[i] < 'a' || word[i] > 'z') &&
            (word[i] < 'A' || word[i] > 'Z'))
            return false;
    
    return true;
}

void Trie::traverseHelper(Processor& processor, Node *currentNode, string currentWord)
{
    for (int i = 0; i < 26; i++)
    {
        if (currentNode->children[i] != NULL)
        {
            char currentChar = i + 'a';
            if (currentNode->children[i]->wordCount > 0)
            {
                for (unsigned int j = 0; j < currentWord.length(); j++)
                    processor(currentWord[j]);
                processor(currentChar);
                cout << endl;
            }
            traverseHelper(processor, currentNode->children[i], currentWord + currentChar);
        }
    }
}

void Processor::operator()(char c)
{
    cout << c;
}

bool Trie::find(string word)
{
    if (!wordIsValid(word))
        return false;
    
    string lowerCaseWord = "";
    
    for (unsigned int i = 0; i < word.length(); i++)
        lowerCaseWord += tolower(word[i]);
    //^ Makes it lowercase
    
    Node *currentNode = head;
    for (unsigned int i = 0; i < word.length(); i++)
    {
    
    char currentChar = lowerCaseWord[i];
    int indexOfChar = currentChar - 'a';
    
    if (currentNode->children[indexOfChar] == NULL)
        return false;
        
    currentNode = currentNode->children[indexOfChar];
    }
    
    if(currentNode->wordCount > 0)
       return true;
    return false;
}


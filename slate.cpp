#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>
#include <conio.h>
using namespace std;

struct EditorAction {
    string type; // For "insert", "delete", "replace" operations
    int row;
    int col;
    char ch; // For a character actions
    string text; // For multiline actions
};

class TextEditor {
private:
    vector<string> lines;
    int cursorRow = 0;
    int cursorCol = 0;
    stack<EditorAction> undoStack;
    stack<EditorAction> redoStack;

    void display() {
        system("cls");

        cout<<"                                Slate: Text Editor                              "<<endl;
        cout<<"                          ----------------------------                          "<<endl;
        cout << "(Actions -> Ctrl+Z: Undo, Ctrl+Y: Redo, Ctrl+A: Search, Ctrl+R: Replace, Ctrl+S: Save & exit)" << endl;
        cout<<"============================================================================================="<<endl;

        for (int i = 0; i < lines.size(); i++) {
            if (i == cursorRow) {
                cout<<lines[i].substr(0, cursorCol)<<"|"<<lines[i].substr(cursorCol)<<endl;
            }
            else {
                cout<<lines[i]<<endl;
            }
        }
    }

    void insertChar(char c) {
        if (cursorRow >= lines.size())
            return;

        lines[cursorRow].insert(cursorCol, 1, c);
        undoStack.push({"insert", cursorRow, cursorCol, c, ""});

        while (!redoStack.empty())
            redoStack.pop();

        cursorCol++;
    }

    void deleteChar() {
        if (cursorRow >= lines.size() || cursorCol <= 0)
            return;

        char deletedChar = lines[cursorRow][cursorCol - 1];

        lines[cursorRow].erase(cursorCol - 1, 1);
        undoStack.push({"delete", cursorRow, cursorCol - 1, deletedChar, ""});

        while (!redoStack.empty())
            redoStack.pop();

        cursorCol--;
    }

    void undo() {
        if (undoStack.empty()) {
            cout<<"Nothing to undo!\n";
            return;
        }

        EditorAction action = undoStack.top();
        undoStack.pop();

        if (action.type == "insert") {
            lines[action.row].erase(action.col, 1);
        }
        else if (action.type == "delete") {
            lines[action.row].insert(action.col, 1, action.ch);
        }
        else if (action.type == "replace") {
            lines[action.row].replace(action.col, action.text.size(), action.text);
        }

        redoStack.push(action);
    }

    void redo() {
        if (redoStack.empty()) {
            cout<<"Nothing to redo!\n";
            return;
        }

        EditorAction action = redoStack.top();
        redoStack.pop();

        if (action.type == "insert") {
            lines[action.row].insert(action.col, 1, action.ch);
        }
        else if (action.type == "delete") {
            lines[action.row].erase(action.col, 1);
        }
        else if (action.type == "replace") {
            lines[action.row].replace(action.col, action.text.size(), action.text);
        }

        undoStack.push(action);
    }

    void search(const string& keyword) {
        bool found = false;
        for (int i = 0; i < lines.size(); i++) {
            size_t pos = lines[i].find(keyword);

            if (pos != string::npos) {
                cout<<"Found at line "<<i + 1<<", column "<<pos + 1<<": "<<lines[i]<<endl;
                found = true;
            }
        }

        if (!found) {
            cout<<"Keyword not found.\n";
        }
    }

    void replace(const string& oldWord, const string& newWord) {
        for (int i = 0; i < lines.size(); i++) {
            size_t pos = 0;

            while ((pos = lines[i].find(oldWord, pos)) != string::npos) {
                undoStack.push({"replace", i, (int)pos, '\0', oldWord});
                lines[i].replace(pos, oldWord.size(), newWord);
                pos += newWord.size();
            }
        }

        while (!redoStack.empty())
            redoStack.pop();

        cout<<"Replaced all occurrences of \""<<oldWord<<"\" with \""<<newWord<<"\".\n";
    }

public:
    void run() {
        lines.push_back("");

        bool quit = false;
        do {
            display();

            char c = _getch();

            switch (c) {
                case 26: // Ctrl+Z
                    undo();
                    break;
                case 25: // Ctrl+Y
                    redo();
                    break;
                case 1: // Ctrl+A
                    {
                        cout<<"Enter keyword to search: ";
                        string keyword;
                        cin >> keyword;
                        search(keyword);
                    }
                    break;
                case 18: // Ctrl+R
                    {
                        cout<<"Enter keyword to replace: ";
                        string oldWord;
                        cin >> oldWord;
                        cout<<"Enter new word: ";
                        string newWord;
                        cin >> newWord;
                        replace(oldWord, newWord);
                    }
                    break;
                case 13: // Enter key
                    cursorRow++;
                    if (cursorRow >= lines.size())
                        lines.push_back("");
                    cursorCol = 0;
                    break;
                case 8: // Backspace key
                    deleteChar();
                    break;
                case 19: // Ctrl+S
                    cout<<"Saving and closing the editor!";
                    quit = true;
                    break;
                default:
                    insertChar(c);
            }
        } while(!quit);
    }
};

int main() {
    TextEditor editor;
    editor.run();

    return 0;
}

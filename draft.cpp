/*
	Debug and implement other functions.
	
*/

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <conio.h>
#include <windows.h> // for accessing windows API
using namespace std;

// global
string marqueeText = "Welcome to the C++ Marquee Emulator!";
atomic<bool> marqueeRunning(false);
atomic<int> marqueeSpeed(200);
mutex textMutex;

void startMessage() {
	cout << "WELCOME to CSOPESY!\n";
	
	cout << "Group developer:\n";
	cout << "Corpuz, Gerald Justine\n";
	cout << "De Jesus, Andrei Zarmin\n";
	cout << "Manaois, Chriscel John\n";
	cout << "Sayat, John Christian\n";
	
	cout << "Version date: 2.0\n";
}

void commandInterpreter() {
    string command;
    while (true) {
        cout << "\nCommand> ";
        cin >> command;

        if (command == "help") {
            cout << "Available commands:\n"
                 << "  \"help\"          - Display the commands and their use.  \n"
                 << "  \"start_marquee\" - Start the marquee \"animation\".     \n"
                 << "  \"stop_marquee\"  - Stop the marquee \"animation\".      \n"
                 << "  \"set_text\"      - Change the marquee text.             \n"
                 << "  \"set_speed\"     - Change marquee speed (ms).           \n"
                 << "  \"exit\"          - Quit program.                        \n";
        }
        else if (command == "start_marquee") {
            marqueeRunning = true;
        }
        else if (command == "stop_marquee") {
            marqueeRunning = false;
        }
        else if (command == "set_text") {
            cout << "Enter new text: ";
            cin.ignore(); 
            string newText;
            getline(cin, newText);
            lock_guard<mutex> lock(textMutex);
            marqueeText = newText;
        }
        else if (command == "set_speed") {
            cout << "Enter speed in ms: ";
            int speed;
            cin >> speed;
            if (speed > 0) marqueeSpeed = speed;
        }
        else if (command == "exit") {
            cout << "Exiting console...\n";
            exit(0);
        }
        else {
            cout << "Unknown command. Type 'help' for options.\n";
        }
    }
}


int main() {
	
	startMessage();
	commandInterpreter();
	
	return 0;
}
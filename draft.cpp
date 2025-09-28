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
#include <queue> // queue data struct for Keyboard Handler (input buffer)


// global
std::string marqueeText = "Welcome to the C++ Marquee Emulator!";
std::atomic<bool> marqueeRunning(false); // thread-safe boolean flag
std::atomic<int> marqueeSpeed(200); // thread-safe int variable, sets speed of marquee display
// in miliseconds
std::mutex textMutex;


void startMessage() {
    using namespace std; // do not declare this global, since we have multiple libraries 
	cout << "WELCOME to CSOPESY!\n";
	
	cout << "Group developer:\n";
	cout << "Corpuz, Gerald Justine\n";
	cout << "De Jesus, Andrei Zarmin\n";
	cout << "Manaois, Chriscel John\n";
	cout << "Sayat, John Christian\n";
	
	cout << "Version date: 2.0\n";
}

void commandInterpreter() {
    
    std::string command;
    while (true) {
        std::cout << "\nCommand> ";
        std::cin >> command;

        if (command == "help") {
            std::cout << "Available commands:\n"
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
            using namespace std;
            cout << "Enter new text: ";
            cin.ignore(); 
            string newText;
            getline(cin, newText);
            lock_guard<mutex> lock(textMutex);
            marqueeText = newText;
        }
        else if (command == "set_speed") {
            using namespace std;
            cout << "Enter speed in ms: ";
            int speed;
            cin >> speed;
            if (speed > 0) marqueeSpeed = speed;
        }
        else if (command == "exit") {
            std::cout << "Exiting console...\n";
            exit(0);
        }
        else {
            std::cout << "Unknown command. Type 'help' for options.\n";
        }
    }
}




int main() {
	
	startMessage();
	commandInterpreter();
	
	return 0;
}
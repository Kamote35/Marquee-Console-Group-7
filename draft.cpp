/*
	Debug and implement other functions.
	
*/

#include <iostream>
#include <string> // string manipulation, useful for creating Marquee Animation
#include <thread> // threading
#include <chrono> // time functions
#include <atomic> // atomic variables for thread safety
#include <mutex> // mutual exclusion for thread safety
#include <conio.h> // console input / output
#include <windows.h> // for accessing windows API
#include <queue> // queue data struct for Keyboard Handler (input buffer)


// global
std::string marqueeText = "Welcome to the C++ Marquee Emulator!";
std::atomic<bool> marqueeRunning(false); // thread-safe boolean flag
std::atomic<int> marqueeSpeed(200); // thread-safe int variable, sets speed of marquee display
// in miliseconds
std::mutex textMutex;
std::queue<std::string> keyboard_queue;
std::mutex queue_mutex;
std::atomic<bool> keyboard_stop(false);


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

// Handles the Keyboard inputs
void keyboardHandler() { 
    std::string input_buffer = "";

    while (!keyboard_stop) {
        if (_kbhit()) { // check any input from the keyboard
            char key = _getch(); // get pressed key

            // Check for ENTER key input (Priority)
            if (key == '\r') { // use carriage return '\r' to detect an Enter key input. 
                               // '\n' is not actual keyboard input, remember CSARCH2 lol.

                std::lock_guard<std::mutex> lock(queue_mutex); // Lock the queue while pushing, automatically unlocks after
                keyboard_queue.push(input_buffer);
                input_buffer = ""; // reinitialize 
                std::cout << std::endl; // automatically newline after Enter
            }
            // Check for Backspace
            else if (key == '\b') {
                if (!input_buffer.empty()) { // check if there are characters in the input
                    input_buffer.pop_back(); // pop Last-In char from queue lol
                    std::cout << "\b \b"; // move cursor back by one character, replace with " " (space),
                    // then move cursor back again.
                    // manual backspace
                }
            }
            // check for displayable characters, refer to ASCII table
            else if (key >= ' ' && key <= '~') { // ASCII decimal 32 ' ' (space) to decimal 126 '~' (tilde)
            // is the part of the ASCII table where all the displayable Alphabet, Numbers, and Symbols reside.
                
                input_buffer += key; // add into the character array the current _getch
                std::cout << key; // display the DISPLAYABLE CHARACTER duhh
            }
        }


        // sleep this thread's while loop, give chance for marquee animation
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
       

    }

}


int main() {
	
    std::thread kb_thread(keyboardHandler); // Keyboard Handler thread

    // main thread
	startMessage();
	commandInterpreter();


    keyboard_stop = true; // end keyboard thread constant checking for inputs
    
    // CLEANUP
    kb_thread.join();
	
	return 0;
}
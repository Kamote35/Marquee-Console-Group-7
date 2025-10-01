#include <iostream>
#include <string> // string manipulation, useful for creating Marquee Animation
#include <thread> // threading
#include <chrono> // time functions
#include <atomic> // atomic variables for thread safety
#include <mutex> // mutual exclusion for thread safety
#include <conio.h> // console input / output
#include <windows.h> // for accessing windows API
#include <queue> // queue data struct for Keyboard Handler (input buffer)
#include <sstream> // to parse command arguments from the single input line


// global
std::string marqueeText = "PLEASE SET TEXT (THIS IS THE DEFAULT TEXT)";
std::atomic<bool> marqueeRunning(false); // thread-safe boolean flag
std::atomic<int> marqueeSpeed(200); // thread-safe int variable, sets speed of marquee in miliseconds
std::mutex textMutex;
std::queue<std::string> keyboard_queue;
std::mutex queue_mutex;
std::atomic<bool> keyboard_stop(false);
std::atomic<bool> command_stop(false);
std::atomic<bool> marquee_stop(false);


// utility: move cursor to (x,y)
void setCursorPosition(int x, int y) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
}

// get console size
int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

// clear an entire line y
void clearLine(int y) {
    int width = getConsoleWidth();
    setCursorPosition(0, y);
    std::cout << std::string(width, ' ') << std::flush;
    setCursorPosition(0, y);
}

// display starting message
void startMessage() {
    using namespace std; // do not declare this global, since we have multiple libraries 
	cout << "WELCOME to CSOPESY!\n";
	
	cout << "Group developer:\n";
	cout << "Corpuz, Gerald Justine\n";
	cout << "De Jesus, Andrei Zarmin\n";
	cout << "Manaois, Chriscel John\n";
	cout << "Sayat, John Christian\n";
	
	cout << "Version date: 3.3\n\n";
}

// Add this function near your other utility functions
void displayPrompt() {
    std::cout << "Command>" << std::flush;
}

// handles OS emulator with marquee animation commands
void commandInterpreter() {
    while (!command_stop) {
        std::string full_input_line; // Renamed 'command' to 'full_input_line' for clarity
        std::string command; // The actual command word (e.g., "set_text")

        if (!keyboard_queue.empty()) {
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (!keyboard_queue.empty()) {
                    full_input_line = keyboard_queue.front();
                    keyboard_queue.pop();
                } else {
                    continue;
                }
            }
            
            // Extract the main command from the full input line
            std::stringstream ss(full_input_line);
            ss >> command;

            // Handle the command first
            if (command == "help") {
                std::cout << "Available commands:\n"
                     << "  \"help\"            - Display the commands and their use. \n"
                     << "  \"start_marquee\"   - Start the marquee \"animation\".    \n"
                     << "  \"stop_marquee\"    - Stop the marquee \"animation\".     \n"
                     << "  \"set_text\" [text] - Change the marquee text.            \n" 
                     << "  \"set_speed\" [ms]  - Change marquee speed (ms).          \n" 
                     << "  \"exit\"            - Quit program.                       \n"
                     << std::endl;
                displayPrompt();// Display prompt after command output
            }
            else if (command == "start_marquee") {
                if (!marqueeRunning) {
                    marqueeRunning = true;
                }
                else std::cout << "Marquee animation is already running!\n";
                std::cout << std::endl;
                displayPrompt();
            }
            else if (command == "stop_marquee") {
                if (marqueeRunning) {
                    marqueeRunning = false;
                }
                else std::cout << "Marquee animation is not running!\n";
                std::cout << std::endl;
                displayPrompt();
            }
            else if (command == "set_text") {
                std::string newText;
                
                // Read the rest of the line as the new text (including spaces)
                // ss is already positioned right after the command word
                std::getline(ss >> std::ws, newText); // >> std::ws skips leading whitespace

                if (newText.empty()) {
                    std::cout << "Error: Please provide text after 'set_text'.\n\n";
                } else {
                    {
                        std::lock_guard<std::mutex> lock(textMutex);
                        marqueeText = newText;
                    }
                    std::cout << "Marquee text successfully set to: \"" << newText << "\"\n\n";
                }
                displayPrompt();
            }
            else if (command == "set_speed") {
                std::string speedStr;
                
                // Extract the next word (the argument) as the speed string
                ss >> speedStr; 
                
                if (speedStr.empty()) {
                    std::cout << "Error: Please provide a speed value (ms) after 'set_speed'.\n\n";
                } else {
                    try {
                        int speed = std::stoi(speedStr);
                        if (speed > 0) {
                            marqueeSpeed = speed;
                            std::cout << "Marquee speed set to " << speed << "ms.\n\n";
                        } else {
                            std::cout << "Invalid speed value. Must be a positive integer.\n\n";
                        }
                    } catch (...) {
                        std::cout << "Invalid speed value. Please enter a number.\n\n";
                    }
                }
                displayPrompt();
            }
            else if (command == "exit") {
                std::cout << "Exiting console...\n";
                command_stop = true;
                keyboard_stop = true;
                marquee_stop = true;// Add this line
                break;
            }
            else {
                std::cout << "Unknown command. Type 'help' for options.\n\n";
                displayPrompt();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

// handles the Keyboard inputs
void keyboardHandler() { 
    std::string input_buffer = "";

    // for debugging
    //std::cout << "Keyboard Handler Debug\n";

    while (!keyboard_stop) {
        if (_kbhit()) { // check any input from the keyboard
            char key = _getch(); // get pressed key

            // Check for ENTER key input (Priority)
            if (key == '\r') {
                std::lock_guard<std::mutex> lock(queue_mutex);
                keyboard_queue.push(input_buffer);
                input_buffer = "";
                std::cout << std::endl;
                
                
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
        std::this_thread::sleep_for(std::chrono::microseconds(500)); 
       

    }

}

void marqueeWorker() {
    size_t pos = 0;
    std::string text;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    while (!marquee_stop) {  // Check stop condition
        if (marqueeRunning) {
            {
                std::lock_guard<std::mutex> lock(textMutex);
                text = marqueeText + "   ";
            }

            int width = getConsoleWidth();
            std::string display(width, ' ');

            for (size_t i = 0; i < text.size(); i++) {
                int index = (pos + i) % text.size();
                if (i < (size_t)width) display[i] = text[index];
            }

            // Save cursor position (so typing isn’t disturbed)
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hOut, &csbi);
            COORD saved = csbi.dwCursorPosition;

            // Write marquee at line 0
            setCursorPosition(0, 0);
            std::cout << display << std::flush;

            // Restore cursor position for user typing
            SetConsoleCursorPosition(hOut, saved);

            pos = (pos + 1) % text.size();
            std::this_thread::sleep_for(std::chrono::milliseconds(marqueeSpeed));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}



int main() {
    // Thread Creation
    std::thread kb_thread(keyboardHandler); 
    std::thread marqueeThread(marqueeWorker); 
    std::thread cmd_thread(commandInterpreter);

    // Main thread
    startMessage();
    displayPrompt();  // Initial prompt

	// Sleep Main thread
    while (!command_stop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // give chance to others
    }
    
    // Set all stop flags
    keyboard_stop = true;
    marquee_stop = true;  // Add this line
    
    // CLEANUP
    marqueeThread.join();
    kb_thread.join();
    cmd_thread.join();
    
    return 0;
}
/*
	Debug and implement other functions.
*/

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <windows.h>

// global
std::string marqueeText = "PLEASE SET TEXT TO DISPLAY";
std::atomic<bool> marqueeRunning(false);
std::atomic<int> marqueeSpeed(200); // ms
std::mutex textMutex;

// utility: move cursor to (x,y)
void setCursorPosition(int x, int y) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {(SHORT)x, (SHORT)y};
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

void startMessage() {
    // Print intro BELOW reserved marquee + command line
    setCursorPosition(0, 3);
    std::cout << "WELCOME to CSOPESY!\n";
    std::cout << "Group developer:\n";
    std::cout << "Corpuz, Gerald Justine\n";
	std::cout << "De Jesus, Andrei Zarmin\n";
	std::cout << "Manaois, Chriscel John\n";
	std::cout << "Sayat, John Christian\n";
    std::cout << "Version date: 3.0\n\n";
}

void marqueeWorker() {
    size_t pos = 0;
    std::string text;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    while (true) {
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
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void commandInterpreter() {
    std::string command;
    while (true) {
        clearLine(1);
        std::cout << "Command> " << std::flush;

        std::getline(std::cin, command);  // capture full input including spaces

        clearLine(1); // clear user input immediately after Enter

        if (command == "help") {
            setCursorPosition(0, 3);
            std::cout << "\nAvailable commands:\n"
                      << "  help                   - Show commands\n"
                      << "  start_marquee          - Start marquee animation\n"
                      << "  stop_marquee           - Stop marquee animation\n"
                      << "  set_text <new text>    - Change marquee text\n"
                      << "  set_speed <ms>         - Change marquee speed (ms)\n"
                      << "  exit                   - Quit program\n\n";
        } else if (command == "start_marquee") {
            marqueeRunning = true;
        } else if (command == "stop_marquee") {
            marqueeRunning = false;
        } else if (command.rfind("set_text ", 0) == 0) {
            // everything after "set_text " becomes the new text
            std::string newText = command.substr(9);
            std::lock_guard<std::mutex> lock(textMutex);
            marqueeText = newText;
        } else if (command.rfind("set_speed ", 0) == 0) {
            try {
                int speed = std::stoi(command.substr(10));
                if (speed > 0) marqueeSpeed = speed;
            } catch (...) {
                setCursorPosition(0, 2);
                std::cout << "Invalid speed value.\n";
            }
        } else if (command == "exit") {
			system("cls"); // clear the console screen
			setCursorPosition(0, 0);
			std::cout << "Exiting console...\n";
			exit(0);
        } else if (!command.empty()) {
            setCursorPosition(0, 2);
            std::cout << "Unknown command. Type 'help' for options.\n";
        }
    }
}

int main() {
    startMessage();

    std::thread marqueeThread(marqueeWorker);
    commandInterpreter();

    marqueeThread.join();
    return 0;
}

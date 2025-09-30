/*
    CSOPESY Marquee Emulator v4.0
    - Combines async input (non-blocking typing) with simple std::getline parsing
    - Marquee runs in its own thread
    - Command interpreter runs in another thread
    - Supports inline arguments (set_text Hello, set_text "Hello World", set_speed 150)
*/

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <queue>

// global
std::string marqueeText = "PLEASE SET TEXT TO DISPLAY";
std::atomic<bool> marqueeRunning(false);
std::atomic<int> marqueeSpeed(200); // ms
std::mutex textMutex;
std::queue<std::string> commandQueue;
std::mutex queueMutex;
std::atomic<bool> stopAll(false);

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

// startup banner
void startMessage() {
    setCursorPosition(0, 3);
    std::cout << "WELCOME to CSOPESY!\n\n";
    std::cout << "Group developer:\n";
    std::cout << "Corpuz, Gerald Justine\n";
    std::cout << "De Jesus, Andrei Zarmin\n";
    std::cout << "Manaois, Chriscel John\n";
    std::cout << "Sayat, John Christian\n\n";
    std::cout << "Version date: 4.0\n\n";
}

// parse command into cmd + args (with quotes support)
std::pair<std::string, std::string> parseCommand(const std::string& input) {
    std::string trimmed = input;
    // trim leading/trailing spaces
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

    size_t spacePos = trimmed.find(' ');
    if (spacePos == std::string::npos) {
        return {trimmed, ""}; // no args
    }

    std::string cmd = trimmed.substr(0, spacePos);
    std::string args = trimmed.substr(spacePos + 1);

    // handle quotes
    if (!args.empty() && args.front() == '"' && args.back() == '"' && args.size() >= 2) {
        args = args.substr(1, args.size() - 2);
    }

    return {cmd, args};
}

// marquee thread
void marqueeWorker() {
    size_t pos = 0;
    std::string text;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    while (!stopAll) {
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

            // Save cursor position
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hOut, &csbi);
            COORD saved = csbi.dwCursorPosition;

            // Write marquee at line 0
            setCursorPosition(0, 0);
            std::cout << display << std::flush;

            // Restore cursor
            SetConsoleCursorPosition(hOut, saved);

            pos = (pos + 1) % text.size();
            std::this_thread::sleep_for(std::chrono::milliseconds(marqueeSpeed));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

// input thread (non-blocking, pushes full lines to queue)
void inputHandler() {
    std::string line;
    while (!stopAll) {
        clearLine(1);
        std::cout << "Command> " << std::flush;

        if (!std::getline(std::cin, line)) break; // EOF/CTRL+Z

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            commandQueue.push(line);
        }
    }
}

// command interpreter thread
void commandInterpreter() {
    while (!stopAll) {
        std::string command;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!commandQueue.empty()) {
                command = commandQueue.front();
                commandQueue.pop();
            }
        }

        if (command.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        auto parsed = parseCommand(command);
        std::string cmd = parsed.first;
        std::string args = parsed.second;

        if (cmd == "help") {
            setCursorPosition(0, 3);
            std::cout << "\nAvailable commands:\n"
                      << "  help                 - Show commands\n"
                      << "  start_marquee        - Start marquee animation\n"
                      << "  stop_marquee         - Stop marquee animation\n"
                      << "  set_text <new text>  - Change marquee text (quotes supported)\n"
                      << "  set_speed <ms>       - Change marquee speed (ms)\n"
                      << "  exit                 - Quit program\n\n";
        }
        else if (cmd == "start_marquee") {
            marqueeRunning = true;
        }
        else if (cmd == "stop_marquee") {
            marqueeRunning = false;
        }
        else if (cmd == "set_text") {
            if (!args.empty()) {
                std::lock_guard<std::mutex> lock(textMutex);
                marqueeText = args;
            } else {
                setCursorPosition(0, 2);
                std::cout << "Usage: set_text <new text>\n";
            }
        }
        else if (cmd == "set_speed") {
            if (!args.empty()) {
                try {
                    int speed = std::stoi(args);
                    if (speed > 0) marqueeSpeed = speed;
                } catch (...) {
                    setCursorPosition(0, 2);
                    std::cout << "Invalid speed value.\n";
                }
            } else {
                setCursorPosition(0, 2);
                std::cout << "Usage: set_speed <milliseconds>\n";
            }
        }
        else if (cmd == "exit") {
            system("cls");
            setCursorPosition(0, 0);
            std::cout << "Exiting console...\n";
            stopAll = true;
            break;
        }
        else {
            setCursorPosition(0, 2);
            std::cout << "Unknown command. Type 'help' for options.\n";
        }
    }
}

// main
int main() {
    startMessage();

    std::thread marqueeThread(marqueeWorker);
    std::thread inputThread(inputHandler);
    std::thread cmdThread(commandInterpreter);

    marqueeThread.join();
    inputThread.join();
    cmdThread.join();

    return 0;
}

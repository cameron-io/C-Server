#ifndef CLI_H
#define CLI_H

#include <thread>
#include <limits>
#include <iostream>
#include <signal.h>
#include "core.hh"

void start_repl_async(int *keep_running)
{
    std::thread client_thread(
        [keep_running]()
        {
            std::string userInput;

#define BIT_LIMIT std::numeric_limits<std::size_t>::digits
#define THREADS_AVAILABLE std::thread::hardware_concurrency()

            std::cout << "C/C++ SHELL: " << " "
                      << "[" << BIT_LIMIT << "-bit] "
                      << "[threads: " << THREADS_AVAILABLE << "]"
                      << std::endl;
            while (1)
            {
                std::cout << ">>> ";
                std::cin >> userInput;

                if (userInput == ".")
                {
                    std::cout << "SyntaxError: invalid syntax" << std::endl;
                }
                else if (userInput == "help()")
                {
                    std::cout << "Usage: ..." << std::endl;
                }
                else if (userInput == "exit()")
                {
                    break;
                }
                else
                {
                    std::cout << userInput << std::endl;
                }
            }

            *keep_running = 0;

            std::cout << std::endl
                      << "Shutting down..." << std::endl;
        });
    client_thread.detach();
}

#endif
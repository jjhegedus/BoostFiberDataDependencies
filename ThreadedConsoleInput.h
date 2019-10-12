#pragma once

#include "pch.h"
#include <iostream>
#include <string>
#include "ThreadSafeQueue.h"


namespace ndtech {

  struct ThreadedConsoleInput {

    ndtech::ThreadSafeQueue<std::string>* m_q;

    ThreadedConsoleInput(ndtech::ThreadSafeQueue<std::string>* q) {
      m_q = q;
    }

    void runOn(std::thread& thread) {
      thread = std::thread(&ThreadedConsoleInput::run, this);
    }

    void run() {
      std::cout << "ThreadedConsoleInput threadId = " << std::this_thread::get_id() << std::endl;

      std::string line;
      while (std::getline(std::cin, line))
      {
        m_q->Push(line);
        if (line == "q") {
          break;
        }
      }
      std::cout << "Exited UI loop" << std::endl;
    }

  };

}
#pragma once

#include "ThreadSafeQueue.h"
#include "NamedItemStore.h"
#include <boost/fiber/all.hpp>

namespace ndtech {

  template <typename ItemType>
  struct FiberedQueueProcessor {

    ndtech::ThreadSafeQueue<std::string>* m_q;
    bool m_running = true;
    NamedItemStore<std::string> m_store;

    FiberedQueueProcessor(ndtech::ThreadSafeQueue<std::string>* q) {
      m_q = q;
    }

    void runOn(boost::fibers::fiber& fiber) {
      fiber = boost::fibers::fiber(&FiberedQueueProcessor::Run, this);
    }

    void runOn(std::thread& fiber) {
      fiber = std::thread(&FiberedQueueProcessor::Run, this);
    }


    void Run() {
      boost::fibers::fiber itemStoreFiber;
      m_store.RunOn(itemStoreFiber);

      while (m_running) {
        ItemType item = m_q->Pop();

        if (item == "q") {
          std::cout << "Received quit signal" << std::endl;
          m_running = false;
          break;
        }
        if (item == "a") {

          boost::fibers::fiber t([this]() {

            std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item:  Enter Item Name: ";
            std::string newName = m_q->Pop();
            std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item:  Enter Item value: ";
            std::string newValue = m_q->Pop();
            std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item: NAME = " << newName << " VALUE = " << newValue << std::endl;

            boost::fibers::fiber([this, newName, newValue]() {m_store.AddItem(newName, newValue); }).detach();
            });

          t.join();
        }
        else if (item == "p") {
          boost::fibers::fiber t([this]() {

            std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Printing Item:  Enter Item Name: ";
            std::string itemName = m_q->Pop();
            std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Printing Item: NAME = " << itemName << std::endl;

            boost::fibers::fiber([this, itemName]() {
              auto returnedItem = m_store.GetItem(itemName);
              std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Got Item: NAME = " << itemName << " VALUE = " << (*returnedItem).second << std::endl;
              }).detach();

            //tp.doJob([this, itemName]() {
            //  auto returnedItem = m_store.GetItem(itemName);
            //  std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Got Item: NAME = " << itemName << " VALUE = " << (*returnedItem).second << std::endl;
            //  });


            });

          t.join();
        }
        else if (item == "r") {
          boost::fibers::fiber t([this]() {

            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Removing Item:  Enter Item Name: ";
            std::string itemName = m_q->Pop();
            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Removing Item: NAME = " << itemName << std::endl;

            boost::fibers::fiber([this, itemName]() {
              m_store.RemoveItem(itemName);
              std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": ThreadedQueueProcessor:  Removed Item: NAME = " << itemName << std::endl;
              }).detach();

            //tp.doJob([this, itemName]() {
            //  m_store.RemoveItem(itemName);
            //  std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": ThreadedQueueProcessor:  Removed Item: NAME = " << itemName << std::endl;
            //  });

            });

          t.join();
        }

        else {
          std::cout << "Read '" << item << "' from stdin, which is not a valid message" << std::endl;
        }

        boost::this_fiber::yield();
      }

      m_store.Stop();
      itemStoreFiber.join();
    }

  };

}
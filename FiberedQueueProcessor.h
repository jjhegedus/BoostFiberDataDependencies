#pragma once

#include "ThreadSafeQueue.h"
#include "NamedItemStore.h"
#include <boost/fiber/all.hpp>
#include "MultiItemStore.h"
#include <iostream>

namespace ndtech {

  template <typename ItemType>
  struct FiberedQueueProcessor {

    ndtech::ThreadSafeQueue<std::string>* m_q;
    bool m_running = true;
    NamedItemStore<std::string> m_store;
    MultiItemStore<std::string, std::string, int> m_multiItemStore;

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
      boost::fibers::fiber multiStoreFiber;
      m_multiItemStore.RunOn(multiStoreFiber);

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
        else 
          if (item == "p") {
          boost::fibers::fiber t([this]() {

            std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Printing Item:  Enter Item Name: ";
            std::string itemName = m_q->Pop();
            std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Printing Item: NAME = " << itemName << std::endl;

            boost::fibers::fiber([this, itemName]() {
              auto returnedItem = m_store.GetItem(itemName);
              std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Got Item: NAME = " << itemName << " VALUE = " << returnedItem << std::endl;
              }).detach();


            });

          t.join();
        }
        else 
            if (item == "r") {
          boost::fibers::fiber t([this]() {

            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Removing Item:  Enter Item Name: ";
            std::string itemName = m_q->Pop();
            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Removing Item: NAME = " << itemName << std::endl;

            boost::fibers::fiber([this, itemName]() {
              m_store.RemoveItem(itemName);
              std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": ThreadedQueueProcessor:  Removed Item: NAME = " << itemName << std::endl;
              }).detach();

            }
          );

          t.join();
        }
        else
          if (item == "am") {

            boost::fibers::fiber t([this]() {

              std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item:  Enter Item Name: ";
              std::string newName = m_q->Pop();
              std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item:  Enter Item string value: ";
              std::string newStringValue = m_q->Pop();
              std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item:  Enter Item int value: ";
              int newIntValue = std::stoi(m_q->Pop());
              std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item: NAME = " << newName << " STRING_VALUE = " << newStringValue << " INT_VALUE = " << newIntValue << std::endl;

              boost::fibers::fiber(
                [this, newName, newStringValue, newIntValue]() {
                  m_multiItemStore.AddItem(newName, newStringValue, newIntValue); 
                }).detach();
              });

            t.join();

          }
          else 
            if (item == "pm") {
            boost::fibers::fiber t([this]() {

              std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Printing Item:  Enter Item Name: ";
              std::string itemName = m_q->Pop();
              std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Printing Item: NAME = " << itemName << std::endl;

              boost::fibers::fiber([this, itemName]() {
                auto returnedItem = m_multiItemStore.GetItem(itemName);
                std::cout << "FiberedQueueProcessor threadId = " << std::this_thread::get_id() << ": Got Item: NAME = " << itemName << " STRING_VALUE = " << std::get<0>(returnedItem) << " INT_VALUE = " << std::get<1>(returnedItem) << std::endl;
                }).detach();


              });

            t.join();

          }
          else 
              if (item == "rm") {
            boost::fibers::fiber t([this]() {

              std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Removing Item:  Enter Item Name: ";
              std::string itemName = m_q->Pop();
              std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Removing Item: NAME = " << itemName << std::endl;

              boost::fibers::fiber([this, itemName]() {
                m_multiItemStore.RemoveItem(itemName);
                std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": ThreadedQueueProcessor:  Removed Item: NAME = " << itemName << std::endl;
                }).detach();

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
      m_multiItemStore.Stop();
      multiStoreFiber.join();
    }

  };

}
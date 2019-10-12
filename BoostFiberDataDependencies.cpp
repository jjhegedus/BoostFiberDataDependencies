// BoostFiberDataDependencies.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <string>
#include <boost/fiber/all.hpp>
#include "ThreadSafeQueue.h"
#include "ThreadedConsoleInput.h"
#include "FiberedQueueProcessor.h"

int main()
{
  std::cout << "main threadId = " << std::this_thread::get_id() << "\n\n";
  std::cout << "Test data dependencies by using the following commands\n\n" <<
    "  p<enter> - 'print' - Launches the print command and requests the item name you wish to print.\n" <<
    "  Type in an item name without spaces and hit enter.  Any name will do since nothing exists yet.\n"
    "  The print command will wait until the requested data exists, then print it.\n" <<
    "  However, this program uses multiple threads to allow you to continue to enter additional commands while you wait.\n\n" <<
    "  a<enter> - 'add item' - Launches the add item command and asks for the name of the item to add.\n" <<
    "  Type in the item name you previously passed to the print command.\n" <<
    "  Next, the add item command will ask for the value of the item to add.\n" <<
    "  Type in the value of the item and hit enter.\n" <<
    "  The add item command will indicate that the item has been added.\n" <<
    "  The print command will now complete because it is notified that the item it was looking for now exists.\n\n" <<
    "  r<enter> - 'remove' - This will launch the remove command and asks for the name of the item to remove\n" <<
    "  Type in the name of an item which will be removed if it exists.\n\n"
    "  q<enter> - 'quit' - This will end the program.\n\n";


  ndtech::ThreadSafeQueue<std::string> tsq;

  ndtech::ThreadedConsoleInput threadedConsoleInput(&tsq);
  std::thread inputThread;
  threadedConsoleInput.runOn(inputThread);

  std::thread queueProcessingThread;
  ndtech::FiberedQueueProcessor<std::string> qp(&tsq);
  qp.runOn(queueProcessingThread);

  inputThread.join();
  queueProcessingThread.join();

}
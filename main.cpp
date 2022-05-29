//
//  main.cpp
//  Real Time - Mutex
//
//  Created by Dilara Karakaş
//

#include <iostream>
#include "GTU.h"

using namespace std;

constexpr auto THREAD_SIZE = 5;

void thread_1_function(void);
void thread_2_function(void);

int main(void)
{
    srand( static_cast<unsigned int>(time(nullptr)));
    thread t[THREAD_SIZE];

    for (int i = 0; i < THREAD_SIZE; ++i)
        t[i] = rand()%2 ? thread(thread_1_function) : thread(thread_2_function);

    thread notScheduledThread(thread_1_function);

    for (int i = 0; i < THREAD_SIZE; ++i)
        gtu::setScheduleThread(t[i],i+1);

    gtu::startProtocol();

    for (int i = 0; i < THREAD_SIZE; ++i)
        t[i].join();

    notScheduledThread.join();

    return 0;
}

//
//  GTU.h
//  Real Time - Mutex
//
//  Created by Dilara Karakaş
//

#ifndef GTU_h
#define GTU_h

#include <stdio.h>

#include <iostream>
#include <mutex>
#include <chrono>
#include <vector>
#include <condition_variable>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

namespace gtu{
    void startProtocol();
    void setScheduleThread(thread& thisThread, int priorityOfThread);

    class mutex;

    class ThreadInfo{
    public:
        const int getRegisteredPriority(void)const;
        const thread::id getRegisteredThreadId(void)const;
        const vector<gtu::mutex*> getRegisteredMutexes(void)const;
        void setRegisteredPriority(const int newPriority);
        void setRegisteredThreadId(const thread::id newThreadID);
        void addRegisteredMutexes(gtu::mutex* newMutex);

    private:
        int regPriority;
        thread::id regThread_id;
        vector<gtu::mutex*> regMutexes;
    };

    class mutex : public std::mutex{
        public:
            void lock();
            void unlock();
            void setCeil(const int newCeil);
            void setAcquired(const bool newAcquired);
            void registerThreadToMutex(const std::thread& newThread);
            const bool containsThread(const std::thread::id newThreadId) const;
            const bool getAcquired(void) const;
            const int getCeil(void) const;
            const int getMaxPriority(void) const;
            const int getThreadPriority(void) const;
            gtu::mutex* getAcquiredMutex(void) const;

        private:
            bool isAcquired = false;
            int ceil = -1;
            condition_variable lockConditionVariable;
            std::mutex lockMutex;
            vector<ThreadInfo> registeredThreads;
    };

    
}

#endif /* GTU_h */

//
//  GTU.cpp
//  Real Time - Mutex
//
//  Created by Dilara Karakaş
//

#include "GTU.h"


void gtu::mutex::lock(){
    if (!containsThread(this_thread::get_id()))
        cerr << "Lock Error! Thread has not been registered! : " << this_thread::get_id() << endl;
    else{
        gtu::mutex* acquiredMutex = getAcquiredMutex();
        if(acquiredMutex != nullptr){
            if (this->getThreadPriority() > acquiredMutex->getCeil()){
                std::mutex::lock();
                this->setAcquired(true);
            }
            else{
                unique_lock<std::mutex> conditionLock(lockMutex);
                lockConditionVariable.wait(conditionLock,[&]{ acquiredMutex = getAcquiredMutex(); return acquiredMutex == nullptr ? true : false; });
                std::mutex::lock();
                this->setAcquired(true);
            }
        }
        else{
            std::mutex::lock();
            this->setAcquired(true);
        }
    }
    cout << "Locked thread id : " << this_thread::get_id() << endl;
}

void gtu::mutex::unlock(){
    if (!containsThread(this_thread::get_id()))
        cerr << "Unlock Error! Thread has not been registered! : " << this_thread::get_id() << endl;
    else{
        for (int i = 0; i < this->registeredThreads.size(); ++i)
            if (this->registeredThreads[i].getRegisteredThreadId() == this_thread::get_id())
                this->registeredThreads.erase(this->registeredThreads.begin() + i);
        int maxPriority = this->getMaxPriority();
        this->setCeil(maxPriority);
        lockConditionVariable.notify_one();
        std::mutex::unlock();
        cout << "Unlocked thread id : " << this_thread::get_id() << endl;
        this->setAcquired(false);
    }
}

const bool gtu::mutex::containsThread(const thread::id newThreadId) const{
    for (int i = 0; i < this->registeredThreads.size(); ++i)
        if (this->registeredThreads[i].getRegisteredThreadId() == newThreadId)
            return true;
    return false;
}

const int gtu::mutex::getMaxPriority(void) const{
    int maxPriority = !this->registeredThreads.empty() ? this->registeredThreads[0].getRegisteredPriority() : -numeric_limits<int>::infinity();;
    for (int i = 1; i < this->registeredThreads.size(); ++i)
        if (this->registeredThreads[i].getRegisteredPriority() > maxPriority)
            maxPriority = this->registeredThreads[i].getRegisteredPriority();
    return maxPriority;
}

void gtu::mutex::registerThreadToMutex(const thread& newThread){
    gtu::ThreadInfo reg_thread;
    reg_thread.setRegisteredThreadId(newThread.get_id());
    reg_thread.addRegisteredMutexes(this);
    this->registeredThreads.push_back(reg_thread);
    int maxPriority = getMaxPriority();
    this->setCeil(maxPriority);
}

gtu::mutex* gtu::mutex::getAcquiredMutex(void) const{
    for (int i = 0; i < this->registeredThreads.size(); ++i)
        if (this->registeredThreads[i].getRegisteredThreadId() == this_thread::get_id())
            for (int j = 0; j < this->registeredThreads[i].getRegisteredMutexes().size(); ++j)
                if (this->registeredThreads[i].getRegisteredMutexes()[j]->getAcquired())
                    return this->registeredThreads[i].getRegisteredMutexes()[j];
    return nullptr;
}

const int gtu::mutex::getThreadPriority(void) const{
    for (int i = 0; i < this->registeredThreads.size(); ++i)
        if (this->registeredThreads[i].getRegisteredThreadId() == this_thread::get_id())
            return this->registeredThreads[i].getRegisteredPriority();
    return -1;
}

void gtu::mutex::setCeil(const int newCeil){
    this->ceil = newCeil;
}

void gtu::mutex::setAcquired(const bool newAcquired){
    this->isAcquired = newAcquired;
}

const bool gtu::mutex::getAcquired(void) const{
    return this->isAcquired;
}

const int gtu::mutex::getCeil(void) const{
    return this->ceil;
}

const int gtu::ThreadInfo::getRegisteredPriority(void) const{
    return this->regPriority;
}

const thread::id gtu::ThreadInfo::getRegisteredThreadId(void) const{
    return this->regThread_id;
}

const vector<gtu::mutex*> gtu::ThreadInfo::getRegisteredMutexes(void) const{
    return this->regMutexes;
}

void gtu::ThreadInfo::setRegisteredThreadId(const thread::id newThreadID){
    this->regThread_id = newThreadID;
}

void gtu::ThreadInfo::setRegisteredPriority(const int newPriority){
    this->regPriority = newPriority;
}

void gtu::ThreadInfo::addRegisteredMutexes(gtu::mutex* newMutex){
    this->regMutexes.push_back(newMutex);
}

int increment = 0;
bool condition = false;
condition_variable conditionVariableOfFunction;
mutex conditionVariableMutexOfFunctions;
gtu::mutex mutex1;
gtu::mutex mutex2;
gtu::mutex mutex3;

void gtu::startProtocol(void){
    condition = true;
    conditionVariableOfFunction.notify_all();
}

void gtu::setScheduleThread(thread& thisThread, int priorityOfThread){
    mutex1.registerThreadToMutex(thisThread);
    mutex2.registerThreadToMutex(thisThread);
    mutex3.registerThreadToMutex(thisThread);
}

void thread_1_function(void){
    unique_lock<mutex> lockThreadFunction1(conditionVariableMutexOfFunctions);
    conditionVariableOfFunction.wait(lockThreadFunction1, [&]{ return condition; });
    const unique_lock<gtu::mutex> lock1(mutex1);
    const lock_guard<gtu::mutex> lock2(mutex2);
    const lock_guard<gtu::mutex> lock3(mutex3);
    ++increment;
    cout << "First Function increased value is :\t" << increment << endl;
}

void thread_2_function(void){
    unique_lock<mutex> lockThreadFunction2(conditionVariableMutexOfFunctions);
    conditionVariableOfFunction.wait(lockThreadFunction2, [&]{ return condition;});
    const lock_guard<gtu::mutex> lock3(mutex3);
    const lock_guard<gtu::mutex> lock2(mutex2);
    const unique_lock<gtu::mutex> lock1(mutex1);
    ++increment;
    cout << "Second Function increased value is :\t" << increment << endl;
}


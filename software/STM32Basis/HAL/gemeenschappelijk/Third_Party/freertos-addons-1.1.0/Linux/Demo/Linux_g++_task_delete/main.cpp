/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
 *
 *  The FreeRTOS C++ Wrappers project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The FreeRTOS C++ Wrappers project is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the FreeRTOS C++ Wrappers project.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 ***************************************************************************/
#include <stdio.h>
#include <iostream>
#include "FreeRTOS.h"
#include "task.h"
#include "thread.hpp"
#include "ticks.hpp"


using namespace cpp_freertos;
using namespace std;



class TestThread : public Thread {

    public:

        TestThread(int i, int delayInSeconds)
           : Thread("TestThread", 100, 1), 
             id (i), 
             DelayInSeconds(delayInSeconds)
        {
            Start();
        };

        ~TestThread() {
            cout << "dtor for Thread " << id << endl;
        };

    protected:

        virtual void Run() {

            cout << "Starting thread " << id << endl;
            
            for (int i = 0; i < 3; i++) {
            
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
            
                if (ticks)
                    Delay(ticks);
            
                cout << "Running thread " << id << endl;
            }

            cout << "Exiting Thread " << id << endl;
        };

    private:
        int id;
        int DelayInSeconds;
};


class TestThreadForever : public Thread {

    public:

        TestThreadForever(int i, int delayInSeconds)
           : Thread("TestThreadForever", 100, 1), 
             id (i), 
             DelayInSeconds(delayInSeconds)
        {
        };

        ~TestThreadForever() {
            cout << "dtor for TestThreadForever " << id << endl;
        };

    protected:

        virtual void Run() {

            cout << "Starting TestThreadForever " << id << endl;
            
            while (true) {
            
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
            
                if (ticks)
                    Delay(ticks);
            
                cout << "Running TestThreadForever " << id << endl;
            }
        };

    private:
        int id;
        int DelayInSeconds;
};


class TestThreadKiller : public Thread {

    public:

        TestThreadKiller(Thread *thr1, Thread *thr2)
           : Thread("KillerThread", 100, 1),
             Thread1(thr1), Thread2(thr2) 
        {
        };

        ~TestThreadKiller() {
            cout << "dtor for KillerThread" << endl;
        };

    protected:

        virtual void Run() {

            cout << "Starting Killer thread" << endl;
            
            TickType_t ticks = Ticks::SecondsToTicks(20);
            
            if (ticks)
                Delay(ticks);
            
            delete Thread1;
            delete Thread2;
        };

    private:
        Thread *Thread1; 
        Thread *Thread2;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Simple Tasks" << endl;

    TestThread thread1(1, 1);
    TestThread thread2(2, 2);
    TestThread thread3(3, 3);
    
    TestThreadForever *threadA = new TestThreadForever(5, 5);
    TestThreadForever *threadB = new TestThreadForever(6, 6);
    TestThreadKiller Killer(threadA, threadB);
    
    //
    //  Try variations of Start outside of the ctor.
    //  
    threadA->Start();
    threadB->Start();
    Killer.Start();

    Thread::StartScheduler();

    //
    //  We shouldn't ever get here unless someone calls 
    //  Thread::EndScheduler()
    //

    cout << "Scheduler ended!" << endl;

    return 0;
}


void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
    printf("ASSERT: %s : %d\n", pcFileName, (int)ulLine);
    while(1);
}


unsigned long ulGetRunTimeCounterValue(void)
{
    return 0;
}

void vConfigureTimerForRunTimeStats(void)
{
    return;
}


extern "C" void vApplicationMallocFailedHook(void);
void vApplicationMallocFailedHook(void)
{
	while(1);
}



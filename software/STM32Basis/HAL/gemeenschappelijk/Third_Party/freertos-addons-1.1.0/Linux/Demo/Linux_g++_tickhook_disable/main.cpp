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
#include "tickhook.hpp"


using namespace cpp_freertos;
using namespace std;




class MyTickHook : public TickHook {

    public:
        MyTickHook(int id) : TickHook(), Id(id), Cnt(0)
        {
            Register();
        }

    protected:
        void Run() {

            if (++Cnt > 1000) {
                cout << "Running TickHook # " << Id << endl;
                Cnt = 0;
            }
        }

    private:
        int Id;
        int Cnt;
};


class MyThread : public Thread {

    public:

        MyThread(MyTickHook &h1, MyTickHook &h2, MyTickHook &h3)
           : Thread("MyThread", 100, 1),
             Hook1(h1), Hook2(h2), Hook3(h3)
        {
            Start();
        };

    protected:

        virtual void Run() {

            int DelayInSeconds = 3;
            int Count = 0;

            cout << "Starting thread" << endl;
            
            while (true) {
            
                TickType_t ticks = Ticks::SecondsToTicks(DelayInSeconds);
                Delay(ticks);

                switch (Count % 6) {
                    case 0:
                        cout << "Disabling Hook 1" << endl;
                        Hook1.Disable();
                        break;
                    case 1:
                        cout << "Disabling Hook 2" << endl;
                        Hook2.Disable();
                        break;
                    case 2:
                        cout << "Disabling Hook 3" << endl;
                        Hook3.Disable();
                        break;
                    case 3:
                        cout << "Enabling Hook 2" << endl;
                        Hook2.Enable();
                        break;
                    case 4:
                        cout << "Enabling Hook 1" << endl;
                        Hook1.Enable();
                        break;
                    case 5:
                        cout << "Enabling Hook 3" << endl;
                        Hook3.Enable();
                        break;
                }

                Count++;
            }
        };

    private:
        MyTickHook &Hook1;
        MyTickHook &Hook2;
        MyTickHook &Hook3;
};


int main (void)
{
    cout << "Testing FreeRTOS C++ wrappers" << endl;
    cout << "Simple Tasks" << endl;

    MyTickHook hook1(1);
    MyTickHook hook2(2);
    MyTickHook hook3(3);

    MyThread thr(hook1, hook2, hook3);

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



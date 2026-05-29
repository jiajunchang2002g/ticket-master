#include "TicketSystem.h"
#include <iostream>
#include <thread>

// Demo
int main() {
        TicketSystem system(3);

        std::thread t1([&]() {
                        system.reserveSeat(1, 0, 3);
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        system.confirmSeat(0, 1);
                        });

        std::thread t2([&]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        system.reserveSeat(2, 0); // should fail
                        });

        std::thread t3([&]() {
                        system.reserveSeat(3, 1, 2); // will expire
                        });

        t1.join();
        t2.join();
        t3.join();

        std::this_thread::sleep_for(std::chrono::seconds(5));

        return 0;
}

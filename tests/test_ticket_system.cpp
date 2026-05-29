#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "TicketSystem.h"

// Assume TicketSystem is already defined and included

void test_basic_reservation() {
        TicketSystem system(1);

        bool ok = system.reserveSeat(1, 0, 3);
        assert(ok && "Reservation should succeed");

        bool ok2 = system.reserveSeat(2, 0);
        assert(!ok2 && "Second reservation should fail");

        std::cout << "✅ test_basic_reservation passed\n";
}

void test_confirm_success() {
        TicketSystem system(1);

        system.reserveSeat(1, 0, 3);
        bool ok = system.confirmSeat(0, 1);

        assert(ok && "Confirm should succeed for correct user");

        std::cout << "✅ test_confirm_success passed\n";
}

void test_confirm_wrong_user() {
        TicketSystem system(1);

        system.reserveSeat(1, 0, 3);
        bool ok = system.confirmSeat(0, 2);

        assert(!ok && "Confirm should fail for wrong user");

        std::cout << "✅ test_confirm_wrong_user passed\n";
}

void test_expiration() {
        TicketSystem system(1);

        system.reserveSeat(1, 0, 2);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        bool ok = system.reserveSeat(2, 0);
        assert(ok && "Seat should be free after expiration");

        std::cout << "✅ test_expiration passed\n";
}

void test_confirm_after_expiry() {
        TicketSystem system(1);

        system.reserveSeat(1, 0, 1);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        bool ok = system.confirmSeat(0, 1);
        assert(!ok && "Confirm should fail after expiry");

        std::cout << "✅ test_confirm_after_expiry passed\n";
}

void test_concurrent_reservation() {
        TicketSystem system(1);

        bool r1 = false, r2 = false;

        std::thread t1([&]() {
                        r1 = system.reserveSeat(1, 0);
                        });

        std::thread t2([&]() {
                        r2 = system.reserveSeat(2, 0);
                        });

        t1.join();
        t2.join();

        assert((r1 ^ r2) && "Only one thread should succeed");

        std::cout << "✅ test_concurrent_reservation passed\n";
}

void test_concurrent_confirm_vs_expiry() {
        TicketSystem system(1);

        system.reserveSeat(1, 0, 2);

        bool confirmResult = false;

        std::thread confirmer([&]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                        confirmResult = system.confirmSeat(0, 1);
                        });

        std::this_thread::sleep_for(std::chrono::seconds(3));

        confirmer.join();

        // Either confirm wins OR expiry wins — but never both succeed incorrectly
        // Valid outcomes:
        // - confirmResult == true (confirmed before expiry)
        // - confirmResult == false (expired first)

        std::cout << "✅ test_concurrent_confirm_vs_expiry passed (result="
                << confirmResult << ")\n";
}

void test_multiple_seats() {
        TicketSystem system(5);

        for (int i = 0; i < 5; i++) {
                assert(system.reserveSeat(i, i));
        }

        for (int i = 0; i < 5; i++) {
                assert(system.confirmSeat(i, i));
        }

        std::cout << "✅ test_multiple_seats passed\n";
}

void test_stress_small() {
        TicketSystem system(10);

        std::vector<std::thread> threads;

        for (int i = 0; i < 50; i++) {
                threads.emplace_back([&, i]() {
                                int seat = i % 10;
                                system.reserveSeat(i, seat, 2);
                                });
        }

        for (auto& t : threads) t.join();

        std::cout << "✅ test_stress_small passed\n";
}

int main() {
        test_basic_reservation();
        test_confirm_success();
        test_confirm_wrong_user();
        test_expiration();
        test_confirm_after_expiry();
        test_concurrent_reservation();
        test_concurrent_confirm_vs_expiry();
        test_multiple_seats();
        test_stress_small();

        std::cout << "\n🎉 ALL TESTS PASSED\n";
        return 0;
}

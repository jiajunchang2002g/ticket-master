#include "TicketSystem.h"

#include <iostream>

namespace {
        void confirmPayment(int userID) {
                std::cout << "Payment confirmed for user " << userID << "\n";
        }

        void refundUser(int userID) {
                std::cout << "Refund issued to user " << userID << "\n";
        }

        enum class SeatState {
                Free,
                Reserved,
                Sold
        };
} // namespace

struct TicketSystem::Seat {
        std::mutex mut;
        int heldBy;
        SeatState state;
        std::chrono::steady_clock::time_point expiry;

        Seat() : heldBy(-1), state(SeatState::Free) {}
};

struct TicketSystem::ExpiryEvent {
        int seatID;
        std::chrono::steady_clock::time_point expiry;

        bool operator>(const ExpiryEvent& other) const {
                return expiry > other.expiry;
        }
};

TicketSystem::TicketSystem(int numSeats)
        : seats(static_cast<size_t>(numSeats)),
        running(true) {
                std::cout << "Ticket System constructor called\n";
                worker = std::thread(&TicketSystem::expirationWorker, this);
        }

TicketSystem::~TicketSystem() {
        {
                std::lock_guard<std::mutex> lock(queueMutex);
                running = false;
        }
        cv.notify_all();

        if (worker.joinable()) {
                worker.join();
        }
        // join expirationWorker 

        std::cout << "Ticket System destructor called\n";
}

bool TicketSystem::reserveSeat(int userID, int seatID, int holdSeconds) {
        if (seatID < 0 || seatID >= static_cast<int>(seats.size())) {
                return false;
        }

        auto& seat = seats[static_cast<size_t>(seatID)];
        auto expiry = std::chrono::steady_clock::now() +
                std::chrono::seconds(holdSeconds);

        {
                std::lock_guard<std::mutex> lock(seat.mut);
                if (seat.state != SeatState::Free) {
                        return false;
                }

                seat.state = SeatState::Reserved;
                seat.heldBy = userID;
                seat.expiry = expiry;
        }

        {
                std::lock_guard<std::mutex> lock(queueMutex);
                expiryQueue.push({seatID, expiry});
        }

        cv.notify_one();

        std::cout << "Seat " << seatID << " reserved by user " << userID << "\n";
        return true;
}

bool TicketSystem::confirmSeat(int seatID, int userID) {
        if (seatID < 0 || seatID >= static_cast<int>(seats.size())) {
                return false;
        }

        bool success = false;
        auto now = std::chrono::steady_clock::now();

        {
                std::lock_guard<std::mutex> lock(seats[static_cast<size_t>(seatID)].mut);
                auto& seat = seats[static_cast<size_t>(seatID)];

                if (seat.state == SeatState::Reserved &&
                                seat.heldBy == userID &&
                                now < seat.expiry) {
                        seat.state = SeatState::Sold;
                        success = true;

                        std::cout << "Seat " << seatID << " sold to user " << userID << "\n";
                }
        }

        if (!success) {
                refundUser(userID);
        }

        return success;
}

bool TicketSystem::processPayment(int userID, int seatID) {
        (void)seatID;
        confirmPayment(userID);
        return true;
}

void TicketSystem::expirationWorker() {
        std::unique_lock<std::mutex> lock(queueMutex);

        while (running) {
                if (expiryQueue.empty()) {
                        cv.wait(lock, [&]() {
                                        return !running || !expiryQueue.empty();
                                        });
                        continue;
                }

                auto next = expiryQueue.top();
                auto now = std::chrono::steady_clock::now();

                if (now < next.expiry) {
                        cv.wait_until(lock, next.expiry);
                        continue;
                }

                expiryQueue.pop();

                lock.unlock();
                handleExpiry(next);
                lock.lock();
        }
}

void TicketSystem::handleExpiry(const ExpiryEvent& event) {
        int seatID = event.seatID;
        if (seatID < 0 || seatID >= static_cast<int>(seats.size())) {
                return;
        }

        std::lock_guard<std::mutex> lock(seats[static_cast<size_t>(seatID)].mut);
        auto& seat = seats[static_cast<size_t>(seatID)];

        if (seat.state == SeatState::Reserved && seat.expiry == event.expiry) {
                std::cout << "Seat " << seatID
                        << " expired for user " << seat.heldBy << "\n";

                seat.state = SeatState::Free;
                seat.heldBy = -1;
        }
}

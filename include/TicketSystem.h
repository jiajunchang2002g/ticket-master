#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class TicketSystem {
public:
    TicketSystem(int numSeats);
    ~TicketSystem();

    bool reserveSeat(int userID, int seatID, int holdSeconds = 5);
    bool confirmSeat(int seatID, int userID);
    bool processPayment(int userID, int seatID);

private:
    struct Seat;
    struct ExpiryEvent;
    std::vector<Seat> seats;

    std::priority_queue<
        ExpiryEvent,
        std::vector<ExpiryEvent>,
        std::greater<ExpiryEvent>
    > expiryQueue;

    std::mutex queueMutex;
    std::condition_variable cv;

    std::atomic<bool> running;
    std::thread worker;

    void expirationWorker();
    void handleExpiry(const ExpiryEvent& event);
};

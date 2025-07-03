//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef HANDLER_H
#define HANDLER_H
#include <thread>

class ThreadHandler {
protected:
    std::thread action_thread;
    std::atomic<bool> running{false};

    virtual void do_stuff() = 0;

    void action_loop() {
        while (running.load(std::memory_order_relaxed)) {
            do_stuff();
        }
    }

public:
    virtual ~ThreadHandler() { ThreadHandler::stop_handler();};

    void start_handler() {
        if (running.load(std::memory_order_relaxed)) return;
        running.store(true, std::memory_order_relaxed);
        action_thread = std::thread([this]() { action_loop(); });
    }

    virtual void stop_handler() {
        if (!running.load()) return;
        running.store(false);

        if (action_thread.joinable()) {
            action_thread.join();
        }
    };
};

#endif //HANDLER_H

//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef FEEDHANDLER_H
#define FEEDHANDLER_H
#include <boost/lockfree/queue.hpp>

#include "OrderBookHandler.h"
#include "ThreadHandler.h"
#include "../feeds/Websocket.h"

using queue = boost::lockfree::queue<BookUpdate>;

class FeedHandler final : public ThreadHandler {

    queue &update_queue;

    io_context &ioc;
    ssl_context &soc;
    std::unique_ptr<Websocket> ws;

    const std::string sub_msg;

    const std::string host;
    const std::string port;
    const std::string target;

    void do_stuff() override;

public:
    FeedHandler(queue &update_queue, io_context &ioc, ssl_context &soc, const std::string &host, const std::string &port, const std::string &target, const std::string& sub_msg);
    ~FeedHandler() override;
    void stop_handler() override;
};



#endif //FEEDHANDLER_H

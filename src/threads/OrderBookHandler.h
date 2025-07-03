//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef ORDERBOOKHANDLER_H
#define ORDERBOOKHANDLER_H

#include "ThreadHandler.h"
#include "../orderbook/OrderBook.h"
#include "boost/lockfree/queue.hpp"

struct BookUpdate {
    int64_t check_against_snapshot;
    bool is_ask;
    double price;
    double quantity;
};

using update_queue = boost::lockfree::queue<BookUpdate>;

class OrderBookHandler final : public ThreadHandler {

    OrderBook order_book;
    update_queue &queue_in;
    int64_t inital_snapshot_id;

    void do_stuff() override;

public:
    explicit OrderBookHandler(update_queue &queue_in);
    void initialise_order_book(const std::string& url);
    ~OrderBookHandler();
};

#endif //ORDERBOOKHANDLER_H

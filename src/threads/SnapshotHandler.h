//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef SNAPSHOTHANDLER_H
#define SNAPSHOTHANDLER_H
#include <memory>
#include <thread>

#include "ThreadHandler.h"
#include "../orderbook/OrderBook.h"

class SnapshotHandler : public ThreadHandler {
    std::shared_ptr<OrderBook> order_book;
};



#endif //SNAPSHOTHANDLER_H

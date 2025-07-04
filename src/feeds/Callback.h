//
// Created by Yeo Shu Heng on 4/7/25.
//

#ifndef CALLBACK_H
#define CALLBACK_H

#include <functional>
#include <boost/beast.hpp>
#include "../threads/OrderBookHandler.h"

namespace beast = boost::beast;

std::function<void(const beast::error_code&)> build_error_handler();

std::function<void(const std::string_view&)> build_resp_handler(boost::lockfree::spsc_queue<BookUpdate>& update_queue);

#endif //CALLBACK_H

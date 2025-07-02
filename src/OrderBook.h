//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include <array>
#include <span>

constexpr size_t ORDERBOOK_MAX_DEPTH = 10;

struct BookLevel {
    int32_t price;
    int64_t quantity;
};

class OrderBook {

    /**
     * Represents bids, descending in nature.
     */
    std::array<BookLevel, ORDERBOOK_MAX_DEPTH> bid;

    /**
     * Represents asks, ascending in nature.
     */
    std::array<BookLevel, ORDERBOOK_MAX_DEPTH> ask;

    int bid_size;
    int ask_size;

    static void update_side(std::array<BookLevel, 10> &side, int &side_size, int32_t price, int64_t quantity, bool is_ask);

    /**
     * Checks if update can be inserted in the first place.
     */
    static bool is_updated_not_needed(const std::array<BookLevel, 10> &side, int side_size, int32_t price, int64_t quantity, bool is_ask);

    /**
     * Returns the position of the target price or the index, which is the smallest price greater than (ask) or biggest price smaller than (bid).
     * This is the position to apply updates to.
     */
    static int search_side(int target_price, int side_size, const std::array<BookLevel, ORDERBOOK_MAX_DEPTH>& side, bool is_ask);

    /**
     * Updates the order book if incoming price is already a level on the book.
     */
    static void found_price_update_side(int index_to_update, std::array<BookLevel, 10> &side, int &side_size, int64_t quantity, bool is_ask);

    /**
     * Updates the order book if incoming price does not exist on the book.
     */
    static void not_found_price_update_side(int index_to_update, std::array<BookLevel, 10> &side, int &side_size, int32_t price, int64_t quantity, bool is_ask);

public:

    OrderBook();

    /**
     * Update functions for the order book.
     */
    void update_bid(int32_t price, int64_t quantity);
    void update_ask(int32_t price, int64_t quantity);

    /**
     * Getters for span of bid & ask sides.
     */
    [[nodiscard]] std::span<const BookLevel> get_bid() const;
    [[nodiscard]] std::span<const BookLevel> get_ask() const;

    /**
     * Getters for book-level data like sizes & L1
     */
    int get_bid_size() const;
    int get_ask_size() const;
    int get_spread() const;
    int get_mid_price() const;
    int get_top_bid() const;
    int get_top_ask() const;
};


#endif //ORDERBOOK_H

//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include <array>
#include <span>

constexpr size_t ORDERBOOK_MAX_DEPTH = 10;

class OrderBook {

    /**
     * Represents bids, descending in nature.
     */
    std::array<float, ORDERBOOK_MAX_DEPTH> bid_price;
    std::array<float, ORDERBOOK_MAX_DEPTH> bid_qty;

    /**
     * Represents asks, ascending in nature.
     */
    std::array<float, ORDERBOOK_MAX_DEPTH> ask_price;
    std::array<float, ORDERBOOK_MAX_DEPTH> ask_qty;

    int bid_size;
    int ask_size;

    static void update_side(std::array<float, 10> &side_price, std::array<float, 10> &side_qty, int &side_size, float price, float quantity, bool
                            is_ask);

    /**
     * Checks if update can be inserted in the first place.
     */
    static bool is_updated_not_needed(const std::array<float, 10> &side, int side_size, float price, float quantity, bool is_ask);

    /**
     * Returns the position of the target price or the index, which is the smallest price greater than (ask) or biggest price smaller than (bid).
     * This is the position to apply updates to.
     */
    static int search_side(float target_price, int side_size, const std::array<float, ORDERBOOK_MAX_DEPTH>& side_price, bool is_ask);
    static int search_side_linear(float target_price, int side_size, const std::array<float, ORDERBOOK_MAX_DEPTH>& side_price, bool is_ask);

    /**
     * Updates the order book if incoming price is already a level on the book.
     */
    static void found_price_update_side(int index_to_update, std::array<float, 10> &side_price, std::array<float, 10> &side_qty, int &side_size, float
                                        quantity, bool is_ask);

    /**
     * Updates the order book if incoming price does not exist on the book.
     */
    static void not_found_price_update_side(int index_to_update, std::array<float, 10> &side_price, std::array<float, 10> &side_qty, int &side_size, float price, float
                                            quantity, bool is_ask);

public:

    OrderBook();

    /**
     * Update functions for the order book.
     */
    void update_bid(float price, float quantity);
    void update_ask(float price, float quantity);

    /**
     * Display functions
     */
    [[nodiscard]] std::string to_string() const;


    /**
     * Getters for book-level data like sizes & L1
     */
    float get_bid_size() const;
    float get_ask_size() const;
    float get_spread() const;
    float get_mid_price() const;
    float get_top_bid() const;
    float get_top_ask() const;
};


#endif //ORDERBOOK_H

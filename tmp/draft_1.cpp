/* DRAFT 1
this is right before i started coding the main OrderBook class
- have not changed the implementation to use vector over list for the DS that holds our orders `OrderPointers`
- there is the OrderBook class to be done in TODO:
*/

// orderbook in cpp

// ------------------------------------------------------------------------------------------------
// Requirements:
// order types that are supported:
// - good 'til canceled (GTC) order types => buy/ sell orders that are there
// till the user cancels them
// - fill & kill order type => generally set higher than the curr price (being
// aggressive) so that u can fill the order
// => for buying, set limit price slightly above "asking" price
// => for selling, set limit price slightly below "bidding" price
// * this is generally done to capture liquidity

// additionally, build robust tesing 100s of different orders

// ------------------------------------------------------------------------------------------------
// headers
#include <algorithm>  // for things like sort
#include <unordered_map>
#include <vector>
#include <list>
// #include <format>  // for std::format => used in cpp 20+, apple clang doesnt support yet
#include <fmt/core.h>
#include <stdexcept> // to throw std::logic_error

// ------------------------------------------------------------------------------------------------
// enum classes
enum class OrderType {
    GoodTillCancel,
    FillAndKill
};

enum class Side {
    Buy,
    Sell
};

// ------------------------------------------------------------------------------------------------
// alias
// alias common domain types => give meaning to types & centralize decisions
// about them
using Price = std::int32_t;
// prices can be negative => electricity markets where producers pay you to
// consume electricity prices usually stored in ticks not floating point =>
// price of 152.34 stored as 15234 => max price in ticks generally
// 42'949'672'000 in ticks
// => way above any realistic asset price
using Quantity = std::uint32_t;  // qty is non negative
// in general we use 32 bits (4 bytes)
using OrderId = std::uint64_t;
// matching engines on the other hand keep millions of orders in RAM => can
// maybe have 10 mil orders (10mil * 8 bytes = 80mil bytes = 80MB) order come in
// quickly ~1 mil orders/s AND orders need to be stored for a long time =>
// within 4000 seconds == 1.1 hours, will overflow _t suffix => means type, the
// entire thing uint32_t means fixed width int => # of bits is guaranteed to be
// same on all platforms

// ------------------------------------------------------------------------------------------------
// Level info (heap vs level based design)
// orderbook is typically designed this way for real exchange style books
// heap based => optimized for quick finding of the best bid/ best ask
// level based desgin => designed for managing all orders, at each price,
// preserving FIFO? & tracking volume?

// generally heap just tells u best price currently
// level info guarantees:
// (1) price time priority: cant maintain orders within the same price (no FIFO
// ordering) => exchange need earlier order gets priority (price-time priority)
// (2) total quantity per level: with heap u need to run through all the orders
// at that price to find out (3) efficient cancellation: heaps usually just mark
// order as cancelled, then when popping, will skip => but takes up MEM
//
/*
rough visualization:
Bids:
100 -> [order1, order2, order3]
99  -> [order4]
98  -> [order5, order6]

Asks:
101 -> [order7, order8]
102 -> [order9]
*/

/* use a combination of both:
bid_heap = []
ask_heap = []
bid_levels = {
    100: LevelInfo(...),
    99: LevelInfo(...)
}
ask_levels = {
    100: LevelInfo(...),
    99: LevelInfo(...)
}

eventually u can:
bid_levels[idx]:
    total_qty
    orderes = [A, B, ...]

so previously, cancellation is either u
- log(n) to find the exact order in the heap
- O(1) but have space overhead

but now, cancellation could be
- O(n) where we iterate through the linked list in the orders to remove
- O(1) in set, BUT no FIFO property
- O(1) doubly linked list => BEST (provides price time priority orders)
*/
struct LevelInfo {
    Price price_;  // need a price for that level
    Quantity Quantity_;  // need a total qty for that lvl
    // var_name_ => "_" helps to identify member variable, if without then u
    // know its a parameter maybe both price_ & quantity_ represent member
    // variables
};

using LevelInfos = std::vector<LevelInfo>;  // set this vector of LevelInfo as a type too

class OrderBookLevelInfos {
private:
    LevelInfos bids_;
    LevelInfos asks_;

public:
    OrderBookLevelInfos(const LevelInfos& asks, const LevelInfos& bids)  // constructor signature on the 1st line
        // : starts the initializer list, each member initializer on a new aligned line
        : bids_(bids),
          asks_(asks) {}

    const LevelInfos& get_bids() const { return bids_; }
    // 1st const means the vector is read only
    // => cant modify the vector i.e. append
    // => cant clear
    // cant replace element i.e. bids[0] = LevelInfo()
    // ALSO cant be changed when accessing via this reference
    // i.e. bids[0].price = 10; NOT ALLOWED
    // const reference is really a strict constant where the container nor the value inside the container cant be changed!

    // 2nd const means that u are not allowed to modify class members inside the fn (i.e. u cant modify _bids in this case)
    // => if 1st const not around but 2nd is, caller can modify but callee cant
    // => how does the compiler do this? it make the entire (this) object constant => so none of the class members can be modified
    const LevelInfos& get_asks() const { return bids_; }
};

// ------------------------------------------------------------------------------------------------
// Order entity

// struct? => initially thought struct would be good since i jsut need a simple data DS, but realise there is other functions that are useful
// struct Order {  // use a struct for now since i dont see a need for it to be private nor have methods
//     OrderId id;
//     OrderType type;
//     Price price;
//     Quantity qty;
//     Side side;
//     Quantity filled_qty;  // use a quantity so we can check if its equal
// };
// maybe can consider to have a is_filled method?
    // get_remaining_qty? => qty remaining to be filled
    // get_filled_qty? => qty that has already been filled
    // get order id
    // get side
    // get order type
    // get initial qty?? => some apis pblish the initial qty of the order => can consider to implement => for book keeping purposes

// to create an order:
// parantheses initialization: Order new_order(order_type, order_id, side, price, quantity);
// brace initialization: Order new_order{order_type, order_id, side, price, quantity};
    // prevents narrowing conversions
    // avoids most vexing parse issue
// heap allocation: Order* new_order = new Order(...)
    // returns a pointer

// copy initialization: Order new_order = Order(order_type, order_id, side, price, quantity);
    // construct a temp Order via Order(order_type, order_id, side, price, quantity);
    // then move that temp into new_order
// copy ellision => compiler skips the temp & directly constructs a new order in new_order
    // Order new_order(...)
    // does "direct initialization"

class Order {
public:
    Order(OrderType order_type, OrderId order_id, Side side, Price price, Quantity quantity)
        : type_{order_type},
          id_{order_id},
          side_{side},
          price_{price},
          initial_quantity_{quantity},
          remaining_quantity_{quantity} {}
    
    // get_type
    // get_id
    // get_side
    // get_price
    // get_initial_qty
    // get_remaining_qty
    // get_filled_qty
    // is_filled
    // fill => to fill an order with some qty (need qty < remaining_qty)
    OrderType get_type() const { return type_; }
    OrderId get_id() const { return id_; }
    Side get_side() const { return side_; }
    Price get_price() const { return price_; }
    Quantity get_initial_quantity() const { return initial_quantity_; }
    Quantity get_remaining_quantity() const { return remaining_quantity_; }
    Quantity get_filled_quantity() const { return initial_quantity_ - remaining_quantity_; }
    bool is_filled() const { return remaining_quantity_ == 0; }

    void fill(Quantity quantity) {
        if (quantity > get_remaining_quantity()) {
            throw std::logic_error(fmt::format("ERROR: Order {} CANT be filled. to fill qty: {} more than remaining qty: {}", get_id(), quantity, get_remaining_quantity()));
        }
        remaining_quantity_ -= quantity;
    }

private:
    OrderType type_;
    OrderId id_;
    Side side_;
    Price price_;
    Quantity initial_quantity_;
    Quantity remaining_quantity_;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

class OrderModfiy {
public:
    OrderModfiy(OrderId order_id, Side side, Price price, Quantity quantity)
        : order_id_{order_id},
          price_,
          side_,
          quantity_ {}
    OrderId get_id() const { return order_id_; }
    Price get_price() const { return price_; }
    Side get_side() const { return side_; }
    Quantity get_quantity() const { return quantity_; }

    OrderPointer to_order_pointer(OrderType type) const {
        return std::make_shared<order>(type, get_id(), get_side(), get_price(), get_quantity())
    }

private:
    OrderId id_;
    Price price_;
    Side side_;
    Quantity quantity_;
};

class TradeInfo {
    OrderId id_;
    Price price_; // this price refers to execution at rest price (the price already in the orderbook)
    Quantity quantity_;
};

class Trade {
public:
    Trade(const TradeInfo& bid_trade, const TradeInfo& ask_trade)
        : bid_trade_{bid_trade},
          ask_trade_{ask_trade} {}
    const TradeInfo& get_bid_trade const { return bid_trade_; }
    const TradeInfo& get_ask_trade const { return ask_trade_; }

private:
    TradeInfo bid_trade_;
    TradeInfo ask_trade_;
};

class Orderbook {
    // TODO: 
};

// ------------------------------------------------------------------------------------------------
int main() {
    return 0;
}

# notes
notes taken while doing this project
# orderbook: purpose
# orderbook: architecture
# orderbook: buy & sell
# orderbook: "price time priority" & "execution at rest price"

# -------------------------------------------------------------------------------------------------
# orderbook: purpose

The purpose of the order book is to receive incremental updates which reflect the current state of the exchange so that the trader knows how to devise strategy for trading.

# -------------------------------------------------------------------------------------------------
# orderbook: architecture
will be using:
- ordered_map <price, list> => to get the min/ max price (heap behavior) from the asks/ bids
- list for orders => in cpp is implemented as doubly linked list => allows for me to remove / add in O(1) 
    - will implement using vector like in production systems => cache locality advantage
- classes for commands => allow for message queueing & async distributed nature of receiving & disseminating orders for my market disseminator later on
- class for orders => (just makes sense to encapsulate in an entity)

# -------------------------------------------------------------------------------------------------
# orderbook: buy & sell
=> i always forget which is max/ min heap
=> to think about it:
    - when buying => i always want the min price to pay for (hence asks => min heap)
    - when selling => i always want the max price i can get for my product (hence bids => max heap)

creating a map in cpp:
```cpp
#include <map>
std::map<KeyType, ValueType> m;
// default way (min heap map)
std::map<int, LevelInfos> level_info_map;
// min heap map
std::map<int, LevelInfo, std::less<int>> level_info_min_map;
// max heap map
std::map<int, LevelInfo, std::greater<int>> level_info_max_map;
auto it = level_info_min_map.begin() // returns the iterator to the min value  std::map<int, LevelInfos>::iterator
it->first // returns key
it->second // returns val
// *iterator points to an element in the map => points to a pair std::pair<const int, LevelInfo>
// iterator defines an operator->() member function to do essentially the same thing as pointers when they dereference from the address & get a member value
// (*it).first == it->first

// full template signature of map
template<
    class Key, // class of the key
    class T, // class of the value
    class Compare = std::less<Key>, // comparison of the key
    class Allocator = std::allocator<std::pair<const Key, T>>
> class map;

```

# -------------------------------------------------------------------------------------------------
# orderbook: "price time priority" & "execution at rest price"
- "price time priority" -> priority of orders executed is based on price first, then to break even, its which order came first
- "execution at rest price" -> when an order comes in to be filled, its executed/ filled at the resting order's price


# notes
notes taken while doing this project

# architecture
# implicit instantiation of undefined template
# name shadowing
# buy & sell
# styling cpp code

# -------------------------------------------------------------------------------------------------
# architecture
will be using:
- ordered_map <price, list> => to get the min/ max price (heap behavior) from the asks/ bids
- list for orders => in cpp is implemented as doubly linked list => allows for me to remove / add in O(1) 
    - will implement using vector like in production systems => cache locality advantage
- classes for commands => allow for message queueing & async distributed nature of receiving & disseminating orders for my market disseminator later on
- class for orders => (just makes sense to encapsulate in an entity)

# -------------------------------------------------------------------------------------------------
# implicit instantiation of undefined template
ISSUE: encountered the red squibbly error in compilation: "implicit instantiation of undefined template"
std namespace is defined across many headers, not just 1
each header adds specific components into std namespace
i.e. <vector> adds std::vector to std namespace
templates live in headerse coz the compiler must see the full implementation when generating types
but if the <vector> template was not included, the std namespace would not know vector exists maybe `template<class T> class vector { ... };` but not the entire definition

* sometimes there are forward declarations in other header files via `#include <something>` hence it throws the error "implicit instantiation of undefined template"

SOLUTION: #include <vector>

# -------------------------------------------------------------------------------------------------
# name shadowing
when we actually need to use `this` keyword

```cpp
class OrderBook {
    int x;
public:
    void setX(int x) {
        this->x = x;
    }
};
```

# -------------------------------------------------------------------------------------------------
# buy & sell
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
# styling cpp code
common cpp style
- (1) closing brace should align with the constructor
- (2) body should be indented 1 lvl 
- (3) comments to not be indented
- * just a note that sometimes u see the comments move by 1 space => its coz clang format enforces `SpacesBeforeTrailingComments: 2` and is generally practiced by the industry to make comments more readable
- * sadly my .clang-format for the curly brace formatting is executed after the save

```cpp
class SomeClass{
public:
    OrderBookLevelInfos(const LevelInfos& asks, const LevelInfos& bids)
        : bids_(bids),  // initialize bids => (3)
          asks_(asks) { // (1)
        // test (2)
        some_function();
    }
};
```

# -------------------------------------------------------------------------------------------------
# "price time priority" & "execution at rest price"
- "price time priority" -> priority of orders executed is based on price first, then to break even, its which order came first
- "execution at rest price" -> when an order comes in to be filled, its executed/ filled at the resting order's price
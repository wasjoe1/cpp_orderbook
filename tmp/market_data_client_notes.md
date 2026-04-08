# NOTES
Notes taken to build the market data client.

## Resources (market data feed + order book + GPT prompts)
## Coinbase APIs
## Actions
## Networking notes

# -------------------------------------------------------------------------------------------------
## Resources (market data feed + order book + GPT prompts)
### Market data feed resources
As practice these were the market data providers i was considering (Closest to HFT reality):
1. Nasdaq TotalView-ITCH style data
2. Coinbase Exchange Full / Level3
3. Binance diff depth

* `Nasdaq TotalView-ITCH style data` is not real time market data => would need to create a replay engine to mimic/ mock real time market data feed
    * for context traditional exchanges often use FIX (order entry) + FAST/ ITCH (market data feed)
    * modern exchanges uses `binary order entry protocols` for order entry that are faster than FIX (i.e. nasdaq(OUCH), CME (iLink), NYSE(Pillar), etc.)
* coinbase is real time market data feed but uses websocket connection


### Order book resources
good orderbook resource: https://github.com/enewhuis/liquibook
- according to its benchmarks, it is able to handle __2.0 million__ to __2.5 million__ inserts per second.
- i havent really used it yet, but will probably refer to its rough structure to give me insight to best practices for a production grade orderbook

# -------------------------------------------------------------------------------------------------
## Coinbase APIs
Coinbase provides a few API, & for building the market data client, i am referring to their __Coinbase exchange API__ (not CDP APIs -> those are for on chain dev)
Coinbase exchange API provides 2 feeds:
1. __websocket feed__ => used this
2. FIX feed API

i am referring to the websocket feed for market data
it provides real-time market data updates for orders and trades.
Within websocket feed, there are 2 endpoints:
    1. `Coinbase Market Data` is our traditional feed which is available without authentication.
    2. `Coinbase Direct Market Data` has direct access to Coinbase Exchange servers and requires Authentication.
* will try to use the direct market data feed to practice __authentication__ & also be able to __benchmark latency requirements__

Coinbase Direct Market Data:
production = wss://ws-direct.exchange.coinbase.com
sandbox = wss://ws-direct.sandbox.exchange.coinbase.com

### GPT prompts:
planning the project & figuring out resource: https://chatgpt.com/share/69b573a1-af20-800a-b398-63c0cb2ca6d6

# -------------------------------------------------------------------------------------------------
## Actions
some actions include:
- subscribe
- unsubscribe
- specify product IDs (for channels)
- subscriptions - list all channels u are subscribed to

# -------------------------------------------------------------------------------------------------
## Networking notes
- websocket compression extension
    - websocket compression defined in RFC 7692 => can compress messages to receive higher throughput -> reduce latency
- sequence numbers
    - if msg has seq num difference more than 1 => msg was dropped
    - msg seq num less than curr => can safely ignore

# -------------------------------------------------------------------------------------------------
## Minimal version

- connect to exchange
- subscribe to 1 product
- subscribe to 1 channel (level 3)
- receive messages?
- maintain local order book
- let the user query the current order book snapshot

connect()
disconnect()
subscribe(channel, product_id)
unsubscribe(channel, product_id)
get_order_book(product_id)          -> what if i have 2 different channels for the same product? (i.e. (level2, ETH), (level3, ETH))
is_connected()

* different channels, same product
    - u can actually derive lower level order books from higher levels; all levels are just diff views of the same underlying level-3 orderbookso 
    - level3 -aggregate-> level2 -
        * full <price: list of orders>
        * level3 <price: list of orders>
        * level2 <price: total size>
        * level top-of-book only (best bid/ask price/ size)
    - what most systems do is to subscribe to L3 feed -> then derive L2 & L3 themselves, rather than subscribing to the L1 & 2 feeds as well

# -------------------------------------------------------------------------------------------------
## level 3 vs full channel
- for coinbase, both level 3 & full channel send same data
- for coinbase, both level 3 & full channel send incremental updates
- difference is purely message format
    => full channel: sends in JSON objects with named fields (better readability)
    => level 3: send in compact array
        -> need to parse a schema message first to understand the array positions
        -> better for performance & bandwidth efficiency
* chose to subscribe to level 3 orderbook

# -------------------------------------------------------------------------------------------------
# Architecture
thread 1: main thread handling user interaction
thread 2: market data/ websocket thread => handle message stream + parsing


# TradeExecution Service
Designing a high performance service to execute a TRADE.


## Complete Repo Structure
```
TradeExecution
├── CMakeLists.txt
├── conanfile.txt
├── Dockerfile
├── src/
│   ├── main.cpp
│   ├── Order.h
│   ├── ExecutionEngine.h
│   ├── ExecutionEngine.cpp
│   ├── TradingMicroservice.h
│   └── TradingMicroservice.cpp
```


### Build service as docker

`docker build -t trading-microservice .`


### Run app in docker
`docker run -d -p 9080:9080 --name trade_app trading-microservice`


### Time to TRADE :-), let's buy somee apple share cheaply
```
panpaliamahen@Mahendras-MacBook-Air TradeExecution % curl -X POST -H "Content-Type: application/json" -d '{"symbol": "AAPL", "side": "BUY", "quantity": 10}' http://localhost:9080/trade/market

{"executed_price":180.5,"order_id":"1764608000093","quantity":10,"side":"BUY","symbol":"AAPL","timestamp":"1764608000"}%   
```

### For Unknown Stock Symbol
```
panpaliamahen@Mahendras-MacBook-Air TradeExecution % curl -X POST -H "Content-Type: application/json" -d '{"symbol": "UNKNOWN", "side": "BUY", "quantity": 10}' http://localhost:9080/trade/market

Symbol not found%  
```
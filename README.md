# Binance Local Order Book Builder
## Design
<img width="497" alt="Screenshot 2025-07-04 at 3 39 27 PM" src="https://github.com/user-attachments/assets/ddddfe7d-11ac-477c-9ab6-933e3433243c" />

- Feed Handler: Manages incoming L2 diff data.
- Order Book Handler: Processes L2 diff data and updates the order book, drops stale data and decides if we need to reobtain a full snapshot of the order book (we missed a update).

## Example output

```bash
    l1={
      bid=108725.810000, ask=108725.820000, mid=108725.825000 # top of book data
    }
    asks={
        price=108725.820000, quantity=2.290330 # top of asks is best ask (lower willing seller)
        price=108725.830000, quantity=0.000300
        price=108726.000000, quantity=0.001950
        price=108726.010000, quantity=0.000220
        price=108726.100000, quantity=0.000350
        price=108726.420000, quantity=0.000060
        price=108726.550000, quantity=0.000100
        price=108726.560000, quantity=0.076010
        price=108726.780000, quantity=0.000460
        price=108726.800000, quantity=0.000350
    }
    bids={
        price=108725.810000, quantity=10.368610 # top of bids is best bid (highest willing bidder)
        price=108725.800000, quantity=0.100620
        price=108725.580000, quantity=0.001910
        price=108724.380000, quantity=0.000680
        price=108724.370000, quantity=0.000690
        price=108724.010000, quantity=0.000100
        price=108724.000000, quantity=0.068200
        price=108723.650000, quantity=0.000100
        price=108723.470000, quantity=0.031980
        price=108723.250000, quantity=0.000050
    } 
```
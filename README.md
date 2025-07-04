# Binance Local Order Book Builder
## Design
<img width="497" alt="Screenshot 2025-07-04 at 3 39 27 PM" src="https://github.com/user-attachments/assets/ddddfe7d-11ac-477c-9ab6-933e3433243c" />

- Feed Handler: Manages incoming L2 diff data.
- Order Book Handler: Processes L2 diff data and updates the order book, drops stale data and decides if we need to reobtain a full snapshot of the order book (we missed a update).

## Example output

```bash
    l1={bid=108705.450000, ask=108705.460000, mid=108705.465000}
    asks={
        {price=108705.460000, qty=1.285450},
        {price=108705.470000, qty=0.000500},
        {price=108705.990000, qty=0.000050},
        {price=108706.000000, qty=0.003450},
        {price=108706.810000, qty=0.001470},
        {price=108707.000000, qty=0.000100},
        {price=108707.090000, qty=0.000100},
        {price=108707.100000, qty=0.025820},
        {price=108707.110000, qty=0.032870},
        {price=108707.210000, qty=0.035320},
    }
    bids={
        {price=108705.450000, qty=3.575490},
        {price=108705.440000, qty=0.001170},
        {price=108705.420000, qty=0.042650},
        {price=108705.310000, qty=0.000100},
        {price=108705.050000, qty=0.001370},
        {price=108705.000000, qty=0.001860},
        {price=108704.910000, qty=0.186250},
        {price=108704.530000, qty=0.000100},
        {price=108704.520000, qty=0.040100},
        {price=108704.000000, qty=0.048850},
    }
```
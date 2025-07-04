# Binance Local Order Book Builder
## Design
<img width="497" alt="Screenshot 2025-07-04 at 3 39 27 PM" src="https://github.com/user-attachments/assets/ddddfe7d-11ac-477c-9ab6-933e3433243c" />

- Feed Handler: Manages incoming L2 diff data.
- Order Book Handler: Processes L2 diff data and updates the order book, drops stale data and decides if we need to reobtain a full snapshot of the order book (we missed a update).

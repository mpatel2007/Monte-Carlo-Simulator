# Monte Carlo Stock Price Simulator

A C++ project that fetches real-time stock data from the Finnhub API and runs a Monte Carlo simulation to estimate future stock prices.  
This project uses WinHTTP for requests and `nlohmann::json` for parsing the API response.

---

## 📈 Features

- Fetches **live stock quotes** from Finnhub  
- Parses JSON results (current price, open, high, low, previous close, etc.)  
- Runs Monte Carlo simulation using:
  - Daily volatility  
  - Drift (expected return)  
  - Number of simulation days  
  - Number of simulation runs  
- Calculates:
  - Average final price  
  - Standard deviation  
  - Lowest/Highest simulated final price  
- Fully Windows-compatible (WinHTTP, no curl needed)

---

## 🧠 Monte Carlo Model Explanation

The simulation uses a geometric Brownian motion model:


## 📂 Project Structure


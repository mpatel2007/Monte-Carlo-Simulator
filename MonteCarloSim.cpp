#include <iostream>
#include <string>
#include <windows.h>
#include <winhttp.h>
#include "json.hpp"
#include <random>
//Everytime you edit, type g++ MonteCarloSim.cpp -o sim -lwinhttp
using json = nlohmann::json;
using namespace std;
class MonteCarloSim {
public:
    MonteCarloSim() {}

    std::string fetchStockPrice(const std::string& symbol, const std::string& apiKey) {
        // bro we hitting finnhub
        std::wstring host = L"finnhub.io";
        std::wstring path = L"/api/v1/quote?symbol=" + toWide(symbol) + L"&token=" + toWide(apiKey);

        HINTERNET hSession = WinHttpOpen(L"MC-Sim-Agent/1.0",
                                         WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                         WINHTTP_NO_PROXY_NAME,
                                         WINHTTP_NO_PROXY_BYPASS, 0);

        if (!hSession) {
            return "error: session fail";
        }

        HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(),
                                            INTERNET_DEFAULT_HTTPS_PORT, 0);

        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "error: connect fail";
        }

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
                                                NULL, WINHTTP_NO_REFERER,
                                                WINHTTP_DEFAULT_ACCEPT_TYPES,
                                                WINHTTP_FLAG_SECURE);

        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "error: request fail";
        }

        BOOL sent = WinHttpSendRequest(hRequest,
                                       WINHTTP_NO_ADDITIONAL_HEADERS,
                                       0, WINHTTP_NO_REQUEST_DATA, 0,
                                       0, 0);

        if (!sent || !WinHttpReceiveResponse(hRequest, NULL)) {
            cleanup(hSession, hConnect, hRequest);
            return "error: send fail";
        }

        std::string response = readResponse(hRequest);

        cleanup(hSession, hConnect, hRequest);

        return response;
    }

    void run() {
        std::string symbol, apiKey;
        std::cout << "Enter stock ticker: ";
        std::cin >> symbol;

        cout << "Enter Finnhub API key: ";
        std::cin >> apiKey;
        std::string raw = fetchStockPrice(symbol, apiKey);
        // parse json
        auto j = json::parse(raw);

        // clean stock dashboard
        cout << "\n========== STOCK DASHBOARD ==========\n";
        cout << "bro current price: " << j["c"] << endl;
        cout << "open price:        " << j["o"] << endl;
        cout << "day high:          " << j["h"] << endl;
        cout << "day low:           " << j["l"] << endl;
        cout << "prev close:        " << j["pc"] << endl;
        cout << "day change:        " << j["d"] << endl;
        cout << "percent change:    " << j["dp"] << "%" << endl;
        cout << "=====================================\n\n";


        json data = json::parse(raw);

        double price = data["c"];
        runMonteCarlo(price);

        std::cout << "bro current price is: " << price << "\n\n";

        // you can now do monte carlo stuff
    }
    double randomNormal(double mean, double stddev) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::normal_distribution<> dist(mean, stddev);
        return dist(gen);
    }

    void runMonteCarlo(double startPrice) {
    double volatility, drift;
    int days, sims;

    std::cout << "bro enter daily volatility (like 0.02 for 2%): ";
    std::cin >> volatility;

    std::cout << "bro enter drift (like 0.001): ";
    std::cin >> drift;

    std::cout << "bro how many days to simulate: ";
    std::cin >> days;

    std::cout << "bro how many simulation runs: ";
    std::cin >> sims;

    std::vector<double> finalPrices;
    finalPrices.reserve(sims);

    for (int s = 0; s < sims; s++) {
        double price = startPrice;

        for (int d = 0; d < days; d++) {
            double r = randomNormal(drift, volatility);
            price *= (1 + r);
        }

        finalPrices.push_back(price);
    }

    // compute stats
    double sum = 0;
    for (double x : finalPrices) sum += x;
    double avg = sum / sims;

    double var = 0;
    for (double x : finalPrices) var += (x - avg) * (x - avg);
    double sd = sqrt(var / sims);

    double minP = *std::min_element(finalPrices.begin(), finalPrices.end());
    double maxP = *std::max_element(finalPrices.begin(), finalPrices.end());

    std::cout << "\n===== MONTE CARLO RESULTS =====\n";
    std::cout << "avg final price: " << avg << "\n";
    std::cout << "std deviation:  " << sd << "\n";
    std::cout << "lowest final:   " << minP << "\n";
    std::cout << "highest final:  " << maxP << "\n";
    std::cout << "================================\n\n";
}


private:

    static std::wstring toWide(const std::string& s) {
        return std::wstring(s.begin(), s.end());
    }

    static void cleanup(HINTERNET h1, HINTERNET h2, HINTERNET h3) {
        WinHttpCloseHandle(h3);
        WinHttpCloseHandle(h2);
        WinHttpCloseHandle(h1);
    }

    static std::string readResponse(HINTERNET hRequest) {
        DWORD size = 0;
        std::string result;

        while (true) {
            WinHttpQueryDataAvailable(hRequest, &size);
            if (size == 0) break;

            char* buffer = new char[size + 1];
            ZeroMemory(buffer, size + 1);

            DWORD downloaded = 0;
            WinHttpReadData(hRequest, buffer, size, &downloaded);

            result.append(buffer, downloaded);
            delete[] buffer;
        }

        return result;
    }
};

int main() {
    MonteCarloSim sim;
    sim.run();
    return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

struct Price {
    int x, y, cost;
};

const static int INF = 1000000;
const static int LENGTH_OF_FILE_NAME = 42;

int insertNumber(const char *text);

string *insertNamesOfCities(int cities);

int *createArrayWithDefault(int cities);

void printMinPricesForChosenCity(string *names, int *minPrices, int cities, int begin, ofstream &file);

void printOptimalPrice(const string *names, const int *price, int k, ofstream &file);

int main() {
    // the number of nods
    auto cities = insertNumber("cities");

    // array for edges
    // max number of edges = nodes * (nodes - 1) / 2
    Price prices[(cities * (cities - 1) / 2)];

    // names for cities-nodes
    auto *names = insertNamesOfCities(cities);

    // this code creates name of file based on local time.
    char timeStr[LENGTH_OF_FILE_NAME];
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    strftime(timeStr, LENGTH_OF_FILE_NAME, "%d.%m.%Y_%H-%M-%S_Prices_of_tickets.txt", timeInfo);
    ofstream file(timeStr, ios_base::app);
    if (!file.is_open()) {
        printf("Error to open file '%s'\n", timeStr);
        return 1;
    }

    // the number of prices (it means, that some cities have connection between them and ticket exists)
    int pricesNumber = 0;

    // cycle for inserting prices of tickets between different cities, if cities have price between them
    for (int i = 0; i < cities; i++) {
        for (int j = i + 1; j < cities; j++) {
            label:
            int price;
            printf("\nCost of ticket between %s(%d)-%s(%d):", names[i].c_str(), i + 1, names[j].c_str(), j + 1);
            scanf("%d", &price);
            if (price < 0) {
                printf("Please insert the positive value of ticket's price\n");
                goto label;
            }
            if (price != 0) {
                prices[pricesNumber].x = i;
                prices[pricesNumber].y = j;
                prices[pricesNumber].cost = price;
                file << "Cost of ticket between " << names[i].c_str() << "-" << names[j].c_str() << " " << price
                     << "\n";
                pricesNumber++;
            }
        }
    }

    // number of start-city
    int start = insertNumber("start city");
    int begin = start - 1;

    // the min price form start-city to each cities
    auto *minPrice = createArrayWithDefault(cities);

    // Begin of Bellman Ford's Algorithm
    minPrice[begin] = 0;
    for (int i = 0; i < cities - 1; i++) {
        for (int j = 0; j < pricesNumber; j++) {
            int firstCity = prices[j].x;
            int secondCity = prices[j].y;
            int road = prices[j].cost;

            if (minPrice[firstCity] + road < minPrice[secondCity]) {
                minPrice[secondCity] = minPrice[firstCity] + road;
            } else if (minPrice[secondCity] + road < minPrice[firstCity]) {
                minPrice[firstCity] = minPrice[secondCity] + road;
            }
        }
    }
    // End of Bellman Ford's Algorithm
    printMinPricesForChosenCity(names, minPrice, cities, begin, file);

    // number of finish-city
    int finish = insertNumber("finish city");
    int end = finish - 1;
    file << "\nFinish city is " << names[end] << "\n";

    // array of cities, from which the cheapest way consist of
    int price[cities];

    // first element is the finish-city
    price[0] = finish;

    //Code for finding fully optimal price, which uses calculated data of min price by Ford's Algorithm
    // k - counter of steps
    int k = 1;
    int reversePrice = minPrice[end];
    while (end != begin) {
        for (int i = 0; i < cities; ++i) {
            for (int j = 0; j < pricesNumber; ++j) {
                int firstCity = prices[j].x;
                int secondCity = prices[j].y;
                int cost = prices[j].cost;

                if (secondCity == end && cost != 0) {
                    int temp = reversePrice - cost;
                    if (temp == minPrice[i]) {
                        reversePrice = temp;
                        end = firstCity;
                        price[k] = i + 1;
                        k++;
                    }
                } else if (firstCity == end && cost != 0) {
                    int temp = reversePrice - cost;
                    if (temp == minPrice[i]) {
                        reversePrice = temp;
                        end = secondCity;
                        price[k] = i + 1;
                        k++;
                    }
                }
            }
        }
    }

    printOptimalPrice(names, price, k, file);

    delete[] names;
    return 0;
}

int insertNumber(const char *text) {
    label:
    int number;
    printf("\nInsert the number of %s:", text);
    scanf("%d", &number);
    cin.ignore();
    if (number <= 0) {
        printf("Wrong number\n");
        goto label;
    }
    return number;
}

string *insertNamesOfCities(int cities) {
    auto *names = new string[cities];
    char buf[35];
    printf("Insert names of cities:\n");
    for (int i = 0; i < cities; ++i) {
        printf("City #%d:", i + 1);
        cin.getline(buf, 35, '\n');
        names[i] = buf;
    }
    return names;
}

int *createArrayWithDefault(int cities) {
    int *arr = new int[cities];
    for (int i = 0; i < cities; i++) {
        arr[i] = INF;
    }
    return arr;
}

void printMinPricesForChosenCity(string *names, int *minPrices, int cities, int begin, ofstream &file) {
    printf("The min price of tickets form start-city to each city:\n");
    file << "\nStart city is " << names[begin] << "\n\nThe min distance form start-city to each city:\n";
    for (int i = 0; i < cities; i++) {
        if (minPrices[i] == INF) {
            printf("%s(%d) -> %s(%d) = %d\n", names[begin].c_str(), begin + 1, names[i].c_str(), i + 1, INF);
            file << names[begin].c_str() << "-" << names[i].c_str() << " = " << INF << "\n";
        } else {
            printf("%s(%d) -> %s(%d) = %d\n", names[begin].c_str(), begin + 1, names[i].c_str(), i + 1, minPrices[i]);
            file << names[begin].c_str() << "-" << names[i].c_str() << " = " << minPrices[i] << "\n";
        }
    }
}

void printOptimalPrice(const string *names, const int *price, int k, ofstream &file) {
    printf("\nOptimal price: start -> ");
    file << "\nOptimal price: start -> ";
    for (int i = k - 1; i >= 0; i--) {
        int numberOfCity = price[i] - 1;
        printf("%s -> ", names[numberOfCity].c_str());
        file << names[numberOfCity].c_str() << " -> ";
    }
    printf("finish");
    file << "finish";
}
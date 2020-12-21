#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>

using namespace std;

struct Ticket {
    int x, y;
    double cost;
};

const static double INF = 1000000.00;
const static int LENGTH_OF_FILE_NAME = 42;
const static int MAX_LENGTH_OF_CITY_NAME = 35;
const static double DOUBLE_ZERO = 0.00000001;

int insertNumber(const char *text);

int insertStartOrFinishCity(const char *text, int cities);

string *insertNamesOfCities(int cities);

double *createArrayWithDefault(int cities);

void printMinPricesForChosenCity(string *names, double *minPrices, int cities, int begin, ofstream &file);

void printOptimalWay(const string *names, const int *way, int k, ofstream &file);

int main() {
    // the number of cities (nods of graph)
    newGraph:
    auto cities = insertNumber("cities");

    // array of tickets between cities
    // max count of elements = nodes * (nodes - 1) / 2
    Ticket tickets[(cities * (cities - 1) / 2)];

    // insert names for cities-nodes
    auto *names = insertNamesOfCities(cities);

    // this code creates name of file based on local time
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

    // the common number of tickets in array of all Tickets (it means, that some cities have connection between them and ticket exists)
    int numberOfTicket = 0;

    // cycle for inserting prices of tickets between different cities, if cities have way between them
    for (int i = 0; i < cities; i++) {
        for (int j = i + 1; j < cities; j++) {
            label:
            double price;
            printf("\nCost of ticket between %s(%d)-%s(%d):", names[i].c_str(), i + 1, names[j].c_str(), j + 1);
            scanf("%lf", &price);
            if (price < 0) {
                printf("Please insert the positive value of ticket's price\n");
                goto label;
            }
            if (price != 0) {
                tickets[numberOfTicket].x = i;
                tickets[numberOfTicket].y = j;
                tickets[numberOfTicket].cost = price;
                file << "Cost of ticket between " << names[i].c_str() << "-" << names[j].c_str() << " " << fixed
                     << setprecision(2) << price << "\n";
                numberOfTicket++;
            }
        }
    }
    newWay:
    // number of start-city
    int start = insertStartOrFinishCity("start city", cities);
    int begin = start - 1;

    // the min way from start-city to each cities, which based on min price of tickets
    auto *minPrice = createArrayWithDefault(cities);
    // begin of Bellman-Ford's algorithm
    minPrice[begin] = 0;
    for (int i = 0; i < cities - 1; i++) {
        for (int j = 0; j < numberOfTicket; j++) {
            int firstCity = tickets[j].x;
            int secondCity = tickets[j].y;
            double cost = tickets[j].cost;

            if (minPrice[firstCity] + cost < minPrice[secondCity]) {
                minPrice[secondCity] = minPrice[firstCity] + cost;
            } else if (minPrice[secondCity] + cost < minPrice[firstCity]) {
                minPrice[firstCity] = minPrice[secondCity] + cost;
            }
        }
    }
    // end of Bellman-Ford's algorithm
    printMinPricesForChosenCity(names, minPrice, cities, begin, file);

    // number of finish-city
    int finish = insertStartOrFinishCity("finish city", cities);
    int end = finish - 1;
    file << "\nFinish city is " << names[end] << "\n";

    // array of cities, from which the cheapest way consist of
    int way[cities];

    // first element is the finish-city
    way[0] = finish;

    // code for finding optimal way, which uses calculated data of min way by Ford's Algorithm
    // k - counter of steps
    int k = 1;
    double reversePrice = minPrice[end];
    while (end != begin) {
        for (int i = 0; i < cities; ++i) {
            for (int j = 0; j < numberOfTicket; ++j) {
                int firstCity = tickets[j].x;
                int secondCity = tickets[j].y;
                double cost = tickets[j].cost;

                if (secondCity == end && cost != 0) {
                    double temp = reversePrice - cost;
                    if (abs(temp - minPrice[i]) < DOUBLE_ZERO) {
                        reversePrice = temp;
                        end = firstCity;
                        way[k] = i + 1;
                        k++;
                    }
                } else if (firstCity == end && cost != 0) {
                    double temp = reversePrice - cost;
                    if (abs(temp - minPrice[i]) < DOUBLE_ZERO) {
                        reversePrice = temp;
                        end = secondCity;
                        way[k] = i + 1;
                        k++;
                    }
                }
            }
        }
    }

    printOptimalWay(names, way, k, file);

    // logic for next behavior of program
    int answer;
    printf("\nWould you like to continue? "
           "\nIf you want to insert new map of cities - chose 1;"
           "\nIf you want to calculate another way and start from another city - chose 2;"
           "\nIf you want to exit - chose 3."
           "\nYour answer: ");
    scanf("%d", &answer);
    switch (answer) {
        case 1:
            delete[] names;
            goto newGraph;
        case 2:
            goto newWay;
        default:
            delete[] names;
            file.close();
            return 0;
    }
}

int insertNumber(const char *text) {
    label1:
    int number;
    printf("\nInsert the number of %s:", text);
    scanf("%d", &number);
    cin.ignore();
    if (number <= 0) {
        printf("Wrong number\n");
        goto label1;
    }
    return number;
}

int insertStartOrFinishCity(const char *text, int cities) {
    label2:
    int number;
    printf("\nInsert the number of %s:", text);
    scanf("%d", &number);
    cin.ignore();
    if (number <= 0 || number > cities) {
        printf("Wrong number of city. Number of city should be > 0 and < count all cities.\n");
        goto label2;
    }
    return number;
}

string *insertNamesOfCities(int cities) {
    auto *names = new string[cities];
    char buf[MAX_LENGTH_OF_CITY_NAME];
    printf("Insert names of cities:\n");
    for (int i = 0; i < cities; ++i) {
        printf("City #%d:", i + 1);
        cin.getline(buf, MAX_LENGTH_OF_CITY_NAME, '\n');
        names[i] = buf;
    }
    return names;
}

double *createArrayWithDefault(int cities) {
    auto *arr = new double[cities];
    for (int i = 0; i < cities; i++) {
        arr[i] = INF;
    }
    return arr;
}

void printMinPricesForChosenCity(string *names, double *minPrices, int cities, int begin, ofstream &file) {
    printf("The min price of tickets form start-city to each city:\n");
    file << "\nStart city is " << names[begin] << "\n\nThe min distance form start-city to each city:\n";
    for (int i = 0; i < cities; i++) {
        if ((abs(minPrices[i] - INF) < DOUBLE_ZERO)) {
            printf("%s(%d) -> %s(%d) = %.2lf\n", names[begin].c_str(), begin + 1, names[i].c_str(), i + 1, INF);
            file << names[begin].c_str() << "-" << names[i].c_str() << " = " << fixed << setprecision(2) << INF << "\n";
        } else {
            printf("%s(%d) -> %s(%d) = %.2lf\n", names[begin].c_str(), begin + 1, names[i].c_str(), i + 1,
                   minPrices[i]);
            file << names[begin].c_str() << "-" << names[i].c_str() << " = " << fixed << setprecision(2) << minPrices[i]
                 << "\n";
        }
    }
}

void printOptimalWay(const string *names, const int *way, int k, ofstream &file) {
    printf("\nOptimal way: start -> ");
    file << "\nOptimal way: start -> ";
    for (int i = k - 1; i >= 0; i--) {
        int numberOfCity = way[i] - 1;
        printf("%s -> ", names[numberOfCity].c_str());
        file << names[numberOfCity].c_str() << " -> ";
    }
    printf("finish");
    file << "finish";
}
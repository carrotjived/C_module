#include <iostream>
#include <string> 
#include <list>
#include <vector>
#include <fstream> 
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <limits>
using namespace std;


class Person {
    private: 
        string name;
        double rate;
        double overTimeRate;
        double tax;
    public: 
        Person (string name, double rate, double overTimeRate, double tax): name(name), rate(rate), overTimeRate(overTimeRate), tax(tax) {}

        string getName() const { return this->name; }
        double getRate() const { return this->rate; }
        double getOverTimeRate() const { return this->overTimeRate; }
        double getTax() const { return this->tax; }

        void displayBasic() const {

            cout << fixed << setprecision(2);
            cout << "Name: " << name << endl;
            cout << "Rate: $" << rate << endl;
            cout << "Over Time Rate: $" << overTimeRate << endl;
            cout << "Tax: " << tax << "%" << endl;

        }
};

vector<Person> readFile(string fileName){
    vector<Person> people;
    ifstream file(fileName);

    if (!file.is_open()){
        cerr << "File not found" << fileName << endl;
        return people;
    }

    string line;
    bool header;

    while (getline(file, line)) {
        stringstream ss(line);
        string name, rateStr, overTimeRateStr, taxStr;
        
        getline(ss, name, ',');
        getline(ss, rateStr, ',');
        getline(ss, overTimeRateStr, ',');
        getline(ss, taxStr, ',');

        if (header) {
            header = false;
            continue;
        }



        if (!name.empty() && !rateStr.empty() && !overTimeRateStr.empty() && !taxStr.empty()){
            double rate = stod(rateStr);
            double overTimeRate = stod(overTimeRateStr);
            double tax = stod(taxStr);

            people.emplace_back(name, rate, overTimeRate, tax);

        }
    }

    file.close();
    return people;


}


string toLowerCase (string word){
    transform(word.begin(), word.end(), word.begin(), [](unsigned char c) {return tolower(c); });
    return word;
}

string toLowerCase (string word){
    transform(word.begin(), word.end(), word.begin(), [](unsigned char c) {return tolower(c); });
    return word;
}


void findAndCalculate(const string& name, const vector<Person>& people){
    for (const Person& person: people) {
        if (toLowerCase(person.getName()) == toLowerCase(name)){
            person.displayBasic();

            double hours, overHours;

            cout << "\n\nEnter Hours: ";
            cin >> hours;

            cout <<"\nEnter Over Time Hours: ";
            cin >> overHours;

            double basePay = person.getRate() * hours;
            double overTimePay = person.getOverTimeRate() * overHours;
            double grossIncome = basePay + overTimePay;
            double netIncome = grossIncome - (grossIncome * (person.getTax()/100));

            cout << fixed << setprecision(2);
            cout << "\nBase Pay: $" << basePay << endl;
            cout << "Over Time Pay: $" << overTimePay << endl;
            cout << "Gross Income: $" << grossIncome << endl;
            cout << "Net Income: $" << netIncome << endl;

            return;
        }
    }
}

Person* createRecord (string name){
    bool stopper = false;
    Person* newPerson = nullptr;


    while (!stopper) {
    cout<< "\n\nWould you like to create a new record for " << name << "? ";
    string prompt;
    getline(cin, prompt);
        if (toLowerCase(prompt) == "y"){
            double rate, overTimeRate, tax;

            cout << "Enter Rate: ";
            cin >> rate;

            cout << "Enter Over Time Rate: ";
            cin >> overTimeRate;

            cout << "Enter Tax Rate: ";
            cin >> tax;


            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            ofstream file("employee.csv", ios::app);

            if (!file.is_open()){
                file << name << ","
                     << fixed << setprecision(2)
                     << rate << ","
                     << overTimeRate << ","
                     << tax << "\n";
                file.close();
            } else {
                cout <<"Error: Could not save record. Please try again." << endl;
            }

            newPerson = new Person(name, rate, overTimeRate, tax);
            cout << "Record created!";
            stopper = true;

        }

        else if (toLowerCase(prompt) == "n"){
            cout <<"No record created." << endl;
            stopper = true;
        }

        else {
            cout <<"\nInvalid data. Please enter Y or N." << endl;
        }
    }

    return newPerson;

}



int main() {
    cout << "Hello World" << endl;
    return 0;
}
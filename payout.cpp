#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <string>
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
//Class
class Person {
    private: 
        string name;
        double rate;
        double overTimeRate;
        double tax;
    public: 
        Person (const string name, double rate, double overTimeRate, double tax): name(name), rate(rate), overTimeRate(overTimeRate), tax(tax) {}

        string getName() const { return this->name; }
        double getRate() const { return this->rate; }
        double getOverTimeRate() const { return this->overTimeRate; }
        double getTax() const { return this->tax; }

       
};


struct DisplayRate{
    Fl_Output* baseRate;
    Fl_Output* overTimeRate;
    Fl_Output* tax;
};

struct DisplayPay {
    Fl_Output* basePay;
    Fl_Output* overTimePay;
    Fl_Output* grossIncome;
    Fl_Output* netIncome;

};

struct HourInputs {
    Fl_Input* hoursInput;
    Fl_Input* overTimeInput;
};

struct SearchContext {
    Fl_Input* inputName;
    DisplayRate outputs;
};

HourInputs createInputFields(Fl_Window* window) {
   Fl_Box* hoursLabel = new Fl_Box(20, 180, 150, 30, "Enter Hours: ");
    hoursLabel->labelsize(18);
    hoursLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    window->add(hoursLabel); // explicitly add to window

    Fl_Input* hoursInput = new Fl_Input(220, 180, 100, 30);
    window->add(hoursInput);

    Fl_Box* overtimeLabel = new Fl_Box(20, 220, 150, 30, "Enter Overtime Hours: ");
    overtimeLabel->labelsize(18);
    overtimeLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    window->add(overtimeLabel);

    Fl_Input* overtimeInput = new Fl_Input(220, 220, 100, 30);
    window->add(overtimeInput);

    window->redraw(); // force redraw

    return HourInputs{hoursInput, overtimeInput};
};

DisplayRate displayRate(Fl_Window* window) {
    Fl_Box* baserateLabel = new Fl_Box(20, 80, 80, 30, "Rate: $");
    baserateLabel->labelsize(18);
    baserateLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    Fl_Output* baseRateValue = new Fl_Output(170, 80, 80, 30);
    baseRateValue->value("");
    baseRateValue->readonly(1);
  

    Fl_Box*  overTimeRateLabel = new Fl_Box(20, 110, 80, 30, "Overtime Rate: $" );
    overTimeRateLabel->labelsize(18);
    overTimeRateLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    Fl_Output* overTimeRateValue = new Fl_Output(170, 110, 80, 30);
    overTimeRateValue->value("");
    overTimeRateValue->readonly(1);

    Fl_Box* taxLabel = new Fl_Box(20, 140, 80, 30, "Tax: ");
    taxLabel->labelsize(18);
    taxLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    Fl_Output* taxValue = new Fl_Output(170, 140, 80, 30);
    taxValue->value("");
    taxValue->readonly(1);

    return DisplayRate{baseRateValue, overTimeRateValue, taxValue};
};




vector<Person> readFile (){
    string fileName = "employee.csv";
    vector<Person> people;
    ifstream file(fileName);

    if (!file.is_open()){
        cerr << "File not found" << fileName << endl;
        return people;
    }

    string line;
    bool header = true;

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

string formatDouble(double value) {
    ostringstream oss;
    oss << fixed << setprecision(2) << value;
    return oss.str();
}






void showDisplayRate(const Person& person, DisplayRate& outputs){
    outputs.baseRate->value(formatDouble(person.getRate()).c_str());
    outputs.overTimeRate->value(formatDouble(person.getOverTimeRate()).c_str());
    
    string taxStr = formatDouble(person.getTax()) + "%";
    outputs.tax->value(taxStr.c_str());
}

void searchNameButton(Fl_Widget* widget, void* data) {

    
   SearchContext* context = static_cast<SearchContext*>(data);
   string name = context->inputName->value();
   vector<Person> people = readFile();

    for(const Person& person: people){
        if (toLowerCase(person.getName()) == toLowerCase(name)){
            showDisplayRate(person, context->outputs);
            HourInputs hours = createInputFields (context->inputName->window());
            

            return;
        }

        
    }

}

int main() {
  
    Fl_Window window(600, 400, "FLTK Payout Program");

    (new Fl_Box(20, 40, 80, 30, "Enter name: "))->labelsize(18);
    Fl_Input* nameInput = new Fl_Input(130, 40, 200 ,30);
    Fl_Button* submitButton = new Fl_Button(350, 45, 100, 20, "Submit");

    DisplayRate outputs = displayRate(&window);

    SearchContext* context = new SearchContext{nameInput, outputs};

    submitButton->callback(searchNameButton, context);


   


    window.end();
    window.show();

    return Fl::run();
}

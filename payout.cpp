#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <fstream> 
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <Fl/fl_ask.H>
#include <ostream>

using namespace std;

//This will serve as a saving point for details of the person
class Person {
    public: 
    
    string name;
    double rate;
    double overTimeRate;
    double tax;

    Person(string n="", double r=0, double ot=0, double t=0) : name(n), rate(r), overTimeRate(ot), tax(t) {}
};

class PayrollApp {
    private: 
        //struct for the basic GUI widgets
        Fl_Window* window;
        Fl_Input* nameInput;
        Fl_Button* submitButton;
        Fl_Button* calculateButton;

        //Widgets for showing the current rate 
        Fl_Box* rateLabel;
        Fl_Box* overTimeLabel;
        Fl_Box* taxLabel;

        //Widgets for hours input
        Fl_Input* hoursInput;
        Fl_Input* overTimeInput;

        //Widgets for calculation for the pays and output of the pay
        Fl_Box* basePay;
        Fl_Box* overTimePay;
        Fl_Box* gross;
        Fl_Box* net;


        Person currentPerson;

        //Widget for a support message 
        Fl_Box* statusLabel;

        //Widgets for creating new record
        Fl_Button* createRecordButton;
        Fl_Input* newRate;
        Fl_Input* newOvertimeRate;
        Fl_Input* newTax;

        //Widget for delete button
        Fl_Button* deleteButton;

        //Widget for update button
        Fl_Button* updateButton;




        //Utility:
        //This will set the decimal points to two
        string formatDouble(double value) {
            stringstream ss;
            ss<< fixed << setprecision(2) << value;
            return ss.str();
        }

        string formatInput (double value){
            ostringstream oss;
            oss << fixed << setprecision(2) << value;
            return oss.str();
        };


        //Read the csv file, split them into respective parts, save them as a person, save them to a vector and return the vector 
        vector<Person> readCSV() {
            vector<Person> people;
            ifstream file("employee.csv");

            if (!file.is_open()){
                cerr << "File not found: employee.csv" << endl;
                return people;
            }

            string line;
            bool header = true;

            while(getline(file, line)) {
                stringstream ss(line);
                string name, rateString, overTimeRateString, taxString;

                getline(ss, name, ',');
                getline(ss, rateString, ',');
                getline(ss, overTimeRateString, ',');
                getline(ss, taxString, ',');

                if (header) {
                    header = false;
                    continue;
                }

                if (!name.empty() && !rateString.empty() && !overTimeRateString.empty() && !taxString.empty()) {
                    try {
                        double rate = stod(rateString);
                        double overTimeRate = stod(overTimeRateString);
                        double tax = stod(taxString);

                        people.emplace_back(name, rate, overTimeRate, tax);
                    } catch (const exception& e) {
                        cerr<< "Invalid data in CSV: " << line << endl;
                    }
                }
            }
            
            file.close();
            return people;

        }
        
        //method to make sure the input string would be proper cased. Useful for saving the name in the creating record to make sure that name is properly cased.
        string properCasingName(const string& input){
            string result = input;
            bool capitalize = true;

            for (size_t i = 0; i < result.size(); i++){
                if (isspace(result[i])){
                    capitalize = true;
                } else if (capitalize && isalpha(result[i])) {
                    result[i] = toupper(result[i]);
                    capitalize = false;
                } else {
                    result[i] = tolower(result[i]);
                }
            }

            return result;
        }
        
        //This will show the rate from the Person class and populate the labels to be seen in the GUI
        void showRates() {
            rateLabel->copy_label(("Rate: $" + formatDouble(currentPerson.rate)).c_str());
            overTimeLabel->copy_label(("Overtime Rate: $" + formatDouble(currentPerson.overTimeRate)).c_str());
            taxLabel->copy_label(("Tax: " + formatDouble(currentPerson.tax) + "%").c_str());

            rateLabel->show();
            overTimeLabel->show();
            taxLabel->show();
            hoursInput->show();
            overTimeInput->show();
            calculateButton->show();

        }
        //Hide some labels and buttons to make the GUI clear
        void hideRates(){
            rateLabel->hide();
            overTimeLabel->hide();
            taxLabel->hide();
            hoursInput->hide();
            overTimeInput->hide();
            calculateButton->hide();
            basePay->hide();
            overTimePay->hide(); 
            gross->hide();
            net->hide();


        }

        void hideNewRates(){
            newRate->hide();
            newOvertimeRate->hide();
            newTax->hide();
        }
        //Method to calculate and output the pay. Populates their respective labels to be shown in the GUI 
        void showPay() {
            
            double h = atof(hoursInput->value());
            double ot = atof(overTimeInput->value());

            double base = h * currentPerson.rate;
            double over = ot * currentPerson.overTimeRate;
            double grossPay = base + over;
            double netPay = grossPay * (1 - currentPerson.tax/100);

            basePay->copy_label(("Base Pay: $" + formatDouble(base)).c_str());
            overTimePay->copy_label(("Overtime Pay: $" + formatDouble(over)).c_str());
            gross->copy_label(("Gross Income: $" + formatDouble(grossPay)).c_str());
            net->copy_label(("Net Income: $" + formatDouble(netPay)).c_str());

            basePay->show();
            overTimePay->show();
            gross->show();
            net->show();
        }

        





        //Static methods for easy callback 
        //Submit Button
        static void submitButtonStatic (Fl_Widget* w, void* data){
            static_cast<PayrollApp*>(data)->submit_cb();
        }

        //Calculate Button
        static void calculateButtonStatic (Fl_Widget* w, void* data){
            static_cast<PayrollApp*>(data)->calc_cb();
        }

        //Create Button
        static void createRecordStatic (Fl_Widget* w, void* data){
            static_cast<PayrollApp*>(data)->createRecord_cb();
        }

        //Delete Button
        static void deleteButtonStatic (Fl_Widget* w, void* data){
            static_cast<PayrollApp*>(data)->delete_cb();
        }

        //Update Button 
        static void updateButtonStatic (Fl_Widget* w, void* data){
            static_cast<PayrollApp*>(data)->update_cb();
        }

        //Callbacks

        //Call back to update a record shoing input fields for new details to be saved to csv
        void update_cb(){
            hideRates();
            newRate->value((formatInput(currentPerson.rate)).c_str());
            newOvertimeRate->value((formatInput(currentPerson.overTimeRate)).c_str());
            newTax->value((formatInput(currentPerson.tax)).c_str());

            newRate->show();
            newOvertimeRate->show();
            newTax->show();

            calculateButton->label("Save Update");
            calculateButton->show();
        }
        //Call back to create a record showing input fields for new details to be saved to csv
        void createRecord_cb(){
            //"No Record" Label disappears when Create Record is clicked     
            statusLabel->copy_label("");
            statusLabel->hide();

            //Input fields appears
            newRate->show();
            newOvertimeRate->show();
            newTax->show();
            
            //Save Button appears 
            calculateButton->label("Save Record");
            calculateButton->show();

            newRate->value("");
            newOvertimeRate->value("");
            newTax->value("");


        }


        //Callback to submit button.
        void submit_cb(){
            string name = properCasingName(nameInput->value());
            vector<Person> people = readCSV();

            //Loop through the list of Person to find the name. If found, status shows that a record is found and will show the Person's current rates, overtime rates, and tax.
            //Input fields for hours rendered will appear and will calculate the base pay, overtime pay, gross income and net income
            for (auto &person: people){
                if (person.name == name) {
                    currentPerson = person;
                    createRecordButton->hide();
                    statusLabel->copy_label("Record Found!");
                    statusLabel->labelcolor(FL_GREEN);
                    statusLabel->show();
                    deleteButton->show();
                    updateButton->show();
                    showRates();
                    window->redraw();
                    return;
                }
            }

            //If no record is found, it will prompt the user to create the record. 
            hideRates();

            statusLabel->copy_label("No Record Found");
            statusLabel->labelcolor(FL_RED);
            statusLabel->show();    
            deleteButton->hide();
            updateButton->hide();
            createRecordButton->show();
        }


        //Callback to calculate the pay according to hours rendered and rates
        void calc_cb(){

            //Checks if the Person is newly created.
            if (string(calculateButton->label()) == "Save Record"){
                string n = properCasingName(nameInput->value());
                double r = atof(newRate->value());
                double ot = atof(newOvertimeRate->value());
                double t= atof(newTax->value());

                currentPerson = Person(n, r, ot, t); //Saves as a new Person 

                //Append to CSV
                ofstream file("employee.csv", ios::app);
                if (file.is_open()) {
                    file << n << "," << r << "," << ot << "," << t << "\n"; //Record will be saved as: "Victor Jared Onato, 12, 12, 12"
                    file.close();
                }

                //Calls the method to show the rates of the new person and calculate the pay. 
                statusLabel->label("Record created!");
                statusLabel->labelcolor(FL_GREEN);
                statusLabel->show();

                createRecordButton->hide();
                deleteButton->show();
                hideNewRates();
                showRates();
                calculateButton->label("Calculate");
                window->redraw();
                return;
            }

            if (string(calculateButton->label()) == "Save Update"){
                string n = properCasingName(nameInput->value());
                double r = atof(newRate->value());
                double ot = atof(newOvertimeRate->value());
                double t = atof(newTax->value());

                currentPerson = Person(n, r, ot, t);
                
                //Rewrite the CSV by looping again to another Vector and rewriting to csv
                vector<Person> people = readCSV();
                ofstream file("employee.csv", ios::trunc); //overwrite the file

                if (file.is_open()) {
                    file<< "Name,Rate,OvertimeRate, Tax\n";
                    bool found = false;

                    for(auto &person : people) {
                        if (person.name == n){
                            file << n << "," << r << "," << ot << "," << t << "\n";
                            found = true;
                        } else {
                            file << person.name << "," << person.rate << "," << person.overTimeRate << "," << person.tax << "\n";
                        }
                    }
                    file.close();
                }

                statusLabel->label("Record Updated");
                statusLabel->labelcolor(FL_GREEN);
                statusLabel->show();

                deleteButton->show();
                hideNewRates();
                showRates();
                calculateButton->label("Calculate");
                window->redraw();
                return;

            }
            showPay();
            hoursInput->value("");
            overTimeInput->value("");
            window->redraw();
        }
        //Call back to delete a record from the name input
        void delete_cb(){
            string name = properCasingName(nameInput->value());
            
            int choice = fl_choice(("Are you sure you want to delete the record for '" + name + "'?").c_str(), "Cancel", "Delete", nullptr);

            if (choice != 1){
                return;
            }

            vector<Person> people = readCSV();
            vector<Person> updated;
            bool found = false;

            //Loop through the old vector of Person and add them to the the new vector. If the name is found, it will skip to add to the new vector. 
            //This way it will be in a stack
            for (auto &person : people){
                if (person.name == name){
                    found = true;
                    continue;
                }
                updated.push_back(person); 
            }

            if (found){
                //Rewrite the whole csv with the new vector
                ofstream file("employee.csv");
                if (file.is_open()){
                    file << "Name,Rate,Overtime,Tax\n";
                    for (auto &person : updated) {
                        file << person.name << "," << person.rate << "," << person.overTimeRate << "," << person.tax << "\n";
                    }

                    file.close();
                }

                statusLabel->copy_label("Record deleted");
                statusLabel->show();
                
                //Reset UI
                hideNewRates();
                hideRates();

                nameInput->value("");
                deleteButton->hide();   

            } else {
                statusLabel->copy_label("Record not found");
                statusLabel->show();
            }

            window->redraw();


        }

    public:
        PayrollApp(){
            window = new Fl_Window(700, 400, "Payroll App");

            (new Fl_Box(20, 40, 100, 30, "Enter name: "))->labelsize(16);
            nameInput = new Fl_Input(130, 40, 200, 30);
            submitButton = new Fl_Button(20, 80, 100, 30, "Find");
            submitButton->callback(submitButtonStatic, this);

            


            //Labels to show rates details from csv
            rateLabel = new Fl_Box(20, 120, 200, 30, "Rate: $0.00");
            rateLabel->labelsize(16);
            rateLabel->hide();

            overTimeLabel = new Fl_Box(20, 160, 200, 30, "Overtime Rate: $0.00");
            overTimeLabel->labelsize(16);
            overTimeLabel->hide();

            taxLabel = new Fl_Box(20, 200, 200, 30, "Tax: 0.00%");
            taxLabel->labelsize(16);
            taxLabel->hide();

            //Hours input but hidden
            hoursInput = new Fl_Input(400, 120, 100, 30, "Enter Hours: ");
            hoursInput->hide();
            overTimeInput = new Fl_Input(400, 160, 100, 30, "Enter Overtime Hours: ");
            overTimeInput->hide();

            //calculate button also hidden
            calculateButton = new Fl_Button(400, 200, 100, 30, "Calculate");
            calculateButton->hide();
            calculateButton->callback(calculateButtonStatic, this);

            //Labels to show the calculated pay from the given rates
            basePay = new Fl_Box(20, 260, 200, 30, "Base Pay: $0.00"); 
            basePay->labelsize(16);
            basePay->hide();  


            overTimePay = new Fl_Box(240, 260, 200, 30, "Overtime Pay: $0.00"); 
            overTimePay->labelsize(16);
            overTimePay->hide();  


            gross = new Fl_Box(20, 290, 200, 30, "Gross Income: $0.00");   
            gross->labelsize(16);
            gross->hide();


            net = new Fl_Box(240, 290, 200, 30, "Net Income: $0.00");   
            net->labelsize(16);
            net->hide();


            
            //Create new Record Labels and Button
            createRecordButton = new Fl_Button(150, 80, 175, 30, "Create Record");
            createRecordButton->hide();
            createRecordButton->callback(createRecordStatic, this);

            newRate = new Fl_Input(170, 120, 200, 30, "Enter Rate: ");
            newRate->hide();

            newOvertimeRate = new Fl_Input(170, 160, 200, 30, "Enter Overtime Rate: ");
            newOvertimeRate->hide();

            newTax = new Fl_Input(170, 200, 200, 30, "Tax: ");
            newTax->hide();

            //Label for message if name not found from input 
            statusLabel = new Fl_Box(350, 40, 200, 30, "");
            statusLabel->labelsize(14);
            statusLabel->hide();

            //Button to delete a record
            deleteButton = new Fl_Button(150, 80, 100, 30, "Delete Record");
            deleteButton->hide();
            deleteButton->callback(deleteButtonStatic, this);

            //Button to update the record
            updateButton = new Fl_Button(280, 80, 100, 30, "Update Record");
            updateButton->hide();
            updateButton->callback(updateButtonStatic, this);


            

            window->end();
            window->show();
            

          
        }

    void run() {
        Fl::run();
    };



};

int main() {
    PayrollApp app;
    app.run();
    return 0;
}
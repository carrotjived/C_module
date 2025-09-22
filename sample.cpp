#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <string>
#include <iostream>

// Struct to hold widgets
struct InputWidgets {
    Fl_Input* hoursInput;
    Fl_Input* overHoursInput;
    Fl_Output* resultOutput;
};

// Callback
void calculate_cb(Fl_Widget*, void* data) {
    InputWidgets* inputs = (InputWidgets*)data;

    try {
        double hours = std::stod(inputs->hoursInput->value());
        double overHours = std::stod(inputs->overHoursInput->value());

        double total = hours + overHours;

        std::string result = "Total Hours = " + std::to_string(total);
        inputs->resultOutput->value(result.c_str());
    }
    catch (std::invalid_argument&) {
        inputs->resultOutput->value("Invalid input!");
    }
}

// Method to create GUI widgets
InputWidgets createInputs(Fl_Window* window) {
    Fl_Box* hoursLabel = new Fl_Box(20, 20, 100, 30, "Enter Hours:");
    Fl_Input* hoursInput = new Fl_Input(130, 20, 100, 30);

    Fl_Box* overLabel = new Fl_Box(20, 70, 100, 30, "Overtime Hours:");
    Fl_Input* overHoursInput = new Fl_Input(130, 70, 100, 30);

    Fl_Box* resultLabel = new Fl_Box(20, 120, 100, 30, "Result:");
    Fl_Output* resultOutput = new Fl_Output(130, 120, 150, 30);

    return {hoursInput, overHoursInput, resultOutput};
}

int main() {
    Fl_Window* window = new Fl_Window(350, 250, "Work Hours");

    // Call the method
    InputWidgets inputs = createInputs(window);

    // Button
    Fl_Button* button = new Fl_Button(100, 170, 120, 30, "Calculate");
    button->callback(calculate_cb, &inputs);

    window->end();
    window->show();
    return Fl::run();
}

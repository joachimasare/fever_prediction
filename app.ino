#include <Wire.h> 
#include <Adafruit_SSD1306.h> 
#include <Adafruit_GFX.h> 
#include "LM75.h" 

// OLED display parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LM75 sensor setup
LM75 tempSensor;

// Function prototypes
float collectTemperatureData();
float* fitPolynomial(float* temperatureData, int degree);
int compareWithFeverModels(float* coefficients);
void displayResult(int result);

void setup() {
  // Initializing I2C communication
  Wire.begin();

  // Initializing OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initializing temperature sensor
  tempSensor.begin();
}

void loop() {
  // Collect temperature data over 24hrs
  float temperatureData[24]; // Assumes reading per hour
  for(int i = 0; i < 24; i++) {
    temperatureData[i] = collectTemperatureData();
    delay(3600000); // Delay for an hour. was considering taking every second based program crashes on ESP2 so reduced the values.
  }

  // Fit polynomial to the temperature data
  int polynomialDegree = 3; // Example degree
  float* coefficients = fitPolynomial(temperatureData, polynomialDegree);

  // Compare with predefined fever models
  int result = compareWithFeverModels(coefficients);

  // Display result
  displayResult(result);

  // Clean up
  delete[] coefficients;
}

float collectTemperatureData() {
  // Function to collect temperature data from LM75
  return tempSensor.readTemperature();
}

float* fitPolynomial(float* temperatureData, int degree) {
  // Polynomial fitting logic
  float* coefficients = new float[degree + 1];

  // Example: Set coefficients to some dummy values for demonstration
  coefficients[0] = 1.0; // Constant term
  coefficients[1] = -0.5; // Linear term
  coefficients[2] = 0.2; // Quadratic term
  coefficients[3] = -0.05; // Cubic term

  return coefficients;
}

int compareWithFeverModels(float* coefficients) {
  // Compare coefficients with predefined models
 // Chekcing if certain coefficients are above a threshold
  if (coefficients[2] > 0.1 && coefficients[3] < -0.04) {
    return 1; // Malaria Likely
  } else if (coefficients[1] > 0.2 && coefficients[2] < 0.05) {
    return 2; // Typhoid Likely
  } else {
    return 0; // Normal Temperature
  }
}

void displayResult(int result) {
  display.clearDisplay();
  display.setCursor(0,0);
  if(result == 1) {
    display.println("Malaria Likely");
  } else if(result == 2) {
    display.println("Typhoid Likely");
  } else {
    display.println("Normal Temperature");
  }
  display.display();
}

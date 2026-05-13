#include "environmentparameters.h"

EnvironmentParameters::EnvironmentParameters(double temp, double hum, double sun)
    : temperature(temp), humidity(hum), sunlight(sun) {
}

double EnvironmentParameters::getParameter(const std::string& type) const {
    if (type == "temperature") return temperature;
    if (type == "humidity") return humidity;
    if (type == "sunlight") return sunlight;
    return 0.0;
}

void EnvironmentParameters::modifyParameter(const std::string& name, double value) {
    if (name == "temperature") temperature = value;
    else if (name == "humidity") humidity = value;
    else if (name == "sunlight") sunlight = value;
}

double EnvironmentParameters::getTemperature() const { return temperature; }
double EnvironmentParameters::getHumidity() const { return humidity; }
double EnvironmentParameters::getSunlight() const { return sunlight; }

EnvironmentParameters EnvironmentParameters::operator+(const EnvironmentParameters& other) const {
    return EnvironmentParameters(
        this->temperature + other.temperature,
        this->humidity + other.humidity,
        this->sunlight + other.sunlight
        );
}

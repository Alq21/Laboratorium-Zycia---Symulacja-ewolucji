#ifndef ENVIRONMENT_PARAMETERS_H
#define ENVIRONMENT_PARAMETERS_H

#include <string>
//ogl można by było dodać że własnie czesc płytek jest zimna ciepła, itd. i to sie bedzie łaczyć z parametrami ustawionymi w pliku przez uż

class EnvironmentParameters {
private:
    double temperature;
    double humidity;
    double sunlight;

public:
    EnvironmentParameters(double temp = 20.0, double hum = 50.0, double sun = 50.0);

    double getParameter(const std::string& type) const;
    void modifyParameter(const std::string& name, double value);

    double getTemperature() const;
    double getHumidity() const;
    double getSunlight() const;
    // Przeciążenie operatora dodawania do łączenia parametrów globalnych i lokalnych
    EnvironmentParameters operator+(const EnvironmentParameters& other) const;
};

#endif
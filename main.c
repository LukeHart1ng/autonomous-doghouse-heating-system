#include <msp430.h>

#define AMBIENT_SENSOR_INPUT ADCINCH_1
#define HEATING_PAD_SENSOR_INPUT ADCINCH_0
#define HEATER_CONTROL BIT2

void initializeADC();
void initializeGPIO();
void initializePWM();
int readTemperature(int channel);
void controlHeater(int ambientTemp, int padTemp);
void setPWM(int dutyCycle);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;     // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;         // Unlock GPIOs

    initializeGPIO();
    initializeADC();
    initializePWM();

    while (1) {
        int ambientTempF = readTemperature(AMBIENT_SENSOR_INPUT);
        int padTempF = readTemperature(HEATING_PAD_SENSOR_INPUT);
        controlHeater(ambientTempF, padTempF);
        __delay_cycles(1000000);  // 1 second delay
    }
}

void initializeGPIO() {
    P1DIR |= HEATER_CONTROL;
    P1OUT &= ~HEATER_CONTROL;
}

void initializeADC() {
    ADCCTL0 &= ~ADCENC;
    ADCCTL0 = ADCSHT_2 | ADCON;
    ADCCTL1 = ADCSHP;
    ADCCTL2 = ADCRES;
    ADCCTL0 |= ADCENC;
}

void initializePWM() {
    TB0CCR0 = 999;
    TB0CCR1 = 0;
    TB0CTL = TBSSEL_2 | MC_1;
}

void setPWM(int dutyCycle) {
    if (dutyCycle <= 0) {
        // Turn PWM OFF
        TB0CCTL1 = 0;
        P1SEL0 &= ~HEATER_CONTROL;
        P1SEL1 &= ~HEATER_CONTROL;
        P1DIR  |= HEATER_CONTROL;
        P1OUT  &= ~HEATER_CONTROL;
    } else {
        // Set PWM mode on P1.2
        P1SEL0 |= HEATER_CONTROL;
        P1SEL1 &= ~HEATER_CONTROL;
        P1DIR  |= HEATER_CONTROL;
        TB0CCTL1 = OUTMOD_7;
        TB0CCR1 = (dutyCycle * TB0CCR0) / 100;
    }
}

int readTemperature(int channel) {
    ADCMCTL0 = channel;
    ADCCTL0 |= ADCSC;
    while (ADCCTL1 & ADCBUSY);

    int adcValue = ADCMEM0;
    float voltage = (adcValue * 3.3f) / 1023.0f;
    float tempC = (voltage - 0.5f) * 100.0f;
    return (int)((tempC * 9.0f / 5.0f) + 32.0f);
}

void controlHeater(int ambientTemp, int padTemp) {
    int lowerThreshold = 0 - 2;
    int upperThreshold = 40 + 2;
    int heatPadLimit = 115;

    if (ambientTemp > lowerThreshold && ambientTemp < upperThreshold) {
        if (padTemp < heatPadLimit) {
            int Tmin = 68;
            int Tmax = 115;
            int dutyCycle = 90 * (1 - ((padTemp - Tmin) / (float)(Tmax - Tmin)));
            setPWM(dutyCycle);
        } else {
            setPWM(0);
        }
    } else {
        setPWM(0);
    }
}

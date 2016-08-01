// Particle Pokeball - Photon Firmware - Technobly 2016

/* PHOTON - Pololu TB6612FNG Dual Motor Driver Carrier (CONNECTIONS)
 * ==============================
 *   A1 - AIN2 (direction bits)
 *   A2 - AIN1
 *   A3 - /STBY (motor enable)
 *   A4 - PWMA (speed)
 *   VIN - VMOT
 *   3V3 - VCC
 *   GND - GND
 *
 * MOTOR - Pololu TB6612FNG Dual Motor Driver Carrier (CONNECTIONS)
 * ==============================
 *   Motor1 - AO1
 *   Motor2 - AO2
 */

#include "application.h"
#include <math.h>

#define STBY A3
#define FWD 0
#define RVS 1
#define AIN1 A2
#define AIN2 A1
#define PWM_PIN A4

int _speed = 0;
int _duration = 0;
int _direction = 75;
bool _new_command = false;

Servo myservo;

void setup() {
    Serial.begin(9600);

    int pin;
    for (pin=A1; pin<=A4; pin++) {
        pinMode(pin, OUTPUT);
    }
    for (pin=A1; pin<=A4; pin++) {
        digitalWrite(pin, LOW);
    }

    Particle.function("cmd", cmdRobot);
    myservo.attach(TX);  // attaches the servo on the TX pin to the servo object
    myservo.write(_direction);
    enableMotors(true);
    setSpeed(_speed);
}

void loop() {
    if (_new_command == true) {
        setSpeed(_speed);
        myservo.write(_direction);
        _new_command = false;
    }
}

int cmdRobot(String c) {
    // c format: ",xxx,yyy,"
    // xxx = direction (60 = left, 90 = center, 120 = right)
    // yyy = speed in percentage (-100 = full reverse, 100 = full forward)
    int start = 0;
    int end = 0;
    int speed = 0;
    int direction = 0;

    // Parse direction and speed out of command
    start = c.indexOf(',');
    if (start != -1) {
        end = c.indexOf(',', start+1);
        if (end == -1) {
            return 2;
        }
    }
    else {
        return 1;
    }

    direction = c.substring(start+1,end).toInt();

    start = end;
    end = c.indexOf(',', start+1);
    if (end == -1) {
        return 3;
    }

    speed = c.substring(start+1,end).toInt();

    Serial.print("Speed: ");
    Serial.println(speed);
    Serial.print("Direction: ");
    Serial.println(direction);

    if (speed > 0) {
        setDirection(FWD);
        Serial.println("F");
    }
    else if (speed < 0) {
        setDirection(RVS);
        Serial.println("R");
    }

    if (direction > 90) {
        Serial.println("R");
    }
    else if (direction < 90) {
        Serial.println("L");
    }

    _speed = abs(speed);
    _direction = direction;
    _new_command = true;

    return _speed;
}

void enableMotors(bool enable) {
    if (enable) digitalWrite(STBY, HIGH); // enable motor output
    else digitalWrite(STBY, LOW); // disable motor output
}

void setDirection(bool dir) {
    if (dir == RVS) {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
    }
    else if (dir == FWD) {
        digitalWrite(AIN2, LOW);
        digitalWrite(AIN1, HIGH);
    }
}

void setSpeed(int speed) {
    analogWrite(PWM_PIN, map(speed,0,100,0,255) );
}
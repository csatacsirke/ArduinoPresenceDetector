#include <Wire.h>

const int MPU = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
unsigned long previous_time = 0;
unsigned long current_time = 0;
unsigned long delta_time = 0;


void setup()
{
    Serial.begin(115200);

    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
}

void update_time() {

    current_time = millis();
    if(previous_time == 0) {
        previous_time = current_time;
    }

    delta_time = current_time - previous_time;  

    previous_time = current_time;
}

void update_gyro_info() {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 14, true);
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();

    const int16_t temperature = Wire.read() << 8 | Wire.read();

    GyX = Wire.read() << 8 | Wire.read();
    GyY = Wire.read() << 8 | Wire.read();
    GyZ = Wire.read() << 8 | Wire.read();
}

template<typename T, size_t container_size>
class Queue {
    
    size_t begin_index = 0;
    size_t end_index = 0;
    size_t size = 0;
public:
    T data[container_size];

    void Add(T value) {
        if(Size() + 1 >= container_size) {
            Pop();
        }

        data[end_index] = value;

        end_index += 1;
        if(end_index > container_size) {
            end_index = 0;
        }
        ++size;
    }

    void RemoveFirst() {
        begin_index += 1;
        if (begin_index > container_size) {
            begin_index = 0;
        }
        --size;
    }

    T& Peek() {
        return data[begin_index];
    }

    size_t Size() const {
        return size;
    }

};

struct Measurement {
    int16_t gyro_y;
    unsigned long delta_time;
};

//Queue<Measurement, 100> queue;

float degrees = 0;

const float millisecs_to_seconds = 0.001;

inline float LERP(float t, float a, float b) {
    return t * b + (1-t) * a;
} 

float FilterFunction(const float degrees) {
    constexpr float z0 = 0;
    constexpr float z1 = 90;
    constexpr float decay = 0.02;
    
    if(degrees < (z1 - z0) / 2) {
        // -> z0
        return degrees - (degrees - z0) * decay;
    } else {
        // -> z1
        return degrees + (z1 - degrees) * decay;
    }
    
}

float OverflowCorrection(float degrees) {
    if(degrees > 135) {
        return degrees - 90;
    }

    if(degrees < -45) {
        return degrees + 90;
    }

    return degrees;
}

void loop()
{
    update_time();
    update_gyro_info();
   
    //queue.Add({GyY, delta_time});
    const float angular_velocity_y = float(GyX) / 131.0f;
    const float delta_time_seconds = delta_time * millisecs_to_seconds;
    degrees += angular_velocity_y * delta_time_seconds;
    degrees = FilterFunction(degrees);
    degrees = OverflowCorrection(degrees);


    if(degrees < 10) {
        Serial.println("Jobbra");
    }

    if(degrees > 80) {
        Serial.println("Balra");
    }
    // Serial.print("Degrees = ");
    // Serial.println(degrees);

    // Serial.print("Accelerometer: ");
    // Serial.print("X = ");
    // Serial.print(AcX);
    // Serial.print(" | Y = ");
    // Serial.print(AcY);
    // Serial.print(" | Z = ");
    // Serial.println(AcZ);

    // Serial.print("Gyroscope: ");
    // Serial.print("X = ");
    // Serial.print(GyX);
    // Serial.print(" | Y = ");
    // Serial.print(GyY);
    // Serial.print(" | Z = ");
    // Serial.println(GyZ);
    // Serial.println(" ");
    //delay(333);
    delay(30);
}

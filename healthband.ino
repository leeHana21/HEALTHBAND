#include <SoftwareSerial.h>

#define R0 10000
#define T0 25 //room temperature
#define B 4200 //the coefficient of the thermistor
#define SERISR 10000 //seris resistor 10K
 

#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.

 

#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.  

#include <SPI.h>

#include <Wire.h>

#include <Adafruit_GFX.h>

#include <Adafruit_SSD1306.h>

#include <Adafruit_MLX90614.h>

#include <Adafruit_MPU6050.h>

#include <Adafruit_Sensor.h>

 

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

SoftwareSerial mySerial(9, 10); // RX, TX

Adafruit_MPU6050 mpu;

 

//  Variables

const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0

const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.

int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.

const int MPU = 0x68; //MPU 6050 의 I2C 기본 주소

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

unsigned long c_time;

int state = 0;

 

void setup() {

  Serial.begin(9600);

  mySerial.begin(9600);

 

  pulseSensor.analogInput(PulseWire);

  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.

  pulseSensor.setThreshold(Threshold);

 

  pinMode(2, OUTPUT);
pinMode(7, OUTPUT);
  Serial.println("Adafruit MLX90614 test");

 

  mlx.begin();

  while (!Serial)

    delay(500); // will pause Zero, Leonardo, etc until serial console opens

 

  Serial.println("Adafruit MPU6050 test!");

 

  // Try to initialize!

  if (!mpu.begin(0x68)) {

    Serial.println("Failed to find MPU6050 chip");

    while (1) {

      delay(10);

    }

  }

  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  mpu6050test();

 

 

  // For Serial Monitor

  //Serial.println("OLED FeatherWing ON");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally

  //if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

   // Serial.println(F("SSD1306 Not Connected"));

    //for (;;);   // SSD1306 에 주소할당이 되지 않으면 무한루프

//  }

 

  //display.println("able tech");

  //display.display();

  //delay(2000);

  // Double-check the "pulseSensor" object was created and "began" seeing a signal.

  

  if (pulseSensor.begin() && mlx.begin()) {

    Serial.print("we ready to check your health state : ");

    Serial.println("pulseSensor / temperature / gyro are all ready");  //This prints one time at Arduino power-up,  or on Arduino reset.

 

  }

 

 

}

 

void loop() {

  float thermistorRValue;
  thermistorRValue = 1023.0 / analogRead(A3) - 1;
  thermistorRValue = SERISR / thermistorRValue;
  float temperature;
  temperature = thermistorRValue / R0; // R/R0
  temperature = log(temperature);
  temperature = temperature / B;
  temperature = temperature + 1.0 / (T0 + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15 ;// kelvins to C
  int count =0;


  int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".

  // "myBPM" hold this BPM value now.

 

  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  //Serial.print("Rotation X: ");

  //Serial.print(g.gyro.x);
  if(g.gyro.x >0){
    count++;
    if(count == 1000){
      Serial.println(1);
    }
  }
//Serial.print(",");
//  Serial.print(", Y: ");

//  Serial.print(g.gyro.y);
//Serial.print(",");
//  Serial.print(", Z: ");

 // Serial.print(g.gyro.z);
//Serial.print(",");
//  Serial.println(" rad/s");

 

  //Serial.print("Temperature: ");

//  Serial.println(temperature);

//  Serial.println(" degC");

  //Serial.println("");

  delay(100);

 

  if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".

    //Serial.print("BPM: ");

    //Serial.println(myBPM);

  }

 

  if (myBPM > 100 && millis() - c_time < 3000 && state == 0) {

   

    digitalWrite(2, HIGH);

    delay(1000);

    state = 1;

    c_time  = millis();

  }

  if (mlx.readObjectTempC() > 37  && millis() - c_time < 3000 && state == 0) {

  

    digitalWrite(2, HIGH);

    delay(1000);

    state = 1;

    c_time  = millis();

  }

 

  if (g.gyro.x < 1 && g.gyro.y < 1 && g.gyro.z < 1 && millis() - c_time < 3000 && state == 0) {

    digitalWrite(2, HIGH);

    delay(1000);

    mySerial.print(1);

    state = 1;

    c_time  = millis();

  }

  if (state == 1 && millis() - c_time > 3000) {

    digitalWrite(2, LOW);

   delay(1000);

   // display.clearDisplay();

   // display.display();

    state = 0;

  }

}

 

void mpu6050test() {

  Serial.print("Accelerometer range set to: ");

  switch (mpu.getAccelerometerRange()) {

    case MPU6050_RANGE_2_G:

      Serial.println("+-2G");

      break;

    case MPU6050_RANGE_4_G:

      Serial.println("+-4G");

      break;

    case MPU6050_RANGE_8_G:

      Serial.println("+-8G");
      
      break;

    case MPU6050_RANGE_16_G:

      Serial.println("+-16G");

      break;

  }

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  Serial.print("Gyro range set to: ");

  switch (mpu.getGyroRange()) {

    case MPU6050_RANGE_250_DEG:

      Serial.println("+- 250 deg/s");

      break;

    case MPU6050_RANGE_500_DEG:

      Serial.println("+- 500 deg/s");

      break;

    case MPU6050_RANGE_1000_DEG:

      Serial.println("+- 1000 deg/s");

      break;

    case MPU6050_RANGE_2000_DEG:

      Serial.println("+- 2000 deg/s");

      break;

  }

 

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.print("Filter bandwidth set to: ");

  switch (mpu.getFilterBandwidth()) {

    case MPU6050_BAND_260_HZ:

      Serial.println("260 Hz");

      break;

    case MPU6050_BAND_184_HZ:

      Serial.println("184 Hz");

      break;

    case MPU6050_BAND_94_HZ:

      Serial.println("94 Hz");

      break;

    case MPU6050_BAND_44_HZ:

      Serial.println("44 Hz");

      break;

    case MPU6050_BAND_21_HZ:

      Serial.println("21 Hz");

      break;

    case MPU6050_BAND_10_HZ:

      Serial.println("10 Hz");

      break;

    case MPU6050_BAND_5_HZ:

      Serial.println("5 Hz");

      break;

  }

 

  Serial.println("");

  delay(100);

}

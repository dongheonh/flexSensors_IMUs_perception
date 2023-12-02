// Include necessary libraries for I2C communication and sensor interfacing
#include <Wire.h> // Wire library for I2C communication
#include <Adafruit_Sensor.h> // Adafruit Unified Sensor Driver
#include <Adafruit_BNO055.h> // Adafruit BNO055 library

// Define the I2C address for the TCA9548A multiplexer
#define TCAADDR 0x70

// Create instances of the BNO055 sensor, all with the same I2C address
Adafruit_BNO055 bno1 = Adafruit_BNO055(55, 0x28); // Sensor instance 1
Adafruit_BNO055 bno2 = Adafruit_BNO055(55, 0x28); // Sensor instance 2
Adafruit_BNO055 bno3 = Adafruit_BNO055(55, 0x28); // Sensor instance 3

// If the calibration of bno055 is completed, boolean is true
bool calib_bno1 = 0;
bool calib_bno2 = 0;
bool calib_bno3 = 0;

// Once the calibration is done, calib_fnish is true
// If you don't want to conduct calibration set bool calib_fnish = 1
// bool calib_fnish = 0;
bool calib_fnish = 1;

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
  Wire.begin(); // Initialize the I2C communication

  // Initialize each BNO055 sensor through the multiplexer
  initializeSensor(bno1, 0); // Initialize the first sensor
  initializeSensor(bno2, 1); // Initialize the second sensor
  initializeSensor(bno3, 2); // Initialize the third sensor
}

void loop() {
  // If bno055s calibrations were done, move to the next step. But never come back to the calibration again
  if (calib_bno1 == 1 && calib_bno2 == 1 && calib_bno3 == 1) {
    calib_fnish = 1;
  }
  // If calibration was finished, get angular orientation at each iteration and send it to computer
  if (calib_fnish == 1) {
    // Create a variable to hold sensor event data
    sensors_event_t event;

    // Read and print the orientation data from each BNO055 sensor
    tcaselect(0); // Select the first BNO055 sensor
    bno1.getEvent(&event); // Get the orientation data
    printOrientation(event.orientation); // Print the orientation data

    tcaselect(1); // Repeat for the second BNO055 sensor
    bno2.getEvent(&event);
    Serial.print(", ");
    printOrientation(event.orientation);

    tcaselect(2); // Repeat for the third BNO055 sensor
    bno3.getEvent(&event);
    Serial.print(", ");
    printOrientation(event.orientation);

    // Read Flex sensor's voltage
    // Read analog values from pins A0, A1, and A2
    int valueA0 = analogRead(A0);
    int valueA1 = analogRead(A1);
    int valueA2 = analogRead(A2);

    // Print analog input values in CSV format (values from the flex sensors)
    Serial.print(",");
    Serial.print(valueA0);
    Serial.print(",");
    Serial.print(valueA1);
    Serial.print(",");
    Serial.println(valueA2); // 'println' adds a new line after printing valueA2

    delay(1); // Delay for 3 seconds before the next reading
  } else {
    // Continuously check and print calibration status for each sensor
    calib_bno1 = checkAndPrintCalibration(bno1, 0); // Check first sensor
    calib_bno2 = checkAndPrintCalibration(bno2, 1); // Check second sensor
    calib_bno3 = checkAndPrintCalibration(bno3, 2); // Check third sensor
  }
  delay(1000); // Wait for 5 seconds before checking again
}

void tcaselect(uint8_t i) {
  if (i > 7) return; // If channel number is out of range, do nothing
  Wire.beginTransmission(TCAADDR); // Begin I2C transmission to the TCA9548A
  Wire.write(1 << i); // Select the appropriate channel (0 to 7)
  Wire.endTransmission(); // End the I2C transmission
}

void initializeSensor(Adafruit_BNO055& bno, uint8_t channel) {
  tcaselect(channel); // Select the appropriate channel on the multiplexer
  if (!bno.begin()) { // Begin communication with the BNO055 sensor
    Serial.println("Failed to initialize BNO055 on channel " + String(channel)); // Print error message if initialization fails
    while (1); // Infinite loop to halt further execution
  }
  delay(1000); // Wait for a short time after initialization
  bno.setExtCrystalUse(true); // Use the external crystal for more accuracy
}

bool checkAndPrintCalibration(Adafruit_BNO055& bno, uint8_t channel) {
  bool calib_status = 0;
  tcaselect(channel); // Select the appropriate channel on the multiplexer
  uint8_t system, gyro, accel, mag; // Variables to store calibration status
  bno.getCalibration(&system, &gyro, &accel, &mag); // Get calibration status

  // Check if all components are fully calibrated (values should be 3)
  if (system < 3 || gyro < 3 || accel < 3 || mag < 3) {
    Serial.println();
    Serial.println(">>>> Sensor not fully calibrated");
    if (system < 3) {
      Serial.println("- System: not fully calibrated");
    }
    if (gyro < 3) {
      Serial.println("- Gyro: not fully calibrated");
    } 
    if (accel < 3) {
      Serial.println("- Accelerometer: not fully calibrated");
    }
    if (mag < 3) {
      Serial.println("- Magnometer: not fully calibrated");
    }
  
  } else {
    Serial.println(">>>> Sensor fully calibrated.");
    calib_status = 1;
    adafruit_bno055_offsets_t calibrationData; // Structure to hold calibration data
    bno.getSensorOffsets(calibrationData); // Get sensor offsets

    // Print accelerometer offsets
    Serial.print("Accelerometer: ");
    Serial.print(calibrationData.accel_offset_x); Serial.print(" ");
    Serial.print(calibrationData.accel_offset_y); Serial.print(" ");
    Serial.print(calibrationData.accel_offset_z); Serial.print(", ");

    Serial.print("gyro: ");
    Serial.print(calibrationData.gyro_offset_x); Serial.print(" ");
    Serial.print(calibrationData.gyro_offset_y); Serial.print(" ");
    Serial.print(calibrationData.gyro_offset_z); Serial.print(", ");
    
    Serial.print("magnometer: ");
    Serial.print(calibrationData.mag_offset_x); Serial.print(" ");
    Serial.print(calibrationData.mag_offset_y); Serial.print(" ");
    Serial.print(calibrationData.mag_offset_z); Serial.print(", ");

    Serial.println(); // Print a newline
    
  }
  return calib_status;
}

void printOrientation(sensors_vec_t orientation) {
  // Function to print orientation data
  Serial.print(orientation.x, 3);
  Serial.print(", ");
  Serial.print(orientation.y, 3);
  Serial.print(", ");
  Serial.print(orientation.z, 3);
}
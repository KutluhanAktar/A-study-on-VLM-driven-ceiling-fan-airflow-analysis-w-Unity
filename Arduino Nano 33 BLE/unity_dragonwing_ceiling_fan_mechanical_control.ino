         /////////////////////////////////////////////  
        //   A study on VLM-driven ceiling fan     //
       //       airflow analysis w/ Unity         //
      //             ---------------             //
     //          (Arduino Nano 33 BLE)          //           
    //             by Kutluhan Aktar           // 
   //                                         //
///////////////////////////////////////////////
//
//
//
// Utilizing VLMs to determine dual fan configurations for optimum cooling based on LiDAR-oriented real-time airflow simulations on Unity.
//
// For more information:
// https://www.kutluhanaktar.com/projects/A_study_on_VLM_driven_ceiling_fan_airflow_analysis_w_Unity
//
//
// Connections
// Arduino Nano 33 BLE :
//                                BTS7960B DC Motor Driver (40A) [First]
// 5V      ------------------------ VCC
// GND     ------------------------ GND
// D2      ------------------------ R_PWM
// D3      ------------------------ L_PWM
// 3.3V    ------------------------ R_EN
// 3.3V    ------------------------ L_EN
//                                BTS7960B DC Motor Driver (40A) [Second]
// 5V      ------------------------ VCC
// GND     ------------------------ GND
// D4      ------------------------ R_PWM
// D5      ------------------------ L_PWM
// 3.3V    ------------------------ R_EN
// 3.3V    ------------------------ L_EN
//                                A4988 Driver Module [First]
// 3.3V    ------------------------ VDD
// GND     ------------------------ GND
// D6      ------------------------ DIR
// D7      ------------------------ STEP
//                                A4988 Driver Module [Second]
// 3.3V    ------------------------ VDD
// GND     ------------------------ GND
// D8      ------------------------ DIR
// D9      ------------------------ STEP
//                                A4988 Driver Module [Third]
// 3.3V    ------------------------ VDD
// GND     ------------------------ GND
// D10     ------------------------ DIR
// D11     ------------------------ STEP
//                                A4988 Driver Module [Fourth]
// 3.3V    ------------------------ VDD
// GND     ------------------------ GND
// D12     ------------------------ DIR
// D13     ------------------------ STEP
//                                Grove - Triple Color E-Ink Display (1.54")
// GND     ------------------------ GND
// 3.3V    ------------------------ VCC
// TX      ------------------------ RX
// RX      ------------------------ TX
//                                2 Channel Relay Module
// GND     ------------------------ GND
// A6      ------------------------ IN1
// A7      ------------------------ IN2
// 5V      ------------------------ VCC
//                                KY-040 Rotary Encoder Module [First]
// GND     ------------------------ GND
// 3.3V    ------------------------ +
// A1      ------------------------ DT
// A0      ------------------------ CLK
//                                KY-040 Rotary Encoder Module [Second]
// GND     ------------------------ GND
// 3.3V    ------------------------ +
// A3      ------------------------ DT
// A2      ------------------------ CLK
//                                Motor Speed Sensor (LM393 Optical) [First]
// GND     ------------------------ GND
// A4      ------------------------ OUT
// 3.3V    ------------------------ VCC
//                                Motor Speed Sensor (LM393 Optical) [Second]
// GND     ------------------------ GND
// A5      ------------------------ OUT
// 3.3V    ------------------------ VCC


// Include the required libraries:
#include <ArduinoBLE.h>

// Import the custom interfaces to show them on the Grove - triple color e-ink display.
#include "interfaces.h"

// Declare the BLE service hosted by this device.
BLEService ceiling_fan("9755c88d-cae7-43c6-9013-16d456e05bf6");

// Declare BLE data characteristics and allow the remote device (central) to write, read, or notify.
BLEByteCharacteristic first_fan_speed("9756c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEByteCharacteristic first_fan_vertical("9757c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEByteCharacteristic first_fan_horizontal("9758c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEByteCharacteristic second_fan_speed("9759c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEByteCharacteristic second_fan_vertical("9760c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEByteCharacteristic second_fan_horizontal("9761c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEByteCharacteristic first_light("9762c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEByteCharacteristic second_light("9763c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEShortCharacteristic first_fan_status("9764c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLENotify);
BLEShortCharacteristic second_fan_status("9765c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLENotify);
BLEByteCharacteristic homing_status("9766c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);
BLEByteCharacteristic active_procedures("9767c88d-cae7-43c6-9013-16d456e05bf6", BLERead | BLEWrite);

// Define the motor speed sensor (LM393 Optical) pin configurations.
#define first_speed_sensor       A4
#define second_speed_sensor      A5

// Define the 2 channel relay module pin configurations.
#define relay_IN1                A6
#define relay_IN2                A7

// Declare a struct containing RS775 DC motor (12V - 12000 rpm) configurations.
struct fan_motors {
  static constexpr int motor_num = 2;
  int pins[motor_num][2] = {
    {D2, D3}, // R_PWM, L_PWM
    {D4, D5}
  };
  int speed_level[5] = {0, 50, 120, 180, 255};
  int first_motor_speed = 0,
      second_motor_speed = 0;
  float homing_angle_buffer[motor_num] = {0, 0};
  volatile boolean horizontally_homed[motor_num] = {false, false};
};

// Declare a struct containing Nema 17 (17HS3401) stepper motor configurations.
struct gear_motors {
  static constexpr int motor_num = 4;
  int pins[motor_num][2] = {
    {D6, D7}, // DIR, STEP
    {D8, D9},
    {D10, D11},
    {D12, D13}
  };
  int speed = 12000,
      stepsPerRevolution = 200;
  float oneDegreeStepX = 0.2;
  int oneDegreeAccX = 10;
  float oneDegreeStepY = 0.5;
  int oneDegreeAccY = 5;
  
      
};

// Declare a struct containing rotary encoder configurations.
struct encoder {
  static constexpr int encoder_num = 2;
  int pins[encoder_num][2] = {
    {A0, A1}, // CLK, DT
    {A2, A3}
  };
  volatile long ticks[encoder_num] = {0, 0};
  long last_ticks[encoder_num] = {0, 0};
  float angle[encoder_num] = {0, 0};
  const float ticks_per_rev = 20.0,
              gear_ratio = 2;
  volatile boolean buffer_given = false;
};

// Define data holders.
struct fan_motors fan_motors;
struct gear_motors gear_motors;
struct encoder encoder;
String current_first_motor_task = "idle",
       current_second_motor_task = "idle",
       latest_performed_task = "idle";
volatile boolean screen_active = true,
                 screen_change = false,
                 ble_phone_con = false,
                 ble_computer_con = false,
                 unity_active = false;


void setup() {

  Serial.begin(115200);
  delay(1000);

  // Initiate the hardware serial port (Serial1) to communicate with the triple color e-ink display.
  Serial1.begin(230400); 
  delay(1000);
  
  // Register pin configurations.
  pinMode(first_speed_sensor, INPUT_PULLUP); pinMode(second_speed_sensor, INPUT_PULLUP);
  pinMode(relay_IN1, OUTPUT); pinMode(relay_IN2, OUTPUT); digitalWrite(relay_IN1, HIGH); digitalWrite(relay_IN2, HIGH);

  // Initial pin configurations for the BTS7960B DC motor drivers to make RS775 DC motors idle at execution.
  for(int i=0; i<fan_motors.motor_num; i++){
    pinMode(fan_motors.pins[i][0], OUTPUT); analogWrite(fan_motors.pins[i][0], 0);
    pinMode(fan_motors.pins[i][1], OUTPUT); analogWrite(fan_motors.pins[i][1], 0);
  }

  // Initial pin configurations to activate the A4988 driver modules.
   for(int i=0; i<gear_motors.motor_num; i++){
    pinMode(gear_motors.pins[i][0], OUTPUT);
    pinMode(gear_motors.pins[i][1], OUTPUT);
  }

  // Initial pin configurations for the rotary encoders.
  for(int i=0; i<encoder.encoder_num; i++){
    pinMode(encoder.pins[i][0], INPUT_PULLUP);
    pinMode(encoder.pins[i][1], INPUT_PULLUP);
  }  

  // Show the default states on the triple color e-ink display.
  if(screen_active) show_screen_e_ink(ble_phone_con, ble_computer_con, unity_active);

  // Initiate the BLE communication procedure.
  while(!BLE.begin()){
    Serial.println("BLE initialization is failed!");
  }
  Serial.println("\nBLE initialization is successful!\n");
  // Print this peripheral device's address information:
  Serial.print("MAC Address: "); Serial.println(BLE.address());
  Serial.print("Service UUID Address: "); Serial.println(ceiling_fan.uuid()); Serial.println();

  // Set the local name this peripheral advertises, which will be shown to central devices.
  BLE.setLocalName("Dragonwing Ceiling Fan");
  // Set the primary service UUID.
  BLE.setAdvertisedService(ceiling_fan);
  // Add the declared BLE data characteristics to the defined primary service.
  ceiling_fan.addCharacteristic(first_fan_speed);
  ceiling_fan.addCharacteristic(first_fan_vertical);
  ceiling_fan.addCharacteristic(first_fan_horizontal);
  ceiling_fan.addCharacteristic(second_fan_speed);
  ceiling_fan.addCharacteristic(second_fan_vertical);
  ceiling_fan.addCharacteristic(second_fan_horizontal);
  ceiling_fan.addCharacteristic(first_light);
  ceiling_fan.addCharacteristic(second_light);
  ceiling_fan.addCharacteristic(first_fan_status);
  ceiling_fan.addCharacteristic(second_fan_status);
  ceiling_fan.addCharacteristic(homing_status);
  ceiling_fan.addCharacteristic(active_procedures);
  // Register the primary service.
  BLE.addService(ceiling_fan);

  // Assign event handlers for connected and disconnected central devices to/from this peripheral.
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // Assign event handlers for the BLE data characteristics modified (written) by the connected central device.
  // Then, perform the requested task accordingly.
  first_fan_speed.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  first_fan_vertical.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  first_fan_horizontal.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  second_fan_speed.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  second_fan_vertical.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  second_fan_horizontal.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  first_light.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  second_light.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  homing_status.setEventHandler(BLEWritten, perform_tasks_via_BLE);
  active_procedures.setEventHandler(BLEWritten, perform_tasks_via_BLE);

  // Start advertising with the concluded device settings.
  BLE.advertise();
  Serial.println(("BLE connection is active, waiting for the central device..."));
  delay(5000);

  // Attach seperate interrupt handlers for the rotary encoders.
  attachInterrupt(digitalPinToInterrupt(encoder.pins[0][0]), []() { obtain_encoder_values(encoder.pins[0][0], encoder.pins[0][1], encoder.ticks[0]); }, CHANGE); // Using lambda to pass variables.
  attachInterrupt(digitalPinToInterrupt(encoder.pins[1][0]), []() { obtain_encoder_values(encoder.pins[1][0], encoder.pins[1][1], encoder.ticks[1]); }, CHANGE);

}

void loop() {

  // Estimate the angle (0° - 360°) values of the rotary encoders.
   for(int i=0; i<encoder.encoder_num; i++){
    if(encoder.ticks[i] != encoder.last_ticks[i]){
        encoder.last_ticks[i] = encoder.ticks[i];
        float raw_angle = (encoder.ticks[i] / encoder.ticks_per_rev) * 360.0 / encoder.gear_ratio;
        // Remove the provided horizontal motor buffer angle to zero the encoder position based on the homing speed sensor position.
        encoder.angle[i] = fmod(raw_angle - fan_motors.homing_angle_buffer[i], 360.0);
        if(encoder.angle[i] < 0) encoder.angle[i] += 360.0;
        // After estimating the angle successfully, update the fan status information over BLE.
        int16_t angle = (int16_t)encoder.angle[i];
        if(i == 0){ first_fan_status.writeValue(angle); }
        else if(i == 1){ second_fan_status.writeValue(angle); }
    }   
  }

  // Once the motor buffer angles are set, update the fan status information over BLE immediately, which lets the user see the zeroed encoder angle values without turning encoder knobs.
  if(encoder.buffer_given){
    for(int i=0; i<encoder.encoder_num; i++){
      // Estimate the current angle to precisely zero the encoder position.
      float raw_angle = (encoder.ticks[i] / encoder.ticks_per_rev) * 360.0 / encoder.gear_ratio;
      encoder.angle[i] = fmod(raw_angle - fan_motors.homing_angle_buffer[i], 360.0);
      if(encoder.angle[i] < 0) encoder.angle[i] += 360.0;      
    }
    first_fan_status.writeValue((int16_t)encoder.angle[0]); second_fan_status.writeValue((int16_t)encoder.angle[1]);
    encoder.buffer_given = false;
  }

  // Once the motor speed sensors detect the homing pins, update the fan status information immediately.
  if(digitalRead(first_speed_sensor) && !fan_motors.horizontally_homed[0]){
    first_fan_status.writeValue(-1);
    fan_motors.horizontally_homed[0] = true;
  }else if(!digitalRead(first_speed_sensor) && fan_motors.horizontally_homed[0]){
    first_fan_status.writeValue(-2);
    fan_motors.horizontally_homed[0] = false;
  }
  if(digitalRead(second_speed_sensor) && !fan_motors.horizontally_homed[1]){
    second_fan_status.writeValue(-1);
    fan_motors.horizontally_homed[1] = true;
  }else if(!digitalRead(second_speed_sensor) && fan_motors.horizontally_homed[1]){
    second_fan_status.writeValue(-2);
    fan_motors.horizontally_homed[1] = false;
  }    
 
  // Once the operation states change, update the triple color e-ink display.
  if(screen_change && screen_active){
    show_screen_e_ink(ble_phone_con, ble_computer_con, unity_active);
    screen_change = false;
  }

  // To avoid interruption issues, seperate task from the primary BLE poll loop.
  /* First (Left) ceiling fan motor tasks. */
  if(current_first_motor_task != "idle"){
    if(current_first_motor_task == "one_degree_down" && latest_performed_task != "one_degree_down"){
      driver_bevel_gear_move(0, gear_motors.oneDegreeStepY, gear_motors.oneDegreeAccY, "TD", "D_R");
      latest_performed_task = "one_degree_down";
    }
    if(current_first_motor_task == "one_degree_up" && latest_performed_task != "one_degree_up"){
      driver_bevel_gear_move(0, gear_motors.oneDegreeStepY, gear_motors.oneDegreeAccY, "TD", "U_L");
      latest_performed_task = "one_degree_up";
    }    
    if(current_first_motor_task == "one_degree_right" && latest_performed_task != "one_degree_right"){
      driver_bevel_gear_move(0, gear_motors.oneDegreeStepX, gear_motors.oneDegreeAccX, "OP", "D_R");
      latest_performed_task = "one_degree_right";
    } 
    if(current_first_motor_task == "one_degree_left" && latest_performed_task != "one_degree_left"){
      driver_bevel_gear_move(0, gear_motors.oneDegreeStepX, gear_motors.oneDegreeAccX, "OP", "U_L");
      latest_performed_task = "one_degree_left";
    }
    if(current_first_motor_task == "speed_update"){
      analogWrite(fan_motors.pins[0][0], fan_motors.first_motor_speed);
      analogWrite(fan_motors.pins[0][1], 0);
    }
    current_first_motor_task = "idle";
    delay(200);                 
  }

  /* Second (Right) ceiling fan motor tasks. */
  if(current_second_motor_task != "idle"){
    if(current_second_motor_task == "one_degree_down" && latest_performed_task != "one_degree_down"){
      driver_bevel_gear_move(2, gear_motors.oneDegreeStepY, gear_motors.oneDegreeAccY, "TD", "D_R");
      latest_performed_task = "one_degree_down";
    }
    if(current_second_motor_task == "one_degree_up" && latest_performed_task != "one_degree_up"){
      driver_bevel_gear_move(2, gear_motors.oneDegreeStepY, gear_motors.oneDegreeAccY, "TD", "U_L");
      latest_performed_task = "one_degree_up";
    }    
    if(current_second_motor_task == "one_degree_right" && latest_performed_task != "one_degree_right"){
      driver_bevel_gear_move(2, gear_motors.oneDegreeStepX, gear_motors.oneDegreeAccX, "OP", "D_R");
      latest_performed_task = "one_degree_right";
    } 
    if(current_second_motor_task == "one_degree_left" && latest_performed_task != "one_degree_left"){
      driver_bevel_gear_move(2, gear_motors.oneDegreeStepX, gear_motors.oneDegreeAccX, "OP", "U_L");
      latest_performed_task = "one_degree_left";
    }
    if(current_second_motor_task == "speed_update"){
      analogWrite(fan_motors.pins[1][0], 0);
      analogWrite(fan_motors.pins[1][1], fan_motors.second_motor_speed);
    }
    current_second_motor_task = "idle";
    delay(200);                 
  }  

  // Initiate the built-in loop of the BLE class (poll) to process BLE events.
  BLE.poll();

}

void driver_bevel_gear_move(int gear_set, float step_number, int acc, String _state, String _dir){
  /*
      Move the requested driver bevel gear set by controlling the rotation of the associated stepper motors.
      Gear Set State:
        Tandem [TD]: rotate the target stepper motors at the same direction at the same speed.
        Opposite [OP]: rotate the target stepper motors at opposite directions at the same speed.
      Gear Set Direction:
        Up or Left [U_L]: if the gear state is tandem, mechanism moves left. if the gear state is opposite, the mechanism moves up.
        Down or Right [D_R]: if the gear state is tandem, mechanism moves right. if the gear state is opposite, the mechanism moves down.
      Gear Set Numbers:
        0: First (Left) ceiling fan driver bevel gears.
        2: Second (Right) ceiling fan driver bevel gears.
  */
  if(_state == "TD" && _dir == "U_L"){
    digitalWrite(gear_motors.pins[gear_set][0], LOW);
    digitalWrite(gear_motors.pins[gear_set+1][0], HIGH);
  }
  else if(_state == "TD" && _dir == "D_R"){
    digitalWrite(gear_motors.pins[gear_set][0], HIGH);
    digitalWrite(gear_motors.pins[gear_set+1][0], LOW);
  }  
  if(_state == "OP" && _dir == "U_L"){
    digitalWrite(gear_motors.pins[gear_set][0], HIGH);
    digitalWrite(gear_motors.pins[gear_set+1][0], HIGH);
  }
  else if(_state == "OP" && _dir == "D_R"){
    digitalWrite(gear_motors.pins[gear_set][0], LOW);
    digitalWrite(gear_motors.pins[gear_set+1][0], LOW);
  } 

  // Calculate the total rotation number to achieve the requested step number based on the defined revolution number.
  float rotation_number = gear_motors.stepsPerRevolution * step_number;
  // Then, rotate the target stepper motors accordingly.
  for(int i = 0; i < (int)rotation_number; i++){
    digitalWrite(gear_motors.pins[gear_set][1], HIGH);
    digitalWrite(gear_motors.pins[gear_set+1][1], HIGH);
    delayMicroseconds(gear_motors.speed/acc);
    digitalWrite(gear_motors.pins[gear_set][1], LOW);
    digitalWrite(gear_motors.pins[gear_set+1][1], LOW);
    delayMicroseconds(gear_motors.speed/acc);
  }  

}

void obtain_encoder_values(int clkPin, int dtPin, volatile long &ticks){
  // Update the tick value of the given rotary encoder.
  if(digitalRead(clkPin) != digitalRead(dtPin)){
    ticks++;
  }else{
    ticks--;
  }
}

void perform_tasks_via_BLE(BLEDevice central, BLECharacteristic characteristic){
  /* First (Left) ceiling fan motor configurations. */
  if(characteristic.uuid() == first_fan_vertical.uuid()){
    // Adjust the vertical angle of the target ceiling fan base.
    switch(first_fan_vertical.value()){
      case 1:
        current_first_motor_task = "one_degree_down";
      break;
      case 2:
        current_first_motor_task = "one_degree_up";
      break;     
    }
  }
  if(characteristic.uuid() == first_fan_horizontal.uuid()){
    // Adjust the horizontal angle of the target ceiling fan base.
    switch(first_fan_horizontal.value()){
      case 1:
        current_first_motor_task = "one_degree_right";
      break;
      case 2:
        current_first_motor_task = "one_degree_left";
      break;     
    }
  }  
  if(characteristic.uuid() == first_fan_speed.uuid()){
    // Rotate the target ceiling fan motor according to the given fan motor speed.
    current_first_motor_task = "speed_update";
    fan_motors.first_motor_speed = fan_motors.speed_level[first_fan_speed.value()];
    // Clear configurations.
    if(first_fan_speed.value() == 0) latest_performed_task = "idle";
  }

  /* Second (Right) ceiling fan motor configurations. */
  if(characteristic.uuid() == second_fan_vertical.uuid()){
    // Adjust the vertical angle of the target ceiling fan base.
    switch(second_fan_vertical.value()){
      case 1:
        current_second_motor_task = "one_degree_down";
      break;
      case 2:
        current_second_motor_task = "one_degree_up";
      break;     
    }
  }
  if(characteristic.uuid() == second_fan_horizontal.uuid()){
    // Adjust the horizontal angle of the target ceiling fan base.
    switch(second_fan_horizontal.value()){
      case 1:
        current_second_motor_task = "one_degree_right";
      break;
      case 2:
        current_second_motor_task = "one_degree_left";
      break;     
    }
  }  
  if(characteristic.uuid() == second_fan_speed.uuid()){
    // Rotate the target ceiling fan motor according to the given fan motor speed.
    current_second_motor_task = "speed_update";
    fan_motors.second_motor_speed = fan_motors.speed_level[second_fan_speed.value()];
    // Clear configurations.
    if(second_fan_speed.value() == 0) latest_performed_task = "idle";
  }

  /* Cooling fan homing adjustments. */
  if(characteristic.uuid() == homing_status.uuid()){
    switch(homing_status.value()){
      case 1:
        for(int i=0; i<fan_motors.motor_num; i++){
          fan_motors.homing_angle_buffer[i] = encoder.angle[i];
          encoder.buffer_given = true;
        }
      break;    
    }
  } 

  /* Dual ceiling lights configurations. */
  if(characteristic.uuid() == first_light.uuid()){
    // Change the state of the target ceiling light.
    digitalWrite(relay_IN1, first_light.value()-1);
  }
  if(characteristic.uuid() == second_light.uuid()){
    // Change the state of the target ceiling light.
    digitalWrite(relay_IN2, second_light.value()-1);
  }

  /* Screen configurations by the connected device type. */
  if(characteristic.uuid() == active_procedures.uuid()){
    switch(active_procedures.value()){
      case 1:
        ble_phone_con = true;
      break;
      case 2:
        ble_phone_con = false;
      break;
      case 3:
        unity_active = true;
      break;
      case 4:
        unity_active = false;
      break;                 
    }
    screen_change = true;    
  }      
  
}

void show_screen_e_ink(bool phone_ble, bool computer_ble, bool unity_analysis){
  // Interface configurations. 
  e_ink_send_begin();
  delay(2000);  
  if(phone_ble && computer_ble && unity_analysis){
    e_ink_write_image_picture(black_layer_bg, red_layer_all);
  }else if(!phone_ble && computer_ble && unity_analysis){
    e_ink_write_image_picture(black_layer_bg, red_layer_computer_unity);
  }else if(phone_ble && computer_ble && !unity_analysis){
    e_ink_write_image_picture(black_layer_bg, red_layer_phone_computer);
  }else if(phone_ble && !computer_ble && !unity_analysis){
    e_ink_write_image_picture(black_layer_bg, red_layer_phone);
  }else if(!phone_ble && computer_ble && !unity_analysis){
    e_ink_write_image_picture(black_layer_bg, red_layer_computer);
  }else if(!phone_ble && !computer_ble && !unity_analysis){
    e_ink_write_image_picture(black_layer_bg, red_layer_none);
  }  
  delay(2000);
}

void e_ink_serial_send_data(const uint8_t* data, uint32_t data_len){
    for (int i = 0; i < data_len; i++) {
        Serial1.write(pgm_read_byte(&data[i]));
    }
}

void e_ink_write_image_picture(const uint8_t* black_buffer, const uint8_t* red_buffer){
    for (int i = 0; i < 38; i++) {
        e_ink_serial_send_data(&black_buffer[i * 76], 76);
        delay(70);
    }
    delay(70);
    for (int i = 0; i < 38; i++) {
        e_ink_serial_send_data(&red_buffer[i * 76], 76);
        delay(70);
    }
}

void e_ink_send_begin(){
    char str = 'a';
    Serial1.write(str);
    while(1) {
        if (Serial1.available() > 0) {
            delay(10);
            char str1 = Serial1.read();
            //Serial.print(str1);
            if (str1 == 'b') {
                Serial.println("\nE-ink: Screen update success!");
                break;
            }
        }
    }
}

void blePeripheralConnectHandler(BLEDevice central) {
  // Central connected event handler:
  Serial.print("\nConnected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // Central disconnected event handler:
  Serial.print("\nDisconnected event, central: ");
  Serial.println(central.address());
}

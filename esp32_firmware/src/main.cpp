/*#include <Arduino.h>
#include <DHT.h>

const int DHTPIN = 4;

DHT dht(DHTPIN, DHT22);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("DHT22 Sensor Test");
}

void loop() {
  float temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  delay(1000);
}*/

/*#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;

void setup() {
  Serial.begin(115200);
  set_microros_serial_transports(Serial);
  delay(2000);

  allocator = rcl_get_default_allocator();

  rclc_support_init(&support, 0, NULL, &allocator);

  rclc_node_init_default(&node, "stud_krystych_esp32", "", &support);

  rclc_executor_init(&executor, &support.context, 1, &allocator);
}

void loop() {
  delay(100);
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
}*/

#include <Arduino.h>
#include <DHT.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>

const int DHTPIN = 4;

DHT dht(DHTPIN, DHT22);

rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;
rcl_publisher_t publisher_celsius;
rcl_publisher_t publisher_fahrenheit;

std_msgs__msg__Float32 temperature_celsius_msg;
std_msgs__msg__Float32 temperature_fahrenheit_msg;

unsigned long last_publish_time = 0;
const unsigned long PUBLISH_INTERVAL = 1000;

void setupSensor()
{
  dht.begin();
}

float readTemperature()
{
  return dht.readTemperature();
}

float celsiusToFahrenheit(float celsius)
{
  return celsius * 9.0f / 5.0f + 32.0f;
}

void setupMicroROS()
{
  Serial.begin(115200);

  set_microros_serial_transports(Serial);

  delay(2000);

  allocator = rcl_get_default_allocator();

  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "stud_krystych_esp32", "", &support);
  rclc_publisher_init_default(&publisher_celsius, &node,
                              ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "/stud_krystych/temperature/celsius");

  rclc_publisher_init_default(&publisher_fahrenheit, &node,
                              ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "/stud_krystych/temperature/fahrenheit");

  rclc_executor_init(&executor, &support.context, 1, &allocator);
}

void publishTemperature()
{
  float temperature_celsius = readTemperature();

  if (isnan(temperature_celsius))
  {
    return;
  }

  float temperature_fahrenheit = celsiusToFahrenheit(temperature_celsius);

  temperature_celsius_msg.data = temperature_celsius;
  temperature_fahrenheit_msg.data = temperature_fahrenheit;

  rcl_publish(&publisher_celsius, &temperature_celsius_msg, NULL);
  rcl_publish(&publisher_fahrenheit, &temperature_fahrenheit_msg, NULL);
}

void setup()
{
  setupSensor();
  setupMicroROS();
}

void loop()
{
  unsigned long current_time = millis();

  if (current_time - last_publish_time >= PUBLISH_INTERVAL)
  {
    last_publish_time = current_time;

    publishTemperature();
  }

  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
}

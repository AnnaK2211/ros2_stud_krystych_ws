#include <Arduino.h>
#include <DHT.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include <rmw_microros/rmw_microros.h>

constexpr int DHT_PIN = 4;

DHT dht(DHT_PIN, DHT22);

rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;
rcl_publisher_t publisher_celsius;
rcl_publisher_t publisher_fahrenheit;
rcl_timer_t timer;

std_msgs__msg__Float32 temperature_celsius_msg;
std_msgs__msg__Float32 temperature_fahrenheit_msg;

constexpr unsigned long PUBLISH_INTERVAL = 1000;
constexpr unsigned long MICROROS_STARTUP_DELAY = 2000;

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

void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
  (void)last_call_time;

  if (timer != NULL)
  {
    publishTemperature();
  }
}

bool setupMicroROS()
{
  Serial.begin(115200);

  set_microros_serial_transports(Serial);

  delay(MICROROS_STARTUP_DELAY);

  allocator = rcl_get_default_allocator();

  if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK)
  {
    return false;
  }

  if (rclc_node_init_default(&node, "stud_krystych_esp32", "", &support) != RCL_RET_OK)
  {
    return false;
  }

  if (rclc_publisher_init_default(
          &publisher_celsius,
          &node,
          ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
          "/stud_krystych/temperature/celsius") != RCL_RET_OK)
  {
    return false;
  }

  if (rclc_publisher_init_default(
          &publisher_fahrenheit,
          &node,
          ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
          "/stud_krystych/temperature/fahrenheit") != RCL_RET_OK)
  {
    return false;
  }

  if (rclc_timer_init_default(
          &timer,
          &support,
          RCL_MS_TO_NS(PUBLISH_INTERVAL),
          timer_callback) != RCL_RET_OK)
  {
    return false;
  }

  if (rclc_executor_init(&executor, &support.context, 1, &allocator) != RCL_RET_OK)
  {
    return false;
  }

  if (rclc_executor_add_timer(&executor, &timer) != RCL_RET_OK)
  {
    return false;
  }

  return true;
}

bool checkMicroROSConnection()
{
  return rmw_uros_ping_agent(100, 1) == RMW_RET_OK;
}

void setup()
{
  setupSensor();
  setupMicroROS();
}

void loop()
{
  if (!checkMicroROSConnection())
  {
    setupMicroROS();
    return;
  }

  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
}

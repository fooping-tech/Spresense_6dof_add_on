// micro-ROS
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <sensor_msgs/msg/imu.h>
#include <builtin_interfaces/msg/time.h>
#include <math.h>

static rcl_allocator_t allocator;
static rclc_support_t support;
static rcl_node_t node;
static rcl_publisher_t publisher;
sensor_msgs__msg__Imu imu_msg;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


void error_loop() {
  while (1) {
    digitalWrite(LED0, !digitalRead(LED0));
    delay(100);
  }
}

// IMU
#include "BMI270_Arduino.h"
BMI270Class BMI270;

void IMU_print_rslt(int8_t rslt) {
  switch (rslt) {
    case BMI2_OK: return;
    default:
      Serial.println("Error [" + String(rslt) + "] : Unknown error code");
      break;
  }
}

int8_t IMU_configure_sensor() {
  int8_t rslt;
  uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };
  struct bmi2_sens_config config[2];

  // 加速度センサー設定
  config[0].type = BMI2_ACCEL;
  config[0].cfg.acc.odr = BMI2_ACC_ODR_200HZ;
  config[0].cfg.acc.range = BMI2_ACC_RANGE_2G; // ±2G設定
  config[0].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;
  config[0].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

  // ジャイロセンサー設定
  config[1].type = BMI2_GYRO;
  config[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
  config[1].cfg.gyr.range = BMI2_GYR_RANGE_2000; // ±2000 dps設定
  config[1].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;
  config[1].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;
  config[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

  rslt = BMI270.set_sensor_config(config, 2);
  if (rslt != BMI2_OK) return rslt;

  rslt = BMI270.sensor_enable(sens_list, 2);
  return rslt;
}


void microRosInit() {
  set_microros_transports();
  
  delay(2000);

  allocator = rcl_get_default_allocator();

  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "my_imu_node", "", &support));
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
    "imu_spresense"));

  // IMUメッセージの初期化
  imu_msg.header.frame_id.data = (char*)"imu_link";
  imu_msg.header.frame_id.size = strlen("imu_link");
  imu_msg.header.frame_id.capacity = strlen("imu_link");

  // オリエンテーションを無効化
  imu_msg.orientation_covariance[0] = -1.0f;

  // 角速度の共分散行列を未知（0）として設定
  for(int i = 0; i < 9; i++) {
    imu_msg.angular_velocity_covariance[i] = 0.0f;
  }

  // 線形加速度の共分散行列を未知（0）として設定
  for(int i = 0; i < 9; i++) {
    imu_msg.linear_acceleration_covariance[i] = 0.0f;
  }

  imu_msg.linear_acceleration.x = imu_msg.linear_acceleration.y = imu_msg.linear_acceleration.z = 0.0f;
  imu_msg.angular_velocity.x = imu_msg.angular_velocity.y = imu_msg.angular_velocity.z = 0.0f;

  digitalWrite(LED0, HIGH);  
}

void setup() {
  // IMU初期化
  int8_t rslt = BMI270.begin(BMI270_I2C, BMI2_I2C_SEC_ADDR);
  IMU_print_rslt(rslt);

  rslt = IMU_configure_sensor();
  IMU_print_rslt(rslt);

  // micro-ROS初期化
  microRosInit();
}


void loop() {
  struct bmi2_sens_float sensor_data;

  int8_t rslt = BMI270.bmi2_get_sensor_float(&sensor_data);

  if (rslt == BMI2_OK) {
    // ヘッダーのタイムスタンプを更新
    imu_msg.header.stamp.sec = millis() / 1000;
    imu_msg.header.stamp.nanosec = (millis() % 1000) * 1000000;

    // 加速度データ（m/s²）に変換
    imu_msg.linear_acceleration.x = sensor_data.acc.x / 16384.0f * 9.80665f;
    imu_msg.linear_acceleration.y = sensor_data.acc.y / 16384.0f * 9.80665f;
    imu_msg.linear_acceleration.z = sensor_data.acc.z / 16384.0f * 9.80665f;

    // ジャイロデータ（rad/s）に変換
    imu_msg.angular_velocity.x = sensor_data.gyr.x / 16.4f * (3.141592653589793f / 180.0f);
    imu_msg.angular_velocity.y = sensor_data.gyr.y / 16.4f * (3.141592653589793f /180.0f);
    imu_msg.angular_velocity.z = sensor_data.gyr.z /16.4f * (3.141592653589793f /180.0f);

    RCSOFTCHECK(rcl_publish(&publisher, &imu_msg, NULL));
    
    delay(100);
  }
}
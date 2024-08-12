//IMU
#include "BMI270_Arduino.h"
BMI270Class BMI270;


void IMU_print_rslt(int8_t rslt)
{
  switch (rslt)
  {
    case BMI2_OK: return; /* Do nothing */ break;
    case BMI2_E_NULL_PTR:
      Serial.println("Error [" + String(rslt) + "] : Null pointer");
      break;
    case BMI2_E_COM_FAIL:
      Serial.println("Error [" + String(rslt) + "] : Communication failure");
      break;
    case BMI2_E_DEV_NOT_FOUND:
      Serial.println("Error [" + String(rslt) + "] : Device not found");
      break;
    case BMI2_E_OUT_OF_RANGE:
      Serial.println("Error [" + String(rslt) + "] : Out of range");
      break;
    case BMI2_E_ACC_INVALID_CFG:
      Serial.println("Error [" + String(rslt) + "] : Invalid accel configuration");
      break;
    case BMI2_E_GYRO_INVALID_CFG:
      Serial.println("Error [" + String(rslt) + "] : Invalid gyro configuration");
      break;
    case BMI2_E_ACC_GYR_INVALID_CFG:
      Serial.println("Error [" + String(rslt) + "] : Invalid accel/gyro configuration");
      break;
    case BMI2_E_INVALID_SENSOR:
      Serial.println("Error [" + String(rslt) + "] : Invalid sensor");
      break;
    case BMI2_E_CONFIG_LOAD:
      Serial.println("Error [" + String(rslt) + "] : Configuration loading error");
      break;
    case BMI2_E_INVALID_PAGE:
      Serial.println("Error [" + String(rslt) + "] : Invalid page ");
      break;
    case BMI2_E_INVALID_FEAT_BIT:
      Serial.println("Error [" + String(rslt) + "] : Invalid feature bit");
      break;
    case BMI2_E_INVALID_INT_PIN:
      Serial.println("Error [" + String(rslt) + "] : Invalid interrupt pin");
      break;
    case BMI2_E_SET_APS_FAIL:
      Serial.println("Error [" + String(rslt) + "] : Setting advanced power mode failed");
      break;
    case BMI2_E_AUX_INVALID_CFG:
      Serial.println("Error [" + String(rslt) + "] : Invalid auxilliary configuration");
      break;
    case BMI2_E_AUX_BUSY:
      Serial.println("Error [" + String(rslt) + "] : Auxilliary busy");
      break;
    case BMI2_E_SELF_TEST_FAIL:
      Serial.println("Error [" + String(rslt) + "] : Self test failed");
      break;
    case BMI2_E_REMAP_ERROR:
      Serial.println("Error [" + String(rslt) + "] : Remapping error");
      break;
    case BMI2_E_GYR_USER_GAIN_UPD_FAIL:
      Serial.println("Error [" + String(rslt) + "] : Gyro user gain update failed");
      break;
    case BMI2_E_SELF_TEST_NOT_DONE:
      Serial.println("Error [" + String(rslt) + "] : Self test not done");
      break;
    case BMI2_E_INVALID_INPUT:
      Serial.println("Error [" + String(rslt) + "] : Invalid input");
      break;
    case BMI2_E_INVALID_STATUS:
      Serial.println("Error [" + String(rslt) + "] : Invalid status");
      break;
    case BMI2_E_CRT_ERROR:
      Serial.println("Error [" + String(rslt) + "] : CRT error");
      break;
    case BMI2_E_ST_ALREADY_RUNNING:
      Serial.println("Error [" + String(rslt) + "] : Self test already running");
      break;
    case BMI2_E_CRT_READY_FOR_DL_FAIL_ABORT:
      Serial.println("Error [" + String(rslt) + "] : CRT ready for DL fail abort");
      break;
    case BMI2_E_DL_ERROR:
      Serial.println("Error [" + String(rslt) + "] : DL error");
      break;
    case BMI2_E_PRECON_ERROR:
      Serial.println("Error [" + String(rslt) + "] : PRECON error");
      break;
    case BMI2_E_ABORT_ERROR:
      Serial.println("Error [" + String(rslt) + "] : Abort error");
      break;
    case BMI2_E_GYRO_SELF_TEST_ERROR:
      Serial.println("Error [" + String(rslt) + "] : Gyro self test error");
      break;
    case BMI2_E_GYRO_SELF_TEST_TIMEOUT:
      Serial.println("Error [" + String(rslt) + "] : Gyro self test timeout");
      break;
    case BMI2_E_WRITE_CYCLE_ONGOING:
      Serial.println("Error [" + String(rslt) + "] : Write cycle ongoing");
      break;
    case BMI2_E_WRITE_CYCLE_TIMEOUT:
      Serial.println("Error [" + String(rslt) + "] : Write cycle timeout");
      break;
    case BMI2_E_ST_NOT_RUNING:
      Serial.println("Error [" + String(rslt) + "] : Self test not running");
      break;
    case BMI2_E_DATA_RDY_INT_FAILED:
      Serial.println("Error [" + String(rslt) + "] : Data ready interrupt failed");
      break;
    case BMI2_E_INVALID_FOC_POSITION:
      Serial.println("Error [" + String(rslt) + "] : Invalid FOC position");
      break;
    default:
      Serial.println("Error [" + String(rslt) + "] : Unknown error code");
      break;
  }
}


int8_t IMU_configure_sensor()
{
  int8_t rslt;
  uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };

  struct bmi2_sens_config config[2];

  /* Configure the type of feature. */
  config[0].type = BMI2_ACCEL;

  /* NOTE: The user can change the following configuration parameters according to their requirement. */
  /* Set Output Data Rate */
  config[0].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

  /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
  config[0].cfg.acc.range = BMI2_ACC_RANGE_2G;

  /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
   * if it is set to 2, then 2^(bandwidth parameter) samples
   * are averaged, resulting in 4 averaged samples.
   * Note1 : For more information, refer the datasheet.
   * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
   * this has an adverse effect on the power consumed.
   */
  config[0].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

  /* Enable the filter performance mode where averaging of samples
   * will be done based on above set bandwidth and ODR.
   * There are two modes
   *  0 -> Ultra low power mode
   *  1 -> High performance mode(Default)
   * For more info refer datasheet.
   */
  config[0].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

  /* Configure the type of feature. */
  config[1].type = BMI2_GYRO;

  /* The user can change the following configuration parameters according to their requirement. */
  /* Set Output Data Rate */
  config[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;

  /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
  config[1].cfg.gyr.range = BMI2_GYR_RANGE_2000;

  /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
  config[1].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

  /* Enable/Disable the noise performance mode for precision yaw rate sensing
   * There are two modes
   *  0 -> Ultra low power mode(Default)
   *  1 -> High performance mode
   */
  config[1].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

  /* Enable/Disable the filter performance mode where averaging of samples
   * will be done based on above set bandwidth and ODR.
   * There are two modes
   *  0 -> Ultra low power mode
   *  1 -> High performance mode(Default)
   */
  config[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

  /* Set the accel configurations. */
  rslt = BMI270.set_sensor_config(config, 2);
  if (rslt != BMI2_OK) return rslt;

  rslt = BMI270.sensor_enable(sens_list, 2);
  if (rslt != BMI2_OK) return rslt;

  return rslt;
}

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
  //IMU
  int8_t rslt = BMI270.begin(BMI270_I2C,BMI2_I2C_SEC_ADDR);
  IMU_print_rslt(rslt);

  rslt = IMU_configure_sensor();
  IMU_print_rslt(rslt);

}

void loop() {
  // put your main code here, to run repeatedly:
  struct bmi2_sens_float sensor_data;
  int8_t rslt = BMI270.bmi2_get_sensor_float(&sensor_data);
  IMU_print_rslt(rslt);

  float IMU_x = sensor_data.acc.x;
  float IMU_y = sensor_data.acc.y;
  float IMU_z = sensor_data.acc.z;
  float IMU_gx = sensor_data.gyr.x;
  float IMU_gy = sensor_data.gyr.y;
  float IMU_gz = sensor_data.gyr.z;

  Serial.print("IMU_x:");
  Serial.print(IMU_x);
  Serial.print(",IMU_y:");
  Serial.print(IMU_y);
  Serial.print(",IMU_z:");
  Serial.print(IMU_z);
  Serial.print(",IMU_gx:");
  Serial.print(IMU_gx);
  Serial.print(",IMU_gy:");
  Serial.print(IMU_gy);
  Serial.print(",IMU_gz:");
  Serial.println(IMU_gz);

}

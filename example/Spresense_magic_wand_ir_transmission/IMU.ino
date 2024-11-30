//--------------------------------------
//  9軸センサ値処理
//--------------------------------------

const float AccValue = 0.04; //静止加速度閾値
const int SpentTime = 150;

const int numReadings = 10;  // センサ値を読み取る回数
int IMU_vec[numReadings];  // センサ値を保存する配列
int IMU_readIndex = 0;  // 現在の読み取りインデックス
int thLower = 0;
int thUpper = 0;

float IMU_x, IMU_y, IMU_z;
float IMU_mx,IMU_my,IMU_mz;
float IMU_gx,IMU_gy,IMU_gz;
float IMU_gxAve,IMU_gyAve,IMU_gzAve;
#define NUM 3 //移動平均回数
float IMU_gyroX[NUM];
float IMU_gyroY[NUM];
float IMU_gyroZ[NUM];
int IMU_index = 0;               //  配列のインデックス
float IMU_totalX = 0;               // 読み取り値の合計
float IMU_averageX = 0;             // 移動平均値
float IMU_totalY = 0;               // 読み取り値の合計
float IMU_averageY = 0;             // 移動平均値
float IMU_totalZ = 0;               // 読み取り値の合計
float IMU_averageZ = 0;             // 移動平均値
float IMU_offsetX = 0;
float IMU_offsetY = 0;
float IMU_offsetZ = 0;

//加速度センサ補正値
float acc_gainX = 1.01010101010101;
float acc_gainY = 1.01010101010101;
float acc_gainZ = 1;
float acc_offsetX = 0;
float acc_offsetY = -0.01;
float acc_offsetZ = 0;


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

void IMU_Init(){
  int8_t rslt = BMI270.begin(BMI270_I2C,BMI2_I2C_SEC_ADDR);
  IMU_print_rslt(rslt);

  rslt = IMU_configure_sensor();
  IMU_print_rslt(rslt);
  /*
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Magnetic field sample rate = ");
  Serial.print(IMU.magneticFieldSampleRate());
  Serial.println(" uT");
  Serial.println();
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");
  Serial.println("X\tY\tZ");
  */
  //配列初期化
  for(int i=0;i<NUM;i++){
    IMU_gyroX[i]=0;
    IMU_gyroY[i]=0;
    IMU_gyroZ[i]=0;
  }
    // 配列を初期化
  for (int i = 0; i < numReadings; i++) {
    IMU_vec[i] = 0;
  }
  Serial.print("Gyro init.Put Device");
  for(int i=0;i<NUM;i++){
    IMU_main();
    Serial.print(".");
    delay(10);
  }
  Serial.println();

  Serial.print("AccTH init.Put Device");
  //配列初期化
  for (int i = 0; i < numReadings; i++) {
    IMU_main();
    Serial.print(".");
    delay(10);
  }
  thLower = IMU_checkSensorValuesAve() - 1;
  thUpper = IMU_checkSensorValuesAve() + 1;
  Serial.println();
  
  Serial.print("LowerTH:");
  Serial.println(thLower);
  Serial.print("UpperTH:");
  Serial.println(thUpper);

  IMU_Reset();
  Serial.println("IMU_init_Finish");
  
}
//ジャイロセンサの平均値を求める
void IMU_CalcAverage(float valueX,float valueY,float valueZ){
  
  // 過去の読み取り値の合計から古い値を引く
  IMU_totalX = IMU_totalX - IMU_gyroX[IMU_index];
  IMU_totalY = IMU_totalY - IMU_gyroY[IMU_index];
  IMU_totalZ = IMU_totalZ - IMU_gyroZ[IMU_index];

  // 最新のセンサの値を  配列に追加
  IMU_gyroX[IMU_index] = valueX;
  IMU_gyroY[IMU_index] = valueY;
  IMU_gyroZ[IMU_index] = valueZ;

  // 過去の読み取り値の合計に新しい値を加える
  IMU_totalX = IMU_totalX + IMU_gyroX[IMU_index];
  IMU_totalY = IMU_totalY + IMU_gyroY[IMU_index];
  IMU_totalZ = IMU_totalZ + IMU_gyroZ[IMU_index];

  // 次の  配列のインデックスを計算
  IMU_index = (IMU_index + 1) % NUM;

  // 移動平均を計算
  IMU_averageX = IMU_totalX / NUM;
  IMU_averageY = IMU_totalY / NUM;
  IMU_averageZ = IMU_totalZ / NUM;
}

float IMU_CalcAccVec(float x,float y,float z){
    return abs(sqrt(x*x+y*y+z*z) - 1);
}

float CorrectAccX(float a){
  return acc_gainX*(a+acc_offsetX);
}
float CorrectAccY(float a){
  return acc_gainY*(a+acc_offsetY);
}
float CorrectAccZ(float a){
  return acc_gainZ*(a+acc_offsetZ);
}

float IMU_ReadAccX(){
  return CorrectAccX(IMU_x);
}
float IMU_ReadAccY(){
  return CorrectAccY(IMU_y);
}
float IMU_ReadAccZ(){
  return CorrectAccZ(IMU_z);
}

float IMU_ReadMagX(){
  return IMU_mx;
}
float IMU_ReadMagY(){
  return IMU_my;
}
float IMU_ReadMagZ(){
  return IMU_mz;
}
float IMU_ReadGyroX(){
  return IMU_gx;
}
float IMU_ReadGyroY(){
  return IMU_gy;
}
float IMU_ReadGyroZ(){
  return IMU_gz;
}
float IMU_ReadAveGyroX(){
  return IMU_averageX - IMU_offsetX;
}
float IMU_ReadAveGyroY(){
  return IMU_averageY - IMU_offsetY;
}
float IMU_ReadAveGyroZ(){
  return IMU_averageZ - IMU_offsetZ;
}
void IMU_Reset(){
  IMU_offsetX = IMU_averageX;
  IMU_offsetY = IMU_averageY;
  IMU_offsetZ = IMU_averageZ;
}

// 10回分のセンサ値が8から10の間に収まっているかチェックする関数
bool IMU_checkSensorValues() {
  for (int i = 0; i < numReadings; i++) {
    if (IMU_vec[i] < thLower || IMU_vec[i] > thUpper) {
      return false;  // 収まっていない値があればfalseを返す
    }
  }
  return true;  // 全ての値が8から10の間に収まっていればtrueを返す
}

int IMU_checkSensorValuesAve() {
  int values=0;
  for (int i = 0; i < numReadings; i++) {
    values += IMU_vec[i];
  }
  return values/numReadings;
}
//IMU Reset Check
float IMU_startTime = 0;
bool countflag = false;
bool gflag =false;

bool IMU_CheckAccActive(){
  return gflag;
}
void IMU_main(){
/*
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(IMU_x, IMU_y, IMU_z);
  }
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(IMU_mx, IMU_my, IMU_mz);
    Serial.println("MagneticField_OK");
  }
    if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(IMU_gx, IMU_gy, IMU_gz);
  }
*/
  struct bmi2_sens_float sensor_data;
  int8_t rslt = BMI270.bmi2_get_sensor_float(&sensor_data);
  IMU_print_rslt(rslt);

  IMU_x = sensor_data.acc.x;
  IMU_y = sensor_data.acc.y;
  IMU_z = sensor_data.acc.z;
  IMU_gx = sensor_data.gyr.x;
  IMU_gy = sensor_data.gyr.y;
  IMU_gz = sensor_data.gyr.z;
  
  //ジャイロセンサ値移動平均処理
  IMU_CalcAverage(IMU_gx, IMU_gy, IMU_gz);

  // 配列に値を読み込む
  IMU_vec[IMU_readIndex] = IMU_CalcAccVec(IMU_ReadAccX(),IMU_ReadAccY(),IMU_ReadAccZ());
  //インデックス更新
  IMU_readIndex = (IMU_readIndex + 1) % numReadings;

  //静止判定
  if(IMU_checkSensorValues() < AccValue){
    if(!countflag){
      IMU_startTime = millis(); // カウンター開始前であればStartTimeを記録
      countflag = true;
    }
    if(millis() - IMU_startTime > SpentTime){
      gflag=true;
    }
    else{
      gflag=false;
    }
  }else{
    countflag = false; //閾値の以上なら
    gflag = false;     // IMUの値が閾値以上ならgflagをfalseに設定
  }

}
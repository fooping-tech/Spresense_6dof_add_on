//--------------------------------------
//  6軸センサによる姿勢推定
//--------------------------------------

//センサからの読み出し値
float accx=0; //加速度x
float accy=0; //加速度y
float accz=0; //加速度z
float gyrox=0; //ジャイロセンサx
float gyroy=0; //ジャイロセンサy
float gyroz=0; //ジャイロセンサz

//ジャイロセンサ補正値(Initで算出)
float gyrox0=0;
float gyroy0=0;
float gyroz0=0;

//姿勢値オフセット変数
float head0=0;
float roll0=0;
float pitch0=0;

//ジャイロセンサドリフト補正  
void GyroInit(){
  Serial.print("Gyro Init");
  for(int i=0 ;i<100;i++){
    //加算
    gyrox0 +=gyrox;
    gyroy0 +=gyroy;
    gyroz0 +=gyroz;
    Serial.print(".");
  }
  
    gyrox0 = gyrox0/100;
    gyroy0 = gyroy0/100;
    gyroz0 = gyroz0/100;
    //初期値を表示
    Serial.println("Finished");
    Serial.print("GyroX:"+ String(gyrox0));
    Serial.print(",GyroY:"+ String(gyroy0));
    Serial.println(",GyroZ:"+ String(gyroz0));
}

//タイマー割り込み関数
unsigned int TimerInterruptFunction() {
  gyrox = IMU_ReadAveGyroX();
  gyroy = IMU_ReadAveGyroY();
  gyroz = IMU_ReadAveGyroZ();
  accx = IMU_ReadAccX();
  accy = IMU_ReadAccY();
  accz = IMU_ReadAccZ();
  //magx = 0;//mx;//屋内では地磁気センサ誤動作するため0
  //magy = 0;//my;//屋内では地磁気センサ誤動作するため0
  //magz = 0;//mz;//屋内では地磁気センサ誤動作するため0
  filter->update(gyrox,gyroy,gyroz,accx,accy,accz,0,0,0);
  // Serial.print("heading:");
  // Serial.print(filter->getYaw());
  // Serial.print(",roll:");
  // Serial.println(filter->getRoll());
  roll = filter->getRoll()-roll0;
  pitch = filter->getPitch()-pitch0;
  heading = filter->getYaw()-head0;
  return INTERVAL;
}

//--------------------------------------
//  CANVAS
//--------------------------------------

//Canvasリセット
void ResetCanvas(){
    MadgWick_Init();     //フィルタおよび変数を初期化
    canvas->Reset();    //Canvasを初期化
    delete canvas;
    canvas = new CANVAS(240,240,0,0);    //杖軌跡
}

//MadgWickフィルタ初期化
void MadgWick_Init(){
  filter = new Madgwick();
  filter->begin(1000000/INTERVAL);
  roll0=filter->getRoll();
  pitch0=filter->getPitch();
  head0=filter->getYaw();
  gyrox0=0;
  gyroy0=0;
  gyroz0=0;
  gyrox=0;
  gyroy=0;
  gyroz=0;
  accx=0;
  accy=0;
  accz=0;
}


//--------------------------------------
//  Switch
//--------------------------------------
/*
void SW_main(){
  //SW状態を取得
  bool swflag = switch1->check_change();
  //スイッチの値に変化があった場合
  if(swflag){
    ResetCanvas();
  }
}

bool SW_Check(){
  return switch1->check_change();
}
*/
//--------------------------------------
//  Record Functions
//--------------------------------------
int number=0;
int label=0;
//Arudinoシリアル通信の受信値を処理
void Serial_main(){
  if (Serial.available() > 0) { // シリアルバッファにデータがあるか確認
    char receivedChar = Serial.read(); // 1バイト読み取り

    if (receivedChar == 's') { // もし受信したデータが's'なら
        SaveCSV();
        Serial.println("Done!");  
    }
    if (receivedChar == 'l') { // もし受信したデータが'l'ならラベル+1
        label++;
        if(label>5)label=0;
        Serial.print("label=");
        Serial.println(label);  
    }
    if (receivedChar == 'p') { // もし受信したデータが'p'なら
        canvas->PrintSerial28();
    }
    if (receivedChar == 'r') { // もし受信したデータが'r'なら
        ResetCanvas();
    }


  }
}
//CSVにセーブする
void SaveCSV(){
  char filename[16];
  sprintf(filename, "%01d%03d.csv", label, number);

  //Flashに保存する場合
//  Flash.remove(filename);
//  File myFile = Flash.open(filename, FILE_WRITE);

  //SDに保存する場合
  SD.remove(filename);
  File myFile = SD.open(filename, FILE_WRITE);

  for (int i=0;i<28;i++) {
    for (int j=0;j<28;j++) {
      myFile.print(canvas->output[i + 28 * j]);
      //Serial.print(canvas->output[i + 28 * j]);
      if(j!=27)myFile.print(",");
      //Serial.print(",");
    }
    myFile.println("");
    //Serial.println("");
  }
  Serial.printf("%01d%03d.csv", label, number);
  myFile.close();
  number++;
}
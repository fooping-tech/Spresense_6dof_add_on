/*
    The MIT License (MIT)

    Copyright (c) 2024 FoopingTech.
    X(Twitter)    : @FoopingTech
    Create Time: 2024-01


    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/
//サブコア誤書き込み防止
#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#define RECORD_MODE 0//0:推論モード,1:学習モード
//SD
#include <SDHCI.h>
SDClass  SD;
#include <File.h>
//Flash
#include <Flash.h>

//IMU
#include "BMI270_Arduino.h"
BMI270Class BMI270;

//Canvas
#include "CANVAS.h"
CANVAS *canvas1;
CANVAS *canvas2;
CANVAS *canvas3;
CANVAS *canvas4;
CANVAS *canvas5;
CANVAS *canvas6;
//SW
#include "SW.h"
SW *switch1;

//MadgWick
#include <MadgwickAHRS.h>
#define INTERVAL 100000 //us
float roll=0;
float pitch=0;
float heading=0;
Madgwick *filter;

//DNN
#include <DNNRT.h>
DNNRT dnnrt;
#define DNN_DATA_WIDTH 28
#define DNN_DATA_HEIGHT 28
DNNVariable input(DNN_DATA_WIDTH*DNN_DATA_HEIGHT);
static String const labels[4]= {"EIGHT", "CIRCLE", "MINUS", "NON"};
int command =3;

//mode
// モードの列挙型
enum MODE {
  MODE1,
  MODE2,
  MODE3,
  MODE4
};
MODE currentMode = MODE4;
MODE beforeMode = MODE4;

int TaskSpan;       //タスク実行間隔
uint32_t startTime; //開始時間
uint32_t cycleTime; //サイクルタイム
uint32_t deltaTime; //
uint32_t spentTime; //経過時間
bool _InitCondition=false;  //初期化状態
bool _DeinitCondition=false;  //終了時状態


File myFile;
bool audio=false;

int CheckCommand(){
  float *dnnbuf = input.data();
  int count=0;
  for (int i=0;i<DNN_DATA_WIDTH;i++) {
    for (int j=0;j<DNN_DATA_HEIGHT;j++) {
      dnnbuf[count] = canvas4->output[i + 28 * j];
      count++;
    }
  }

  dnnrt.inputVariable(input,0);
  dnnrt.forward();

  DNNVariable output = dnnrt.outputVariable(0);
  int index = output.maxIndex();
  return index;
}

void setup() {
  Serial.begin(115200);
 
  //IMU
  IMU_Init();

  switch1 = new SW(PIN_D21,INPUT_PULLUP);
  //タイマ割り込み
  attachTimerInterrupt(TimerInterruptFunction,INTERVAL);

  canvas4 = new CANVAS(240,240,0,0);    //杖軌跡

  //DNN
  //File nnbfile = Flash.open("model.nnb");
  File nnbfile = SD.open("model.nnb");
  int ret = dnnrt.begin(nnbfile);
  if (ret < 0) {
    Serial.println("dnnrt.begin failed" + String(ret));
  }
  
  //ジャイロセンサ
  GyroInit();

  //MadgWick
  MadgWick_Init();
  
  
  Serial.println("Setup_Finished!!");
}


void CANVAS_main(){
  //杖軌跡描画
  canvas4->WandDraw28(heading,roll);
}

void mainloop(MODE m){
  //前回実行時からモードが変わってたら終了処理
  if(beforeMode != m)DeinitActive();
  beforeMode = m;
   //前回実行時からの経過時間を計算
  deltaTime = millis() - cycleTime;
  //経過時間を計算
  spentTime = millis() - startTime;

  if(deltaTime >= TaskSpan){
    //Serial.println("mainloop");
    if(!_InitCondition){//未初期化時実行
      //Serial.println("Init");
      InitFunction(m);
    }else if(_DeinitCondition){//修了処理時実行
      //Serial.println("Deinit");
      DeinitFunction();
    }else{
      //Serial.println("main()");
        //モードごとの処理
    switch (m) {
      case MODE1:
        //TOF_SetLED(255,255,255);      
        break;

      case MODE2:
        //TOF_SetLED(255,0,0);
        break;

      case MODE3:
        //TOF_SetLED(0,255,0);
        //currentMode = MODE4;
        break;

      case MODE4:
        //TOF_SetLED(0,0,0);
        break;
      }
      
    }
    //cycleTimeリセット
    cycleTime = millis();
  }
}

void DeinitActive(){//モード終了時に呼ぶ
  startTime = millis();
  //モードごとの処理
  _DeinitCondition=true;
}
void DeinitFunction(){//最後に呼ばれる
  
  _InitCondition=false;  //初期化状態
  _DeinitCondition=false;  //終了時状態
}
void InitFunction(MODE m){//初回呼ばれる
  //モードごとの処理
  switch (m) {
  case MODE1:
    break;

  case MODE2:
    break;

  case MODE3:
    break;

  case MODE4:
    break;
}
  startTime = millis();
  _InitCondition=true;  //初回フラグon
}


void loop() {
  IMU_main();         //IMUセンサ値更新
  CANVAS_main();      //描画更新
  if(RECORD_MODE == 0)command = CheckCommand(); //DNN
  //SW_main();         //ボタンチェック(押下時Reset処理)
  Serial_main();      //Arduinoシリアル操作

  //モード起動時処理
  if(RECORD_MODE == 0){
    if(command==0){
      currentMode = MODE1;
      ledOn(LED0);
      ledOff(LED1);
      ledOff(LED2);
      ledOff(LED3);
    }
    if(command==1){
      currentMode = MODE2;
      ledOn(LED1);
      ledOff(LED0);
      ledOff(LED2);
      ledOff(LED3);
    }
    if(command==2){
      currentMode = MODE3;
      ledOn(LED2);
      ledOff(LED0);
      ledOff(LED1);
      ledOff(LED3);
    }
    if(command==3){
      currentMode = MODE4;
      ledOn(LED3);
      ledOff(LED0);
      ledOff(LED1);
      ledOff(LED2);
    }
    
    if(SW_Check()){
      if(command==0){
        currentMode = MODE1;
        ResetCanvas();
      }
      if(command==1){
        currentMode = MODE2;
        ResetCanvas();
      }
      if(command==2){
        currentMode = MODE3;
        ResetCanvas();
      }
      if(command==3){
        currentMode = MODE4;
        ResetCanvas();
      }
    }
  }
  mainloop(currentMode);

  /*
  //所定の加速度より早い場合キャンバスを消す
  if(IMU_CalcAccVec(IMU_ReadAccX(),IMU_ReadAccY(),IMU_ReadAccZ())>9.8*1.5){
    ResetCanvas();
  }
  */


}


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
#ifndef CANVAS_H
#define CANVAS_H
#include <Arduino.h>
#include <vector>
#define LINE_COUNT 3
class CANVAS {

public:
  //コンストラクタ
  CANVAS(int w,int h,int x,int y){
    
    height = h;
    width = w;
    offsetX = x;
    offsetY = y;
    // グラフの中央の y オフセットを設定(1bitシフトすることで中央pixelを取得)
    yoffset = height/2+ offsetY; //tft.height() >> 1;
    // グラフの中央の x オフセットを設定
    xoffset = width/2 + offsetX;// tft.width()  >> 1;
    // 各ラインのポイントの数を設定
    point_count = width + 1;

    x0 = xoffset;
    y0 = yoffset;
    x1 = xoffset;
    y1 = yoffset;

    // 各ラインのポイント配列を初期化
    for (int i = 0; i < LINE_COUNT; i++) {
      points[i].resize(point_count);
    }

    // TFT ディスプレイの描画領域を設定
    //tft->setAddrWindow(offsetX, offsetY, width, height);

    // 画面上の各ピクセルに基づいて色を計算し、描画
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        // getBaseColor 関数によって、座標に基づいた基本色を取得
        //tft->writeColor(getBaseColor(x + offsetX, y - yoffset + offsetY), 1);
      }
      //Serial.println("CANVAS setup was completed.");
    }
    if(width == height){
      output = malloc(sizeof(int) * width * height);
       for (int i=0;i<width;i++) {
         for (int j=0;j<height;j++) {
           output[i * height + j] = 0;
         }
       }
      Serial.println("canvas Setting Done");
    }
  }
  ~CANVAS(){
    free(output);
  }
  //int getBaseColor(int,int);
  void setup(){
    Serial.println("Canvas Setup ok");
  }
  //void GraphDraw(float*);
  //void GraphDraw2(float*);
  //void DotDraw(int*);
  //void StringDraw(String,int);
  //void StringDrawL(String,int);
  float scaleToRange(float,float,float);
  void WandDraw(float,float);
  void WandDraw28(float,float);
  void DrawPointsOnLine(int,int,int,int,int);
  void Reset(){
    // TFT ディスプレイの描画領域を設定
    //tft->setAddrWindow(offsetX, offsetY, width, height);
    //tft->fillRect(offsetX,offsetY,width,height,TFT_BLACK);
    // 画面上の各ピクセルに基づいて色を計算し、描画
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        // getBaseColor 関数によって、座標 (x, y - yoffset) に基づいた基本色を取得
        //tft->writeColor(getBaseColor(x + offsetX, y - yoffset + offsetY), 1);
      }
    }
  }
  void PrintSerial();
  void PrintSerial28();
  int counter=0;
  int *output;//出力用
  

  private:
    std::vector<int> points[LINE_COUNT];
    int height = 0;
    int width = 0;
    int offsetX = 0;
    int offsetY = 0;
    int yoffset = 0;
    int xoffset = 0;
    int point_count = 0;
    //Cane
    float a =1.4;
    int bx =0;
    int by =0;
    int x0=0;
    int y0=0;
    int x1=0;
    int y1=0;
    //LGFX_SPRESENSE_SPI_ILI9341 *tft;
    
};

#endif
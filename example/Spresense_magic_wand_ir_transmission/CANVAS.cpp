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
#include "CANVAS.h"



// スケーリング関数
float CANVAS::scaleToRange(float value, float minRange, float maxRange) {
  // 値を指定された範囲に収める
  return constrain(value, minRange, maxRange);
}
//杖の軌跡を描画する関数
void CANVAS::WandDraw(float x,float y){
  //値を描画
  x1 = -1 * (x + bx)* a  + xoffset;
  y1 = -1 * (y + by)* a + yoffset;
  //前回値x0,y0を更新
  x0 = x1;
  y0 = y1;
  //outputに保存
  if(0 <= x1 && x1< width && 0 <= y1 && y1 < height){
    output[x1 * height + y1] = 1;
  }
}
void CANVAS::DrawPointsOnLine(int x1, int y1, int x2, int y2, int numPoints) {
  // tは直線上の位置を示すパラメータ（0 <= t <= 1）
  float tStep = 1.0 / (numPoints - 1);  // tのステップサイズ
  float t = 0.0;

  for (int i = 0; i < numPoints; ++i) {
    int x = x1 + t * (x2 - x1);
    int y = y1 + t * (y2 - y1);

    // Serial.print("座標(");
    // Serial.print(x);
    // Serial.print(", ");
    // Serial.print(y);
    // Serial.println(")");

    //tft->fillRect(x * width /28 ,y * height /28 ,width/28,height/28,TFT_YELLOW);
    //outputに保存
    if(0 <= x && x< 28 && 0 <= y && y < 28){
      output[x * 28 + y] = 1;
    }
    t += tStep;  // tを更新
  }
}
//杖の軌跡を描画する関数
void CANVAS::WandDraw28(float x,float y){
  //描画範囲を指定
  //tft->setAddrWindow(offsetX, offsetY, width, height);
  //tft->drawRect(offsetX,offsetY,width,height,TFT_DARKGRAY);

  //値を描画
  x1 = -1 * (x + bx)* a  + xoffset;
  y1 = -1 * (y + by)* a  + yoffset;
  int _x1 = x1 * 28 / width;
  int _y1 = y1 * 28 / height;
  int _x0 = x0 * 28 / width;
  int _y0 = y0 * 28 / height;

  //2点間のポイントを塗りつぶす
  DrawPointsOnLine(_x0,_y0,_x1,_y1,5);

  //前回値x0,y0を更新
  x0 = x1;
  y0 = y1;

}
void CANVAS::PrintSerial(){
  for (int i=0;i<width;i++) {
    for (int j=0;j<height;j++) {
      Serial.print(output[i + height * j]);
    }
    Serial.println("");
  }
}
void CANVAS::PrintSerial28(){
  Serial.println("----------------------------");
  for (int i=0;i<28;i++) {
    for (int j=0;j<28;j++) {
      Serial.print(output[i + 28 * j]);
    }
    Serial.println("");
  }
}

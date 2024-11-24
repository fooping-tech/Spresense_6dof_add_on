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
#ifndef SW_H
#define SW_H


class SW {


public:
  const int SW_PIN;  // ピン番号は定数としてクラスに含める
  //コンストラクタ：ピン番号、入力モード(INPUT,INPUT_PULLUP)
  SW(int pin, int mode)
    : SW_PIN(pin) {
    pinMode(SW_PIN, mode);
    // 長押しとみなす時間（ミリ秒）
    longPressDuration = 700;
    // ダブルクリックとみなす時間間隔（ミリ秒）
    doubleClickDelay = 300;
    Serial.println("SW setup was completed.");
  }
  //pin状態確認
int check_a() {
    int reading = digitalRead(SW_PIN);
    unsigned long currentTime = millis();

    if (currentTime - lastDebounceTime > debounceDelay) {
      if (reading != lastButtonState) {
        lastDebounceTime = currentTime;
        lastButtonState = reading;
        return reading;
      }
    }

    return lastButtonState;
  }

/**
 * @brief スイッチの状態変化を検出し、変化の種類を返す関数
 * 
 * @return int 
 *         -1: スイッチがLOWに変化（押された）
 *          1: スイッチがHIGHに変化（離された）
 *          0: 変化なし
 */
int check_change() {
  // 現在のスイッチの状態を読み取る
  cur_value = digitalRead(SW_PIN);
  
  // 現在の時間を取得
  unsigned long currentTime = millis();
  
  // デバウンス処理：前回の状態変化から一定時間（debounceDelay）経過しているか確認
  if (currentTime - lastDebounceTime > debounceDelay) {
    // 前回の値と現在の値が異なる場合（状態変化あり）
    if (cur_value != last_value) {
      // スイッチの状態変化フラグをtrueに設定
      sw_val = true;
      
      // 最後のデバウンス時間を更新
      lastDebounceTime = currentTime;
      
      // 前回の値を現在の値で更新
      last_value = cur_value;
      
      // スイッチの状態に応じて戻り値を決定
      // LOWになった場合（押された）は-1、HIGHになった場合（離された）は1を返す
      return (cur_value == LOW) ? -1 : 1;
    } else {
      // 状態変化なしの場合、フラグをfalseに設定
      sw_val = false;
    }
  }
  
  // 変化がない場合または十分な時間が経過していない場合は0を返す
  return 0;
}

    // ダブル:3 ,長押し:2,短押し:1,押していない:0
    int check_m() {
      cur_value = digitalRead(SW_PIN);
      unsigned long currentTime = millis();  // 現在時刻を取得（追加）

      // デバウンス処理（追加）
      if (currentTime - lastDebounceTime > debounceDelay) {
        if (cur_value != last_value) {
          last_value = cur_value;
          lastDebounceTime = currentTime;  // 最後のデバウンス時間を更新（追加）
          if (cur_value == LOW) {
            if ((millis() - lastDebounceTime) > longPressDuration) {
              buttonLongPressed = true;
              Serial.println("LONG");
              return 2;
            } else {
              int currentTime = millis();
              if (currentTime - lastClickTime < doubleClickDelay) {
                lastClickTime = 0;
                Serial.println("DOUBLE");
                return 3;
              } else {
                lastClickTime = currentTime;
                Serial.println("SINGLE");
                return 1;
              }
            }
          } else {
            //lastDebounceTime = millis();
            lastPressTime = currentTime;  // ボタンが離されたときの時間を記録（追加
            buttonLongPressed = false;
          }
        }
      }

      //last_value = cur_value;

      return 0;
    }
  private:
    int cur_value = 0;
    int last_value = 0;

    unsigned long lastDebounceTime = 0;     // 最後にボタンが押された時間
    unsigned long debounceDelay = 10;       // デバウンス用の遅延時間（ミリ秒）
    bool buttonLongPressed = false;         // ボタンが長押し状態かどうかを示すフラグ
    unsigned long longPressDuration = 700;  // 長押しとみなす時間（ミリ秒）

    int lastClickTime = 0;       // 最後のクリック時間
    int doubleClickDelay = 300;  // ダブルクリックとみなす時間間隔（ミリ秒）
    bool sw_val = false;
    unsigned long lastPressTime = 0; // 最後にボタンが押された時間（追加）
    int lastButtonState = HIGH;
};

#endif
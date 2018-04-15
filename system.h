#pragma once

bool checkClick(int button)
{
  // Переменная для хранения предыдущего состояния кнопки
  static bool buttonWasUp = true;
  bool result = false;

  // Читаем текущее состояние кнопки
  bool buttonIsUp = digitalRead(button);

  // Если кнопка была не нажата, а теперь нажата
  // значит был клик
  if (buttonWasUp && !buttonIsUp) {
      delay(100);
      result = true;
    }
  buttonWasUp = buttonIsUp;
  return result;
}

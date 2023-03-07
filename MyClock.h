#include "DS3231.h"

class Clock {
  private:
  DS3231 *clock;
  int arr[3];

  public:
  Clock(uint8_t data_pin, uint8_t clock_pin) {
    clock = new DS3231(data_pin, clock_pin);
    clock->begin();
  }

  void update(String input) {
    int firstIndex = input.indexOf(",");
    int secondIndex = input.lastIndexOf(",");
    clock->setDOW(input.substring(0, firstIndex).toInt());
    splitData(input.substring(firstIndex + 1), "-");
    clock->setTime(arr[0], arr[1], arr[2]);
    splitData(input.substring(secondIndex + 1), "/");
    clock->setDate(arr[0], arr[1], arr[2]);
  }

  String display() {
    return String(clock->getDOWStr()) + " " + String(clock->getDateStr()) + " " + String(clock->getTimeStr());
  }

  private:
  void splitData(String in, String splitBy) {
    int firstIndex = in.indexOf(splitBy);
    arr[0] = in.substring(0, firstIndex).toInt();
    int secondIndex = in.lastIndexOf(splitBy);
    arr[1] = in.substring(firstIndex + 1, secondIndex).toInt();
    arr[2] = in.substring(secondIndex + 1).toInt();
  }
};
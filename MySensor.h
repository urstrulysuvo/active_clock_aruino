class Sensor
{
  private:
  uint8_t pinNo;
  int prevInput = 1;

  public:
  Sensor(uint8_t pin) 
  {
    pinNo = pin;
    pinMode(pin, INPUT);
  }

  bool isActive() {
    int input = digitalRead(pinNo);
    if(prevInput != input) {      
      prevInput = input;
      return (input == 0);
    }
    return false;
  }
};
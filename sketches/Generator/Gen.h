

void setupGenerators();
void updateGenerators();

class Gen {
  public:
    int id;
    bool on = false;
    bool buttonPressed = false;
    bool enabled();
    uint8_t brightness();
    unsigned long stateLastChanged;
    unsigned long lastChange = 0;
    int voltageWhenLastChanged = 0;
   
    bool stable = true;

    int operatingVoltage();
    int bonusVoltage() {
       return 175;
    }
    void init(int id);
    void update();
    int currentVoltageTarget();
    void changeState();
    int currentVoltageContribution();
};

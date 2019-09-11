


int bonusVoltage = 175;
float halfLife = 1;




class Gen {
  public:
    int id;
    bool on = false;
    bool buttonPressed = false;
    bool enabled();
    uint8_t brightness();
    unsigned long stateLastChanged;
    unsigned long voltageWhenLastChanged;
    int operatingVoltage;
    bool stable;

    void update();
    int currentVoltageTarget();
    void changeState();
    int currentVoltageContribution();
    void init(int id);
};

extern Gen [] gen;

void setupGenerators();
void updateGenerators();
extern int currentVoltage;

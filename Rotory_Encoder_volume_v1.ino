#include <BleKeyboard.h>

BleKeyboard bleKeyboard("ESP32 Volume Knobs");

// Define encoder pins
const int encoders[][2] = {
    {21, 22}, // Encoder 1
    {25, 26}, // Encoder 2
    {32, 33}  // Encoder 3
};

// Store last states
int lastState[] = {0, 0, 0};
int volumes[] = {50, 50, 50};  // Default volumes (0-100)

void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();

    for (int i = 0; i < 3; i++) {
        pinMode(encoders[i][0], INPUT_PULLUP);
        pinMode(encoders[i][1], INPUT_PULLUP);
        lastState[i] = (digitalRead(encoders[i][0]) | (digitalRead(encoders[i][1]) << 1));
    }
}

void loop() {
    for (int i = 0; i < 3; i++) {
        int AState = digitalRead(encoders[i][0]);
        int BState = digitalRead(encoders[i][1]) << 1;
        int currentState = AState | BState;

        if (currentState != lastState[i]) {
            if ((lastState[i] == 0b10 && currentState == 0b00) || (lastState[i] == 0b11 && currentState == 0b10)) {
                volumes[i] = min(volumes[i] + 5, 100);
                sendVolumeCommand(i, true);  // Increase volume
            } else if ((lastState[i] == 0b01 && currentState == 0b00) || (lastState[i] == 0b11 && currentState == 0b01)) {
                volumes[i] = max(volumes[i] - 5, 0);
                sendVolumeCommand(i, false);  // Decrease volume
            }
            lastState[i] = currentState;
        }
    }
    delay(5);
}

// Function to send Bluetooth volume commands for different encoders
void sendVolumeCommand(int encoderIndex, bool increase) {
    if (!bleKeyboard.isConnected()) return;

    switch (encoderIndex) {
        case 0:
            bleKeyboard.write(increase ? KEY_MEDIA_VOLUME_UP : KEY_MEDIA_VOLUME_DOWN);
            break;
        case 1:
            bleKeyboard.write(increase ? KEY_MEDIA_NEXT_TRACK : KEY_MEDIA_PREV_TRACK);
            break;
        case 2:
            bleKeyboard.write(increase ? KEY_MEDIA_PLAY_PAUSE : KEY_MEDIA_STOP);
            break;
    }

    Serial.print("Encoder ");
    Serial.print(encoderIndex);
    Serial.print(" Volume: ");
    Serial.println(volumes[encoderIndex]);
}

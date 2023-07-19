
#include <SPI.h>
#include <RH_RF95.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define BUTTON_PIN 5
#define SIGNAL_PIN 10

#define POWER_SAVE_SLEEP 1

/************ Radio Setup ***************/

// M0 Feather Values
#define RADIO_FREQ 915.0
#define RADIO_CS 8
#define RADIO_INT 3
#define RADIO_RST 4
#define LED_PIN 13

// Singleton instance of the radio driver
RH_RF95 radio(RADIO_CS, RADIO_INT);

uint32_t last_tx = 0;
uint32_t wait_tx = 5;

uint8_t remote_state = LOW;
uint8_t curr_state = remote_state;

void setup()
{
    Serial.begin(115200);
    // while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

    pinMode(SIGNAL_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(RADIO_RST, OUTPUT);
    digitalWrite(RADIO_RST, LOW);

    Serial.println("Feather RADIO RX Test!");
    Serial.println();

    // manual reset
    digitalWrite(RADIO_RST, HIGH);
    delay(10);
    digitalWrite(RADIO_RST, LOW);
    delay(10);

    if (!radio.init())
    {
        Serial.println("RADIO radio init failed");
        while (1)
            ;
    }
    Serial.println("RADIO radio init OK!");

    if (!radio.setFrequency(RADIO_FREQ))
    {
        Serial.println("setFrequency failed");
    }

    radio.setTxPower(20, true); // range from 14-20 for power, 2nd arg must be true for 69HCW

    Serial.print("radio @");
    Serial.print((int)RADIO_FREQ);
    Serial.println(" MHz");

    digitalWrite(LED_PIN, HIGH);
}

void loop()
{
    checkButtonState();
    checkForAcknowledgements();
    syncStateWithRemote();

    delay(POWER_SAVE_SLEEP);
}

void checkButtonState()
{
    curr_state = digitalRead(BUTTON_PIN);
}

void checkForAcknowledgements()
{
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (radio.available())
    {
        // Listen and record what the remote believes the state to be
        radio.recv(buf, &len);
        remote_state = buf[0];
    }
}

void syncStateWithRemote()
{
    if (remote_state != curr_state)
    {
        // If the current state is different than what the remote radio believes, update
        if (last_tx + wait_tx < millis())
        {
            uint8_t data[] = {curr_state};
            radio.send(data, sizeof(data));
            radio.waitPacketSent();
            last_tx = millis();
        }
    }
}
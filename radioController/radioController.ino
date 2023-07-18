
#include <SPI.h>
#include <RH_RF95.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define SIGNAL_PIN 10

/************ Radio Setup ***************/

// M0 Feather Values
#define RADIO_FREQ 915.0
#define RADIO_CS 8
#define RADIO_INT 3
#define RADIO_RST 4
#define LED 13

// Singleton instance of the radio driver
RH_RF95 radio(RADIO_CS, RADIO_INT);

uint8_t prev_state = LOW;

void setup()
{
    Serial.begin(115200);
    // while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

    pinMode(SIGNAL_PIN, OUTPUT);
    pinMode(LED, OUTPUT);
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

    radio.setTxPower(20, false); // range from 14-20 for power, 2nd arg must be true for 69HCW

    Serial.print("radio @");
    Serial.print((int)RADIO_FREQ);
    Serial.println(" MHz");

    digitalWrite(LED, HIGH);
}

int ii = 0;

void loop()
{
    if (radio.available())
    {
        // Should be a message for us now
        uint8_t buf[RH_RADIO_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (radio.recv(buf, &len))
        {
            if (!len)
                return;

            uint8_t new_state = buf[0];
            if (new_state != prev_state)
            {
                Serial.print(millis());
                Serial.print("STATE CHANGED: ");
                Serial.println((new_state == HIGH) ? "PRESSED" : "RELEASED");

                digitalWrite(SIGNAL_PIN, new_state);
                prev_state = new_state;

                radio.send(buf, len);
                radio.waitPacketSent();
            }
        }
    }
}
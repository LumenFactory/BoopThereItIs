
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 radio(8, 3); // Adafruit Feather M0 with RFM95

int LED_PIN = 13;
int SIGNAL_PIN = 10;

uint8_t lastState = 0;

void setup()
{

    pinMode(LED_PIN, OUTPUT);
    pinMode(SIGNAL_PIN, OUTPUT);
    Serial.begin(9600);
    digitalWrite(LED_PIN, LOW);
    delay(2000);

    // while (!Serial)
    ; // Wait for serial port to be available
    Serial.println("STARTING");
    if (!radio.init())
    {
        Serial.println("init failed");
        while (1)
        {
            digitalWrite(LED_PIN, HIGH);
            delay(10);
            digitalWrite(LED_PIN, LOW);
            delay(200);
            digitalWrite(LED_PIN, HIGH);
            delay(150);
            digitalWrite(LED_PIN, LOW);
            delay(600);
        };
    }

    Serial.println("Good Radio RC");
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
    // you can set transmitter powers from 5 to 23 dBm:
    //  driver.setTxPower(23, false);
    // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
    // transmitter RFO pins and not the PA_BOOST pins
    // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true.
    // Failure to do that will result in extremely low transmit powers.
    //  driver.setTxPower(14, true);
    radio.setFrequency(915.0);
    radio.setTxPower(20, false);
    radio.setModemConfig(RH_RF95::Bw500Cr45Sf128);
}

void loop()
{
    if (radio.available())
    {
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (radio.recv(buf, &len))
        {
            digitalWrite(LED_PIN, HIGH);
            if (len > 0)
            {
                setState(buf[0]);
            }

            // Send a reply
            radio.send(buf, sizeof(buf));
            radio.waitPacketSent();

            digitalWrite(LED_PIN, LOW);
        }
        else
        {
            Serial.println("recv failed");
        }
    }
}

void setState(uint8_t state)
{
    digitalWrite(SIGNAL_PIN, state);

    if (lastState != state)
    {
        Serial.print("State Changed: ");
        Serial.println(state);

        lastState = state;
    }
}

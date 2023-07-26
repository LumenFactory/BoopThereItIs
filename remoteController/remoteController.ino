
#include <SPI.h>
#include <RH_RF95.h>

#define LED_PIN 13
#define BUTTON_PIN 10
#define POWER_SAVE_SLEEP 1

RH_RF95 radio(8, 3); // Adafruit Feather M0 with RFM95

uint8_t curr_state = LOW;
uint8_t remote_state = curr_state;

uint32_t start_flight = 0;

void setup()
{
    Serial.begin(9600);
    delay(3000);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

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
        }
    }

    Serial.println("Radio Good Remote");
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
    radio.setTxPower(20, false);
    radio.setFrequency(915.0);
    radio.setModemConfig(RH_RF95::Bw500Cr45Sf128);
}

void loop()
{
    checkButtonState();
    checkForAcknowledgements();
    syncStateWithRemote();
    delay(POWER_SAVE_SLEEP);
}

uint32_t last_flip = 0;
uint32_t wait_flip = 5000;
void checkButtonState()
{
    uint8_t last_state = curr_state;
    curr_state = digitalRead(BUTTON_PIN);

    if (last_state != curr_state)
    {
        Serial.print(curr_state);
        Serial.println("CHANGE");
    }
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
        Serial.print("RECV");
        Serial.println(buf[0]);

        Serial.print("RSVDelta:");
        Serial.println(millis() - start_flight);
        start_flight = 0;
    }
}

uint32_t last_tx = 0;
uint32_t wait_tx = 1000;
void syncStateWithRemote()
{
    if (remote_state != curr_state)
    {
        // If the current state is different than what the remote radio believes, update
        if (last_tx + wait_tx < millis())
        {

            if (start_flight == 0)
            {
                start_flight = millis();
            }
            Serial.print("SEEND");
            Serial.println(curr_state);
            uint8_t data[] = {curr_state};
            radio.send(data, sizeof(data));
            radio.waitPacketSent();
            Serial.print("SendDelta:");
            Serial.println(millis() - start_flight);
            last_tx = millis();
        }
    }
}

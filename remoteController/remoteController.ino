
#include <SPI.h>
#include <RH_RF95.h>

#define LED_PIN 13
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
    // curr_state = digitalRead(BUTTON_PIN);

    if (last_flip + wait_flip < millis())
    {
        Serial.print(millis());
        Serial.println("Flip");
        curr_state = (curr_state == LOW) ? HIGH : LOW;
        digitalWrite(LED_PIN, curr_state);
        last_flip = millis();
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

// void loop()
// {
//     Serial.println("Sending to rf95_server");
//     // Send a message to rf95_server
//     uint8_t data[] = "Hello World!";
//     rf95.send(data, sizeof(data));

//     rf95.waitPacketSent();
//     // Now wait for a reply
//     uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//     uint8_t len = sizeof(buf);

//     if (rf95.waitAvailableTimeout(3000))
//     {
//         // Should be a reply message for us now
//         if (rf95.recv(buf, &len))
//         {
//             Serial.print("got reply: ");
//             Serial.println((char *)buf);
//             //      Serial.print("RSSI: ");
//             //      Serial.println(rf95.lastRssi(), DEC);
//         }
//         else
//         {
//             Serial.println("recv failed");
//         }
//     }
//     else
//     {
//         Serial.println("No reply, is rf95_server running?");
//     }
//     delay(400);
// }

// #include <SPI.h>
// #include <RH_RF95.h>

// #include <Adafruit_NeoPixel.h>
// #ifdef __AVR__
// #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
// #endif

// #define BUTTON_PIN 5
// #define SIGNAL_PIN 10

// #define POWER_SAVE_SLEEP 1

// /************ Radio Setup ***************/

// // M0 Feather Values
// #define RADIO_FREQ 915.0
// #define RADIO_CS 8
// #define RADIO_INT 3
// #define RADIO_RST 4
// #define LED_PIN 13

// // Singleton instance of the radio driver
// RH_RF95 radio(RADIO_CS, RADIO_INT);

// uint32_t last_tx = 0;
// uint32_t wait_tx = 5;

// uint8_t remote_state = LOW;
// uint8_t curr_state = remote_state;

// /////////////////////
// // Debug
// /////////////////////
// uint32_t last_flip = 0;
// uint32_t wait_flip = 1000;

// void setup()
// {
//     Serial.begin(9600);
//     // while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

//     pinMode(SIGNAL_PIN, OUTPUT);
//     pinMode(LED_PIN, OUTPUT);
//     pinMode(RADIO_RST, OUTPUT);
//     digitalWrite(RADIO_RST, LOW);

//     delay(2000);
//     Serial.println("Feather RADIO RX Test!");
//     Serial.println();

//     // manual reset
//     digitalWrite(RADIO_RST, HIGH);
//     delay(10);
//     digitalWrite(RADIO_RST, LOW);
//     delay(10);

//     if (!radio.init())
//     {
//         Serial.println("RADIO radio init failed");
//         while (1) {
//           digitalWrite(LED_PIN, HIGH);
//           delay(10);
//           digitalWrite(LED_PIN, LOW);
//           delay(200);
//           digitalWrite(LED_PIN, HIGH);
//           delay(150);
//            digitalWrite(LED_PIN, LOW);
//           delay(600);
//         }
//             ;
//     }
//     Serial.println("RADIO radio init OK!");

//     if (!radio.setFrequency(RADIO_FREQ))
//     {
//         Serial.println("setFrequency failed");
//     }

//     radio.setTxPower(15, false); // range from 14-20 for power, 2nd arg must be true for 69HCW

//     Serial.print("radio @");
//     Serial.print((int)RADIO_FREQ);
//     Serial.println(" MHz");

//     digitalWrite(LED_PIN, HIGH);
// }

// void loop()
// {
//     // checkButtonState();
//     // checkForAcknowledgements();
//     // syncStateWithRemote();

//     Serial.println("Help");
//     delay(POWER_SAVE_SLEEP);
// }

// void checkButtonState()
// {
//     // curr_state = digitalRead(BUTTON_PIN);

//     if (last_flip + wait_flip < millis())
//     {
//         Serial.print(millis());
//         Serial.println("Flip");
//         curr_state = 1 - curr_state;
//         digitalWrite(LED_PIN, curr_state);
//     }
// }

// void checkForAcknowledgements()
// {
//     uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//     uint8_t len = sizeof(buf);
//     if (radio.available())
//     {
//         // Listen and record what the remote believes the state to be
//         radio.recv(buf, &len);
//         remote_state = buf[0];
//     }
// }

// void syncStateWithRemote()
// {
//     if (remote_state != curr_state)
//     {
//         // If the current state is different than what the remote radio believes, update
//         if (last_tx + wait_tx < millis())
//         {
//             uint8_t data[] = {curr_state};
//             radio.send(data, sizeof(data));
//             radio.waitPacketSent();
//             last_tx = millis();
//         }
//     }
// }
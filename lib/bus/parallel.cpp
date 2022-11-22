
#include "parallel.h"

#include <freertos/queue.h>

/* Dependencies */
#include "gpiox.h" // Required for PCF8575/PCA9674/MCP23017

/** PCF8575 instance */
GPIOX expander;

//#include <I2Cbus.hpp>

#include "iec.h"
#include "../../include/debug.h"


parallelBus PARALLEL;

//I2C_t& myI2C = i2c0;  // i2c0 and i2c1 are the default objects

static xQueueHandle ml_parallel_evt_queue = NULL;

static void IRAM_ATTR ml_parallel_isr_handler(void* arg)
{
    // Generic default interrupt handler
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(ml_parallel_evt_queue, &gpio_num, NULL);
    //Debug_printf("INTERRUPT ON GPIO: %d", arg);
}

static void ml_parallel_intr_task(void* arg)
{
    uint32_t io_num;

    // Setup default pin modes
    Debug_printv("clear");
    expander.clear();
    Debug_printv("pa2");
    expander.pinMode( PA2, GPIO_MODE_INPUT );
    Debug_printv("atn");
    expander.pinMode( ATN, GPIO_MODE_INPUT );
    Debug_printv("pc2");
    expander.pinMode( PC2, GPIO_MODE_INPUT );
    Debug_printv("flag2");
    expander.pinMode( FLAG2, GPIO_MODE_OUTPUT );
    Debug_printv("userport");
    expander.portMode( USERPORT_PB, GPIO_MODE_INPUT );

    while ( true ) 
    {
        if(xQueueReceive(ml_parallel_evt_queue, &io_num, portMAX_DELAY)) 
        {
            Debug_printv( "User Port Data Interrupt Received!" );
            // // Read I/O lines, set bits we want to read to 1
            // PARALLEL.readByte();

            // // Set RECEIVE/SEND mode   
            // if ( PARALLEL.status( PA2 ) )
            // {
            //     PARALLEL.mode = MODE_RECEIVE;
            //     expander.portMode( USERPORT_PB, GPIO_MODE_INPUT );
            //     PARALLEL.readByte();

            //     Debug_printv("receive <<< " BYTE_TO_BINARY_PATTERN " (%0.2d) " BYTE_TO_BINARY_PATTERN " (%0.2d)", BYTE_TO_BINARY(PARALLEL.flags), PARALLEL.flags, BYTE_TO_BINARY(PARALLEL.data), PARALLEL.data);
            // }
            // else
            // {
            //     PARALLEL.mode = MODE_SEND;
            //     expander.portMode( USERPORT_PB, GPIO_MODE_OUTPUT );

            //     Debug_printv("send    >>> " BYTE_TO_BINARY_PATTERN " (%0.2d) " BYTE_TO_BINARY_PATTERN " (%0.2d)", BYTE_TO_BINARY(PARALLEL.flags), PARALLEL.flags, BYTE_TO_BINARY(PARALLEL.data), PARALLEL.data);
            // }

            // // DolphinDOS Detection
            // if ( PARALLEL.status( ATN ) )
            // {
            //     if ( IEC.data.secondary == IEC_OPEN || IEC.data.secondary == IEC_REOPEN )
            //     {
            //         IEC.protocol.flags xor_eq DOLPHIN_ACTIVE;
            //         Debug_printv("dolphindos");
            //     }
            // }

            // // WIC64
            // if ( PARALLEL.status( PC2 ) )
            // {
            //     if ( PARALLEL.data == 0x57 ) // WiC64 commands start with 'W'
            //     {
            //         IEC.protocol.flags xor_eq WIC64_ACTIVE;
            //         Debug_printv("wic64");                  
            //     }
            // }
        }
    }
    
    vTaskDelay(portMAX_DELAY);
}


void parallelBus::setup ()
{
    // Setup i2c device
    expander.begin();
    
    // Create a queue to handle parallel event from ISR
    ml_parallel_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // Start task
    xTaskCreate(ml_parallel_intr_task, "ml_parallel_intr_task", 2048, NULL, 10, NULL);
    //xTaskCreatePinnedToCore(ml_parallel_intr_task, "ml_parallel_intr_task", 2048, NULL, 10, NULL, 0);

    // Setup interrupt for paralellel port
    gpio_config_t io_conf = {
        .pin_bit_mask = ( 1ULL << PIN_GPIOX_INT ),    // bit mask of the pins that you want to set
        .mode = GPIO_MODE_INPUT,                    // set as input mode
        .pull_up_en = GPIO_PULLUP_DISABLE,          // disable pull-up mode
        .pull_down_en = GPIO_PULLDOWN_DISABLE,      // disable pull-down mode
        .intr_type = GPIO_INTR_NEGEDGE              // interrupt of falling edge
    };
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_isr_handler_add((gpio_num_t)PIN_GPIOX_INT, ml_parallel_isr_handler, NULL);    
}


void parallelBus::handShake()
{
    // Signal received or sent
    
    // High
    expander.digitalWrite( FLAG2, HIGH );
    
    // Low
    expander.digitalWrite( FLAG2, LOW );
}


uint8_t parallelBus::readByte()
{

    this->data = expander.read( USERPORT_PB );
    this->flags = expander.PORT0;

    // Acknowledge byte received
    this->handShake();

    return this->data;
}

void parallelBus::writeByte( uint8_t byte )
{
    this->data = byte;

    Debug_printv("flags[%.2x] data[%.2x] byte[%.2x]", this->flags, this->data, byte);
    expander.write( byte, USERPORT_PB );

    // Tell receiver byte is ready to read
    this->handShake();
}

bool parallelBus::status( user_port_pin_t pin )
{
    if ( pin < 8 ) 
        return ( this->flags & ( 1 >> pin) );
    
    return ( this->data & ( 1 >> ( pin - 8) ) );
}



void wic64_command()
{
    // if (lastinput.startsWith("W")) // Commando startet mit W = Richtig
    // {
    //     if (lastinput.charAt(3) == 1)
    //     {
    //         ex = true;
    //         displaystuff("loading http");
    //         loader(lastinput);

    //         if (errorcode != "")
    //         {
    //             sendmessage(errorcode);
    //         }
    //     }

    //     if (lastinput.charAt(3) == 2)
    //     {
    //         ex = true;
    //         displaystuff("config wifi");
    //         httpstring = lastinput;
    //         sendmessage(setwlan());
    //         delay(3000);
    //         displaystuff("config changed");
    //     }

    //     if (lastinput.charAt(3) == 3)
    //     {
    //         ex = true; // Normal SW update - no debug messages on serial
    //         displaystuff("FW update 1");
    //         handleUpdate();
    //     }

    //     if (lastinput.charAt(3) == 4)
    //     {
    //         ex = true; // Developer SW update - debug output to serial
    //         displaystuff("FW update 2");
    //         handleDeveloper();
    //     }

    //     if (lastinput.charAt(3) == 5)
    //     {
    //         ex = true; // Developer SW update - debug output to serial
    //         displaystuff("FW update 3");
    //         handleDeveloper2();
    //     }

    //     if (lastinput.charAt(3) == 6)
    //     {
    //         ex = true;
    //         displaystuff("get ip");
    //         String ipaddress = WiFi.localIP().toString();
    //         sendmessage(ipaddress);
    //     }

    //     if (lastinput.charAt(3) == 7)
    //     {
    //         ex = true;
    //         displaystuff("get stats");
    //         String stats = __DATE__ " " __TIME__;
    //         sendmessage(stats);
    //     }

    //     if (lastinput.charAt(3) == 8)
    //     {
    //         ex = true;
    //         displaystuff("set server");
    //         lastinput.remove(0, 4);
    //         setserver = lastinput;
    //         preferences.putString("server", lastinput);
    //     }

    //     if (lastinput.charAt(3) == 9)
    //     {
    //         ex = true; // REM Send messages to debug console.
    //         displaystuff("REM");
    //         Serial.println(lastinput);
    //     }

    //     if (lastinput.charAt(3) == 10)
    //     {
    //         ex = true; // Get UDP data and return them to c64
    //         displaystuff("get upd");
    //         sendmessage(getudpmsg());
    //     }

    //     if (lastinput.charAt(3) == 11)
    //     {
    //         ex = true; // Send UDP data to IP
    //         displaystuff("send udp");
    //         sendudpmsg(lastinput);
    //     }

    //     if (lastinput.charAt(3) == 12)
    //     {
    //         ex = true; // wlan scanner
    //         displaystuff("scanning wlan");
    //         sendmessage(getWLAN());
    //     }

    //     if (lastinput.charAt(3) == 13)
    //     {
    //         ex = true; // wlan setup via scanlist
    //         displaystuff("config wifi id");
    //         httpstring = lastinput;
    //         sendmessage(setWLAN_list());
    //         displaystuff("config wifi set");
    //     }

    //     if (lastinput.charAt(3) == 14)
    //     {
    //         ex = true;
    //         displaystuff("change udp port");
    //         httpstring = lastinput;
    //         startudpport();
    //     }

    //     if (lastinput.charAt(3) == 15)
    //     {
    //         ex = true; // Chatserver string decoding
    //         displaystuff("loading httpchat");
    //         loader(lastinput);

    //         if (errorcode != "")
    //         {
    //             sendmessage(errorcode);
    //         }
    //     }

    //     if (lastinput.charAt(3) == 16)
    //     {
    //         ex = true;
    //         displaystuff("get ssid");
    //         sendmessage(WiFi.SSID());
    //     }

    //     if (lastinput.charAt(3) == 17)
    //     {
    //         ex = true;
    //         displaystuff("get rssi");
    //         sendmessage(String(WiFi.RSSI()));
    //     }

    //     if (lastinput.charAt(3) == 18)
    //     {
    //         ex = true;
    //         displaystuff("get server");

    //         if (setserver != "")
    //         {
    //             sendmessage(setserver);
    //         }
    //         else
    //         {
    //             sendmessage("no server set");
    //         }
    //     }

    //     if (lastinput.charAt(3) == 19)
    //     {
    //         ex = true; // XXXX 4 bytes header for padding !
    //         displaystuff("get external ip");
    //         loader("XXXXhttp://sk.sx-64.de/wic64/ip.php");

    //         if (errorcode != "")
    //         {
    //             sendmessage(errorcode);
    //         }
    //     }

    //     if (lastinput.charAt(3) == 20)
    //     {
    //         ex = true;
    //         displaystuff("get mac");
    //         sendmessage(WiFi.macAddress());
    //     }

    //     if (lastinput.charAt(3) == 30)
    //     {
    //         ex = true; // Get TCP data and return them to c64 INCOMPLETE
    //         displaystuff("get tcp");
    //         getudpmsg();

    //         if (errorcode != "")
    //         {
    //             sendmessage(errorcode);
    //         }
    //     }

    //     if (lastinput.charAt(3) == 31)
    //     {
    //         ex = true; // Get TCP data and return them to c64 INCOMPLETE
    //         displaystuff("send tcp");
    //         sendudpmsg(lastinput);
    //         sendmessage("");
    //         log_i("tcp send %s", lastinput);
    //     }

    //     if (lastinput.charAt(3) == 32)
    //     {
    //         ex = true;
    //         displaystuff("set tcp port");
    //         httpstring = lastinput;
    //         settcpport();
    //     }

    //     if (lastinput.charAt(3) == 99)
    //     {
    //         ex = true;
    //         displaystuff("factory reset");
    //         WiFi.begin("-", "-");
    //         WiFi.disconnect(true);
    //         preferences.putString("server", defaultserver);
    //         display.clearDisplay();
    //         delay(3000);
    //         ESP.restart();
    //     }
    // }
}
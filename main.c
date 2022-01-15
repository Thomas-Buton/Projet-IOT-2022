#include <string.h>

#include "board.h"
#include "xtimer.h"

/* Add necessary include here */
#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */
#include "lm75.h"
#include "lm75_params.h"
#include "scd30.h"
#include "scd30_params.h"
#include "scd30_internal.h"
#include "cayenne_lpp.h"

#define MEASUREMENT_INTERVAL_SECS (2)
#define TEST_BUT  GPIO_FALLING

scd30_t scd30_dev;
scd30_params_t params = SCD30_PARAMS;
scd30_measurement_t result;




/* Declare globally the sensor device descriptor */
extern semtech_loramac_t loramac;  /* The loramac stack descriptor */
static lm75_t lm75;
/* Device and application informations required for OTAA activation */
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x2e, 0x8a, 0x95, 0x7a, 0x66 , 0x01,0xfb,0x9c };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0xD3, 0x74, 0x7E, 0x67, 0xC6, 0x2E, 0x09, 0xEA, 0x80, 0x47, 0x19, 0x54, 0x87, 0x7D, 0x34, 0x11 };
static cayenne_lpp_t lpp;


static void cb(void *arg)
{
    printf("Pressed BTN%d\n", (int)arg);
    LED0_TOGGLE;
}

int main(void)
{
	int cnt = 0;
	if (gpio_init_int(BTN0_PIN, BTN0_MODE, TEST_BUT, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN0!");
        return 1;
    }
    ++cnt;

	 
    
    
    
    printf("SCD30 Test:\n");
    int i = 0;
    scd30_init(&scd30_dev, &params);
    uint16_t pressure_compensation = SCD30_DEF_PRESSURE;
    uint16_t value = 0;
    uint16_t interval = MEASUREMENT_INTERVAL_SECS;

    scd30_set_param(&scd30_dev, SCD30_INTERVAL, MEASUREMENT_INTERVAL_SECS);
    scd30_set_param(&scd30_dev, SCD30_START, pressure_compensation);

    scd30_get_param(&scd30_dev, SCD30_INTERVAL, &value);
    printf("[test][dev-%d] Interval: %u s\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_T_OFFSET, &value);
    printf("[test][dev-%d] Temperature Offset: %u.%02u C\n", i, value / 100u,
           value % 100u);
    scd30_get_param(&scd30_dev, SCD30_A_OFFSET, &value);
    printf("[test][dev-%d] Altitude Compensation: %u m\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_ASC, &value);
    printf("[test][dev-%d] ASC: %u\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_FRC, &value);
    printf("[test][dev-%d] FRC: %u ppm\n", i, value);

    while (i < TEST_ITERATIONS) {
        xtimer_sleep(1);
        scd30_read_triggered(&scd30_dev, &result);
        printf(
            "[scd30_test-%d] Triggered measurements co2: %.02fppm,"
            " temp: %.02f°C, hum: %.02f%%. \n", i, result.co2_concentration,
            result.temperature, result.relative_humidity);
        i++;
    }

    i = 0;
    scd30_start_periodic_measurement(&scd30_dev, &interval,
                                     &pressure_compensation);

    while (i < TEST_ITERATIONS) {
        xtimer_sleep(MEASUREMENT_INTERVAL_SECS);
        scd30_read_periodic(&scd30_dev, &result);
        printf(
            "[scd30_test-%d] Continuous measurements co2: %.02fppm,"
            " temp: %.02f°C, hum: %.02f%%. \n", i, result.co2_concentration,
            result.temperature, result.relative_humidity);
        i++;
    }
		
    scd30_stop_measurements(&scd30_dev);
 /* configure the device parameters */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);
    /* change datarate to DR5 (SF7/BW125kHz) */
    semtech_loramac_set_dr(&loramac, 5);
    /* start the OTAA join procedure */
    printf("start join\n");
    
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");
    if (lm75_init(&lm75, &lm75_params[0]) != LM75_SUCCESS) {
        puts("Sensor initialization failed");
        return 1;
    }

    while (1) {
        /* do some measurements */
        int temperature = 0;
        if (lm75_get_temperature(&lm75, &temperature)!= LM75_SUCCESS) {
            puts("Cannot read temperature!");
        }
        //char message[32];
        //sprintf(message, "T:%d.%dC",(temperature / 1000), (temperature % 1000));
        cayenne_lpp_add_temperature(&lpp, 0, (float)temperature / 1000);
        //printf("Sending message '%s'\n", message);
		scd30_read_periodic(&scd30_dev, &result);

		cayenne_lpp_add_relative_humidity(&lpp, 1, (float)result.relative_humidity);
		cayenne_lpp_add_analog_output(&lpp, 2, (float)result.co2_concentration);
        /* send the message here */
        uint8_t ret = semtech_loramac_send(&loramac, lpp.buffer, lpp.cursor);
 		if (ret ==0)  {
            printf("fail");
        }
        else {
            printf("check");
        }
        cayenne_lpp_reset(&lpp);
        /* wait 20 seconds between each message */
        xtimer_sleep(1);
        
    }
    
    return 0; /* should never be reached */
}

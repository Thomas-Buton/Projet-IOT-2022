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
volatile int cas = 0;


/* interuption bouton test */
static void cb(void *arg)
{
    printf("Pressed BTN%d\n", (int)arg);
    cas += 1;
    if (cas > 2){
    	cas = 0;
    }
    
}

/* interuption panic bouton */
static void cb1(void *arg)
{
    printf("Pressed BTN1%d\n", (int)arg);
    cas = 2;
    xtimer_usleep(50);
}



int main(void)
{
	/* initialisation bouton */
	int cnt = 0;
	int cnt1 = 0;
	if (gpio_init_int(BTN0_PIN, BTN0_MODE, TEST_BUT, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN0!");
        return 1;
    }
    if (gpio_init_int(BTN1_PIN, BTN1_MODE, TEST_BUT, cb1, (void *)cnt1) < 0) {
        puts("[FAILED] init BTN1!");
        return 1;
    }
    ++cnt;

	 
    
    
    /* initialisation capteur CO2 / Humidité / temp */
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
        /* envoie du cas */
        cayenne_lpp_add_analog_input(&lpp, 0, cas);
        /* mesure de la temperature */
        int temperature = 0;
        if (lm75_get_temperature(&lm75, &temperature)!= LM75_SUCCESS) {
            puts("Cannot read temperature!");
        }
		/* envoie de la temerature dans le channel 1 */
        cayenne_lpp_add_temperature(&lpp, 1, (float)temperature / 1000);
		
		/* mesure du capteur sdc */
		scd30_read_periodic(&scd30_dev, &result);
		
		/* envoie de l'humidité dans le channel 2 */
		cayenne_lpp_add_relative_humidity(&lpp, 2, (float)result.relative_humidity);
		
		/* cas 0 : normal : rien ne se passe*/
		if (cas == 0)
		{
			LED0_OFF;
			/* envoie de la concentration de CO2 dans le channel 3 */
			cayenne_lpp_add_analog_input(&lpp, 3, (float)result.co2_concentration/100);
		}
		/* cas 1 : anormal : led qui clignote lentement*/
		else if (cas == 1)
		{
			cayenne_lpp_add_analog_input(&lpp, 3, 17.5);
			LED0_TOGGLE;
		}
		/* cas 2 : anormal ++ : led qui clignote rapidement*/
		else if (cas == 2)
		{
			cayenne_lpp_add_analog_input(&lpp, 3, 22.5);
			LED0_TOGGLE;
			xtimer_sleep(1);
			LED0_TOGGLE;
		}
			
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
        xtimer_sleep(10);
        
    }
    
    return 0; /* should never be reached */
}

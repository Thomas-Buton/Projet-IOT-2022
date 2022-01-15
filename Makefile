# name of the application
APPLICATION = chirpstack-sensors

# The default board is ST B-L072Z-LRWAN1 LoRa discovery board
BOARD ?= lora-e5-dev

RIOTBASE ?= /home/user/Desktop/iot-lab-training/riot/RIOT

#
# Here we add the modules that are needed
#
USEMODULE += auto_init_loramac
USEMODULE += xtimer
USEMODULE += lm75a
#
# The application needs LoRaWAN related modules and variables:
#
USEMODULE += scd30
USEMODULE += printf_float

TEST_ITERATIONS ?= 3
# export iterations for continuous measurements
CFLAGS += -DTEST_ITERATIONS=$(TEST_ITERATIONS)

# The Semtech LoRa radio device (SX126X)
USEMODULE += sx126x_stm32wl
FEATURES_REQUIRED += periph_gpio_irq
# The Semtech Loramac package
USEPKG += semtech-loramac
USEPKG += cayenne-lpp
# Default region is Europe and default band is 868MHz
LORA_REGION ?= EU868

# This must be the last line of the Makefile
include $(RIOTBASE)/Makefile.include

#include <Arduino.h>

#include "hal/canbus/canbus.h"
#include "hal/swm_inputs/swm_inputs.h"
#include "hal/daylight_sensor/daylight_sensor.h"

#include "modules/ModuleBase.h"
#include "hal/lcd/lcd.h"

#include "modules/can_logger/can_logger.h"

ModuleBase *modules[128];
int module_count = 0;

CANbus canbus = CANbus::getInstance();
SWM_Inputs swm_inputs = SWM_Inputs::getInstance();
DaylightSensor daylight_sensor = DaylightSensor::getInstance();
CAN_logger can_logger = CAN_logger::getInstance();

LCD lcd = LCD::getInstance();

// put function declarations here:
void addModule(ModuleBase *module);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    addModule(&lcd);

    lcd.enable();
    lcd.clear();
    char initmsg[] = "OpenCFE (P2 '04)Ver:  0.0.1 Beta";
    lcd.print(initmsg, 32);
}

void loop() {
    // put your main code here, to run repeatedly:
    String command;
    if (Serial.available()) {
        command = Serial.readString();
    }

    if (command.startsWith("can-log")) {
        String nextSection = command.substring(command.indexOf(' ')+1, command.indexOf('\n'));
        if (nextSection.startsWith("enable")) {
            can_logger.enable();
            Serial.println("can-log ENABLED");
        } else if (nextSection.startsWith("disbale")) {
            can_logger.disable();
            Serial.println("can-log DISABLED");
        } else if (nextSection.startsWith("set-filter")) {
            String filterID = nextSection.substring(nextSection.indexOf(' ')+1, nextSection.indexOf('\n'));
            int filter = std::stoul(filterID.c_str(), nullptr, 16);
            can_logger.setFilter(filter);
            printf("can-log id filter set to %08X\n", filter);
        } else if (nextSection.startsWith("enable-focus")) {
            can_logger.setFocused(true);
            Serial.println("can-log focus ENABLED");
        } else if (nextSection.startsWith("disable-focus")) {
            can_logger.setFocused(false);
            Serial.println("can-log focus DISABLED");
        } else if (nextSection.startsWith("set-focus-byte")) {
            can_logger.setFocusByte(7);
            printf("can-log focus byte index set to %d", 7);
        }
    } else if (command.startsWith("lcd")) {
        String nextSection = command.substring(command.indexOf(' ')+1, command.indexOf('\n'));
        if (nextSection.startsWith("enable")) {
            lcd.enable();
            printf("lcd ENABLED\n");
        } else if (nextSection.startsWith("disable")) {
            lcd.disable();
            printf("lcd DISABLED\n");
        } else if (nextSection.startsWith("clear")) {
            lcd.clear();
            printf("lcd CLEARED\n");
        } else if (nextSection.startsWith("print")) {
            String textToPrint = nextSection.substring(nextSection.indexOf(' ')+1, nextSection.indexOf('\n'));
            char copy[31];
            textToPrint.toCharArray(copy, 31);
            printf("lcd print %s\n", copy);
            lcd.print(copy, 31);
        }
    }

    canbus.rx_periodic();

    for (int mod = 0; mod < module_count; mod++) {
        modules[mod]->periodicLoop();
    }
}

// put function definitions here:
void addModule(ModuleBase *module) {
  modules[module_count] = module;
  module_count++;
}
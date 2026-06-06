#ifndef JOYSTICK_H
#define JOYSTICK_H

void setup_joystick();
uint8_t menu_control();
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value);

#endif
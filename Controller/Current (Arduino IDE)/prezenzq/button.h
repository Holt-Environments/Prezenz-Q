
#include <Button.h>

#define SINGLE_STATE_BUTTON 5
#define SINGLE_STATE_BUTTON_LED 6
#define BUTTON_HOLD_DURATION 3000 // 3 Seconds

enum button_state{BUTTON_OFF, BUTTON_CLICKED, BUTTON_HELD};

void button_init();
enum button_state button_get_state();
void button_led_set(int _value);

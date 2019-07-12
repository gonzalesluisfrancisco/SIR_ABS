#include "LocalRemoteMan_2.h"

int localremote_state = REMOTE;
int buttonState_up = SW_OPEN;   
int buttonState_dw = SW_OPEN;  
int buttonState_lr = SW_OPEN;   
int lastButtonState_up = LOW;   
int lastButtonState_dw = LOW;
int lastButtonState_lr = LOW;   
unsigned long lastDebounceTime_up = 0;  
unsigned long lastDebounceTime_dw = 0;
unsigned long lastDebounceTime_lr = 0;
unsigned long debounceDelay       = 50;    
int countTime_up                  = 0;  
int countTime_dw                  = 0;  
int countTime_lr                  = 0;
int return_to_remote              = 1;


uint8_t dw_pins = 0;
uint8_t up_pins = 0;
typedef enum {
	IDLE,
	DETECTED,
	EXECUTION,
	WAIT_RELEASE
} but_states;

but_states state_swup  = IDLE;
but_states state_swdw  = IDLE;
but_states state_swlr  = IDLE;
but_states state_swsim = IDLE;
  
void config_SW(void){
	pinMode(SW_UP, INPUT_PULLUP);
	pinMode(SW_DW, INPUT_PULLUP);
	pinMode(SW_LR, INPUT_PULLUP);
}

void debounceSW_UP(void){
  int reading = digitalRead(SW_UP);
  if (reading != lastButtonState_up) {
    lastDebounceTime_up = millis();
  }   
  if ((millis() - lastDebounceTime_up) > debounceDelay) {
    buttonState_up = reading;
  }
  lastButtonState_up = reading;
  
}

void debounceSW_DW(void){
  int reading = digitalRead(SW_DW);
  if (reading != lastButtonState_dw) {
    lastDebounceTime_dw = millis();
  }   
  if ((millis() - lastDebounceTime_dw) > debounceDelay) {
    buttonState_dw = reading;
  }
  lastButtonState_dw = reading;
  
}

void debounceSW_LR(void){
  int reading = digitalRead(SW_LR);
  /*
  DisableWatchDog();
  Serial.print(reading);
  Serial.println("######");
  EnableWatchDog();
  */
  if (reading != lastButtonState_lr) {
    lastDebounceTime_lr = millis();
	
  }   
  if ((millis() - lastDebounceTime_lr) > debounceDelay) {
    buttonState_lr = reading;
  }
  lastButtonState_lr = reading;
  
}

void readSW_UPDW(void){
  
  debounceSW_UP();
  debounceSW_DW();
  
  if((state_swup == IDLE) && (time_press_up() > SW_UP_TIME))
	state_swup = DETECTED;
  
  if((state_swdw == IDLE) && (time_press_dw() > SW_DW_TIME))
	state_swup = DETECTED;	
  
  if((state_swsim == IDLE) && (time_press_sim() > SW_SIM_TIME))
	state_swsim = DETECTED;
  
  ////////////////
  
  if((state_swup == DETECTED) && buttonState_up == HIGH){
	if(state_swsim != DETECTED)
		state_swup = EXECUTION;
	else
		state_swup = IDLE;
  }
  
  if((state_swdw == DETECTED) && buttonState_dw == HIGH){
	if(state_swsim != DETECTED)
		state_swdw = EXECUTION;
	else
		state_swdw = IDLE;
  }
  
  if((state_swsim     == DETECTED) &&
     (buttonState_up  == HIGH)     && 
	 (buttonState_dw  == HIGH)){
	state_swsim = EXECUTION;
	state_swup = IDLE;
	state_swdw = IDLE;
  }
  
  ///////////////

  if((state_swup == EXECUTION)){
	exe_up();
	return_to_remote = 0;
    state_swup = IDLE;
  }
  
  if((state_swdw == EXECUTION)){
	exe_dw();
    return_to_remote = 0;	
    state_swdw = IDLE;
  }
  
  if((state_swsim == EXECUTION)){
	exe_sim();  
	return_to_remote = 0;
	state_swsim = IDLE;
  }
}

void readSW_UPDWLR(void){
  ResetWatchDogTimer();
  check_return_to_remote();
 
  debounceSW_UP();
  debounceSW_DW();
  debounceSW_LR();
  /*
  DisableWatchDog();
  Serial.print(buttonState_lr);
  Serial.println("****");
  EnableWatchDog();
  */
  if((state_swup == IDLE) && (time_press_up() > SW_UP_TIME))
	state_swup = DETECTED;
  
  if((state_swdw == IDLE) && (time_press_dw() > SW_DW_TIME))
	state_swdw = DETECTED;	
  
  if((state_swlr == IDLE) && (time_press_lr() > SW_LR_TIME))
	state_swlr = DETECTED;
  
  ////////////////
  
  if((state_swup == DETECTED)){
	state_swup = EXECUTION;
  }
  
  if((state_swdw == DETECTED)){
	state_swdw = EXECUTION;
  }
  
  if((state_swlr == DETECTED)){
	state_swlr = EXECUTION;
  }
  
  ///////////////

  if((state_swup == EXECUTION)){
	exe_up();
	return_to_remote = 0;
    state_swup = WAIT_RELEASE;
  }
  
  if((state_swdw == EXECUTION)){
	exe_dw();	
	return_to_remote = 0;
    state_swdw = WAIT_RELEASE;
  }
  
  if((state_swlr == EXECUTION)){
	exe_lr();  
	return_to_remote = 0;
	state_swlr = WAIT_RELEASE;
	Serial.println("EXE LR");
  }
  
  ////////////

  if((state_swup == WAIT_RELEASE)){
	if(time_release_up() > SW_UP_TIME)
		state_swup = IDLE;
  }
  
  if((state_swdw == WAIT_RELEASE)){
	if(time_release_dw() > SW_DW_TIME)
		state_swdw = IDLE;
  }
  
  if((state_swlr == WAIT_RELEASE)){
	DisableWatchDog();
	//Serial.println("WAITING LR");
	
	if(time_release_lr() > SW_LR_TIME){
		state_swlr = IDLE;
		Serial.println("IDLE LR");
	}
	EnableWatchDog();
  }  
  
  ///////////////

}


void check_return_to_remote(void){
	static unsigned long time_return_to_remote = 0;
	uint32_t aux_32;
	if((return_to_remote == 0))
	{
		time_return_to_remote = millis();
		return_to_remote      = 1;
	}
	
	if((millis()-time_return_to_remote) > MS_TO_RETURN_REMOTE && (localremote_state != REMOTE)){	
	    //Serial.println("Regresando a REMOTE");
		localremote_state = REMOTE;
		aux_32=Read_EEPROM(GetActualBeam());
		dw_pins=aux_32%8;
		up_pins=aux_32/8;
		actualiza_up_pins(up_pins);
		actualiza_dw_pins(dw_pins);
	}
	
}

unsigned long time_press_up(void){
  static unsigned long start_press_up = 0;
  if(buttonState_up == SW_OPEN){
    start_press_up = 0;
	return 0;
  }
  if(start_press_up == 0){
    start_press_up = millis();  
  }
  
  return (millis()-start_press_up);
}

unsigned long time_press_dw(void){
  static unsigned long start_press_dw = 0;
  if(buttonState_dw == SW_OPEN){
    start_press_dw = 0;
	return 0;
  }
  if(start_press_dw == 0){
    start_press_dw = millis();  
  }
  
  return (millis()-start_press_dw);
}

unsigned long time_press_lr(void){
  static unsigned long start_press_lr = 0;
  if(buttonState_lr == SW_OPEN){
    start_press_lr = 0;
	return 0;
  }
  if(start_press_lr == 0){
    start_press_lr = millis();  
  }
  
  return (millis()-start_press_lr);
}

unsigned long time_release_up(void){
  static unsigned long start_release_up = 0;
  if(buttonState_up == SW_PRESS){
    start_release_up = 0;
	return 0;
  }
  if(start_release_up == 0){
    start_release_up = millis();  
  }
  
  return (millis()-start_release_up);
}

unsigned long time_release_dw(void){
  static unsigned long start_release_dw = 0;
  if(buttonState_dw == SW_PRESS){
    start_release_dw = 0;
	return 0;
  }
  if(start_release_dw == 0){
    start_release_dw = millis();  
  }
  
  return (millis()-start_release_dw);
}

unsigned long time_release_lr(void){
  static unsigned long start_release_lr = 0;
  //Serial.print(buttonState_lr);
  //Serial.println("****");
  
  if(buttonState_lr == SW_PRESS){
    start_release_lr = 0;
	return 0;
  }
  
  if(start_release_lr == 0){
    start_release_lr = millis();  
  }
  //Serial.println("!!!!");
  return (millis()-start_release_lr);
}

unsigned long time_press_sim(void){
  static unsigned long start_press_sim = 0;
  if((buttonState_up == SW_OPEN) || (buttonState_dw == SW_OPEN)){
    start_press_sim = 0;
	return 0;
  }
  if(start_press_sim == 0){
    start_press_sim = millis();  
  }
  
  return (millis()-start_press_sim);
}

void exe_up(void){
  Serial.println("Ejecutando UP");
  if(localremote_state == REMOTE) return;
  
  up_pins++;
  if(up_pins == 8) up_pins = 0;
  actualiza_up_pins(up_pins);
  
}

void exe_dw(void){
  Serial.println("Ejecutando DW");
  if(localremote_state == REMOTE) return;
  
  dw_pins++;
  if(dw_pins == 8) dw_pins = 0;
  actualiza_dw_pins(dw_pins);
  
}

void exe_lr(void){
  Serial.println("Ejecutando LR");
  uint32_t aux_32;
  if(localremote_state == LOCAL){ 
    localremote_state = REMOTE;
	aux_32=Read_EEPROM(GetActualBeam());
	dw_pins=aux_32%8;
	up_pins=aux_32/8;
	actualiza_up_pins(up_pins);
	actualiza_dw_pins(dw_pins);
	return;
  } 
  localremote_state = LOCAL;
  return;
   
}

void exe_sim(void){
  if(localremote_state == REMOTE){ 
    localremote_state = LOCAL;
	return;
  } 
  localremote_state = REMOTE;
  return;
   
}

void actualiza_up_pins(uint8_t valor){
  DisableWatchDog();
  digitalWrite(FF_CLK,LOW);
  delayMicroseconds(10);//Clearing the FFD_CLK for 10 microseconds. 

  digitalWrite(CTRL_UP0,((valor%2==1)? HIGH:LOW));
  valor /=2;
  digitalWrite(CTRL_UP1,((valor%2==1)? HIGH:LOW));
  valor /=2;  
  digitalWrite(CTRL_UP2,((valor%2==1)? HIGH:LOW));
  
  digitalWrite(FF_CLK,HIGH);//Setting the FFD_CLK for 10 microsend, here the rising edge should be detected by the FF-D chip
  delayMicroseconds(10);//Clearing the FFD_CLK for 10 microseconds. 
  delay(20);
  EnableWatchDog();
}

void actualiza_dw_pins(uint8_t valor){
  DisableWatchDog();
  digitalWrite(FF_CLK,LOW);
  delayMicroseconds(10);//Clearing the FFD_CLK for 10 microseconds. 

  digitalWrite(CTRL_DW0,((valor%2==1)? HIGH:LOW));
  valor /=2;
  digitalWrite(CTRL_DW1,((valor%2==1)? HIGH:LOW));
  valor /=2;  
  digitalWrite(CTRL_DW2,((valor%2==1)? HIGH:LOW));
  
  digitalWrite(FF_CLK,HIGH);//Setting the FFD_CLK for 10 microsend, here the rising edge should be detected by the FF-D chip
  delayMicroseconds(10);//Clearing the FFD_CLK for 10 microseconds. 
  delay(20);
  EnableWatchDog();
}

void actualiza_lr_mens(int estado){
    char cad[4];
	char aux_c;
	if(estado==LOCAL) aux_c = 'L';
	else aux_c = 'R';
	
	sprintf(cad,"%c",aux_c);
	PrintStr(8,3,cad); //Printing the message to the LCD
}

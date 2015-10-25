#include "VarSpeedServo.h" 
//////////////////////////////////////////////////////////
#include "common_coffin.h" 
#include "LEDFader.h" 


#define HAS_SoftSer

//#define HAS_DEBUG1

#define  HAS_MP3_JAW

#define S_TRACK 1
#define T_TRACK 13

#define HAS_CMD

#ifdef HAS_SoftSer
 
#define  HAS_MUSIC
#include <SoftwareSerial.h>



#endif
///////// PINS 
#define  neckservoH_pin 10
#define rxPin 19
#define txPin 18
#define jawservo_pin 9

#define  EYE_R_LED_PIN 5
#define  EYE_L_LED_PIN 6

///////////////////////////
#define FADE_TIME 2000

#define LOOK_MID   90 
#define LOOK_RIGHT_SPEED  30 

//////////////////////////////////// 

#define LED_DIR_UP 1
#define LED_DIR_DOWN -1
////
boolean jawopen = false;
#define jawclosedpos  90
#define jawopenpos   150 



/////////////////////////////////////
LEDFader LedEyeR;
LEDFader LedEyeL;
int LedEyeRdirection = LED_DIR_UP;
int LedEyeLdirection = LED_DIR_UP;

pr_stats programloopstat = pr_none;

long other_inputvalue = NOVALUEINPUT;



loop_stats loopstatus = LOOP_START;

pr_stats otherbooard_pr_stats = pr_none;

unsigned long start_loop_time = 0;

#ifdef HAS_MUSIC
SoftwareSerial serial_MP3(rxPin, txPin); // RX, TX


#endif

////////////////////////////////////////////////////////
VarSpeedServo neckservoH;  // create servo object to control a servo 

VarSpeedServo jawservo;  // create servo object to control a servo 

#define totalneckseqcount 2
int neckValsH[totalneckseqcount] = { 0x2050, 0x206E };


byte neckseqcount = 0;
 int track = S_TRACK;
 
void setup() {
#ifdef HAS_CMD
	Serial.begin(38400);
#endif

	 

#ifdef HAS_MUSIC
	serial_MP3.begin(38400);
#endif
 

  pinMode(EYE_L_LED_PIN,OUTPUT);
  pinMode(EYE_R_LED_PIN,OUTPUT);
  pinMode(A3,INPUT);
	LedEyeL = LEDFader(EYE_L_LED_PIN);
	LedEyeL.fade(255, FADE_TIME);

	LedEyeR = LEDFader(EYE_R_LED_PIN);
	LedEyeR.fade(255, FADE_TIME);

	neckservoH.attach(neckservoH_pin);  // attaches the servo on pin 9 to the servo object
	neckservoH.write(90, 20, true); // set the intial position of the servo, as fast as possible, wait until done
	jawservo.attach(jawservo_pin);  // attaches the servo on pin 9 to the servo object
 
  jawservo.write(jawopenpos,20,true); // set the intial position of the servo, as fast as possible, wait until done
  jawservo.write(jawclosedpos,20,true); // set the intial position of the servo, as fast as possible, wait until done

//	jawservo.write(jawopenpos, 5, true); // set the intial position of the servo, as fast as possible, wait until done


	loopstatus = LOOP_START;
	delay(5000);
	start_loop_time = millis();
 
}


void ledloop() {

	if (LedEyeR.enabled)
	{


		LedEyeR.update();


		// LED no longer fading, switch direction
		if (!LedEyeR.is_fading()) {

			// Fade down
			if (LedEyeRdirection == LED_DIR_UP) {
				LedEyeR.fade(0, FADE_TIME);
				LedEyeRdirection = LED_DIR_DOWN;
			}
			// Fade up
			else {
				LedEyeR.fade(255, FADE_TIME);
				LedEyeRdirection = LED_DIR_UP;
			}
		}
	}


	if (LedEyeL.enabled)
	{


		LedEyeL.update();


		// LED no longer fading, switch direction
		if (!LedEyeL.is_fading()) {

			// Fade down
			if (LedEyeLdirection == LED_DIR_UP) {
				LedEyeL.fade(0, FADE_TIME);
				LedEyeLdirection = LED_DIR_DOWN;
			}
			// Fade up
			else {
				LedEyeL.fade(255, FADE_TIME);
				LedEyeLdirection = LED_DIR_UP;
			}
		}
	}


}


/////////////////////////////////////////////////////////////////////////////////////// do_before_open_loop
byte do_before_open_loop() {



	loopstatus = LOOP_RUN;


	return 	loopstatus;

}


/////////////////////////////////////////////////////////////////////////////////////// do_open_loop
/////////////////////////////////////////////////////////////////////////////////////// do_wakeup_loop
byte do_wakeup_loop() {



	int curposH = -1;
	int val = LOOK_MID  -80;
	switch (loopstatus)
	{
	case LOOP_START:
		// look to the right 
		neckservoH.write(val, LOOK_RIGHT_SPEED, false);

		loopstatus = LOOP_RUN;

		break;
	case LOOP_RUN:
		// play the sound 
		curposH = neckservoH.read();
		if (curposH == val) {

			loopstatus = LOOP_FINISH;
		}


		break;
	case LOOP_FINISH:
		break;

	}

	return 	loopstatus;

}



/////////////////////////////////////////////////////////////////////////////////////// do_situp_loop
byte do_situp_loop() {

	return 	LOOP_FINISH;

}



/////////////////////////////////////////////////////////////////////////////////////// do_talk_loop

byte do_talk_loop() {

	int valMp3 = 0;

	switch (loopstatus)
	{
	case LOOP_START:
		// look to the right 
#ifdef HAS_MUSIC
  //serial_MP3.write('F');
		serial_MP3.write('t');
	  track++;
    if (track > T_TRACK ){
    track = S_TRACK;
    }
		serial_MP3.write(track);

#endif

		LedEyeR.enabled = true;
		LedEyeL.enabled = true;

		loopstatus = LOOP_RUN;

		break;
	case LOOP_RUN:
		// play the sound 





		
		//valMp3 = analogRead(A3);
#ifdef HAS_MP3_JAW
		valMp3 = analogRead(A3);
	
#ifdef HAS_DEBUG1
		Serial.println(valMp3);
#endif
		




		switch (valMp3)
		{
		 
	 
			case	600 ... 1000:
				jawservo.write(jawopenpos);
				delay(3);
				break;

		default:
			jawservo.write(jawclosedpos);
			delay(3);
			break;
		}
	//	jawservo.write(valMp3);

	 


		while (serial_MP3.available()) {

			char inChar = (char)serial_MP3.read();
			switch (inChar)
			{
			case 'X':
				jawservo.write(jawclosedpos);
				loopstatus = LOOP_FINISH;
				break;
			case 'x':
				jawservo.write(jawclosedpos);
				loopstatus = LOOP_FINISH;
				break;
			case 'E':
				jawservo.write(jawclosedpos);
				loopstatus = LOOP_FINISH;
				break;

			default:
				break;
			}
			 
		}
		 
/*

		if (!jawopen){

			jawservo.write(jawclosedpos); // set the intial position of the servo, as fast as possible, wait until done
			jawopen = true;
		}
		else{
			jawservo.write(jawopenpos); // set the intial position of the servo, as fast as possible, wait until done
			jawopen = false;
		}
		

		if ((start_loop_time + 5000) < millis()){
		jawservo.write(jawclosedpos, 30, false);
		loopstatus = LOOP_FINISH;

		}
		*/
#endif		

#ifndef HAS_MP3_JAW



		if (!jawopen) {

			jawservo.write(jawclosedpos); // set the intial position of the servo, as fast as possible, wait until done
			jawopen = true;
		}
		else {
			jawservo.write(jawopenpos); // set the intial position of the servo, as fast as possible, wait until done
			jawopen = false;
		}

	//	if ((start_loop_time + 5000) < millis()) {
	//		jawservo.write(jawclosedpos, 30, false);
	//		loopstatus = LOOP_FINISH;
		


		while (serial_MP3.available()) {

			char inChar = (char)serial_MP3.read();
			switch (inChar)
			{
			case 'X':
				jawservo.write(jawclosedpos);
				loopstatus = LOOP_FINISH;
				break;
			case 'x':
				jawservo.write(jawclosedpos);
				loopstatus = LOOP_FINISH;
				break;
			case 'E':
				jawservo.write(jawclosedpos);
				loopstatus = LOOP_FINISH;
				break;

			default:
				break;
			}

		}

#endif	




		break;
	case LOOP_FINISH:
		break;

	}

	return 	loopstatus;

}

/////////////////////////////////////////////////////////////////////////////////////// do_sitdown_loop
byte do_sitdown_loop() {

#ifdef HAS_CMD
 	Serial.print(pr_close);
	Serial.println('p');
#endif
    LedEyeR.enabled = false;
    LedEyeL.enabled = false;
        LedEyeR.set_value(0);
       LedEyeL.set_value(0);
	return 	LOOP_FINISH;

}


/////////////////////////////////////////////////////////////////////////////////////// do_sleep_loop

#define LOOK_UP_SPEED   40 
byte do_sleep_loop() {




	int curposH = -1;
	int val = LOOK_MID;
	switch (loopstatus)
	{
	case LOOP_START:
		// look to the right 
		neckservoH.write(val, LOOK_RIGHT_SPEED, false);
		LedEyeR.enabled = false;
		LedEyeL.enabled = false;
        LedEyeR.set_value(0);
       LedEyeL.set_value( 0);
		loopstatus = LOOP_RUN;

		break;
	case LOOP_RUN:
		// play the sound 
		curposH = neckservoH.read();
		if (curposH == val) {



			loopstatus = LOOP_FINISH;
		}


		break;
	case LOOP_FINISH:
		break;

	}

	return 	loopstatus;

}




 
/////////////////////////////////////////////////////////////////////////////////////// do_after_close_loop
byte do_after_close_loop() {

	if ((start_loop_time + 5000) < millis()){
		jawservo.write(jawclosedpos, 30, false);
		loopstatus = LOOP_FINISH;
	}
	else
		loopstatus = LOOP_RUN;

	return 	loopstatus;

}




void   programloop() {



	switch (programloopstat)
	{
 

	case pr_before_open_loop:
		switch (do_before_open_loop()) {

		case LOOP_START:

			break;
		case LOOP_RUN:
			break;
		case LOOP_FINISH:
			loopstatus = LOOP_START;
			start_loop_time = millis();
#ifdef HAS_CMD
			programloopstat = pr_none;
#else
			programloopstat = pr_open;
#endif

			return;

		}

	case stat_openandsitdone:
 
 
			programloopstat = pr_situp ;
  	
	     break;

		
/*
	case pr_open:
		switch (do_open_loop()) {

		case LOOP_START:
			break;
		case LOOP_RUN:
			break;
		case LOOP_FINISH:
			loopstatus = LOOP_START;
			start_loop_time = millis();
			programloopstat = pr_situp;
			return;

		}


		break;
*/
	case pr_situp:
		switch (do_situp_loop()) {

		case LOOP_START:
			break;
		case LOOP_RUN:
			break;
		case LOOP_FINISH:
			loopstatus = LOOP_START;
			start_loop_time = millis();
			programloopstat = pr_wake_up;
			return;

		}

		break;


	case pr_wake_up:
		switch (do_wakeup_loop()) {

		case LOOP_START:
			break;
		case LOOP_RUN:
			break;
		case LOOP_FINISH:
			loopstatus = LOOP_START;
			start_loop_time = millis();
			programloopstat = pr_talk;
			return;

		}


		break;
	case pr_talk:
		switch (do_talk_loop()) {

		case LOOP_START:
			break;
		case LOOP_RUN:
			break;
		case  LOOP_FINISH:
			loopstatus = LOOP_START;
			start_loop_time = millis();
		 
 
			programloopstat = pr_sitdown;
 
 

			return;

		}

		break;

	case pr_sitdown:
		switch (do_sitdown_loop()) {

		case LOOP_START:
 			break;
		case LOOP_RUN:
			break;
		case  LOOP_FINISH:
			loopstatus = LOOP_START;
			start_loop_time = millis();
			programloopstat = pr_sleep;
			return;

		}

		break;

	case pr_sleep:
		switch (do_sleep_loop()) {

		case LOOP_START:
			break;
		case LOOP_RUN:
			break;
		case  LOOP_FINISH:
			loopstatus = LOOP_START;
			start_loop_time = millis();
			programloopstat = pr_none;
			return;

		}

		break;
 
	case stat_closeandsitdone:


		programloopstat = pr_after_close;



		break;

	case pr_after_close:
		switch (do_after_close_loop()) {

		case LOOP_START:
			break;
		case LOOP_RUN:
			break;
		case  LOOP_FINISH:
			loopstatus = LOOP_START;
			start_loop_time = millis();
			programloopstat = pr_none;
			return;

		}


		break;
	default:
		loopstatus = LOOP_START;
		start_loop_time = millis();
		programloopstat = pr_none;
		break;
	}

}

void serialEventfromcmdboard() {
	// always has last status ...
#ifdef HAS_CMD

 

	while (Serial.available()) {

		// get the new byte:
		char inChar = (char)Serial.read();

		switch (inChar) {

		case 48 ... 57:

			if (other_inputvalue == NOVALUEINPUT) {
				other_inputvalue = 0;
			}
			other_inputvalue = 10 * other_inputvalue + inChar - '0';


			break;

		case'P':


			switch (other_inputvalue)
			{
			case pr_none:
				otherbooard_pr_stats = pr_none;
				break;
			case pr_before_open_loop:
				otherbooard_pr_stats = pr_before_open_loop;
				programloopstat = pr_before_open_loop;
				break;
			case pr_open:
				otherbooard_pr_stats = pr_open;
				break;
			case pr_wake_up:
				otherbooard_pr_stats = pr_wake_up;
				programloopstat = pr_wake_up;
				break;
			case pr_situp:
				otherbooard_pr_stats = pr_situp;
				programloopstat = pr_situp;
				break;
			case pr_talk:
				otherbooard_pr_stats = pr_talk;
				programloopstat = pr_talk;
				break;
			case pr_sitdown:
				otherbooard_pr_stats = pr_sitdown;
				break;
			case pr_sleep:
				otherbooard_pr_stats = pr_sleep;
 				programloopstat = pr_sleep;
				break;
			case pr_close:
				otherbooard_pr_stats = pr_close;
				break;
			case pr_after_close:
				otherbooard_pr_stats = pr_after_close;
				programloopstat = pr_after_close;
				break;
			case stat_openandsitdone:
				otherbooard_pr_stats = stat_openandsitdone;
				programloopstat = stat_openandsitdone;
				break;
			case stat_closeandsitdone:
				otherbooard_pr_stats = stat_closeandsitdone;
				programloopstat = stat_closeandsitdone;

				break;
			default:
				/// unknown 
				break;

			}


			other_inputvalue = NOVALUEINPUT;
			break;
		}


#ifdef DEBUG1
		Serial.print("NEW otherboard :  ");
		Serial.println(inChar);
#endif

		// if the incoming character is a newline, set a flag
		// so the main loop can do something about it:

	}
#endif
}

void loop()
{

	serialEventfromcmdboard(); //call the function
	ledloop();

#ifndef HAS_CMD
	// do a testing loop 
	if (programloopstat == pr_none)
		programloopstat = pr_before_open_loop;
#endif
	programloop();



	// convert to deg and speed 
	/*

	byte neckposH = lowByte(neckValsH[neckseqcount]);
	int curposH = neckservoH.read();
	//curposH = neckposH;

	byte neckposV = lowByte(neckValsV[neckseqcount]);
	int curposV = neckservoV.read();


	if ((curposH == neckposH)
	&& (curposV == neckposV)
	) {
	// next seq
	neckseqcount++;
	if (neckseqcount > totalneckseqcount)
	neckseqcount = 0;

	byte neckspeedV = highByte(neckValsV[neckseqcount]);
	byte neckspeedH = highByte(neckValsH[neckseqcount]);
	neckspeedH = 15;
	neckspeedV = 15;

	neckposH = lowByte(neckValsH[neckseqcount]);
	neckposV = lowByte(neckValsV[neckseqcount]);

	neckservoH.write(neckposH, neckspeedH, false);
	neckservoV.write(neckposV, neckspeedV, false);


	}

	*/

	/* add main program code here */

}

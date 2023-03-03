#include "main.h"
// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0

#define PORTD_OUT   PORTD
#define TRISD_OUT   TRISD

void init_output(void);
// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
void Test_KeyMatrix();

// Den giao thong
void Phase1_GreenOn();
void Phase1_GreenOff();
void Phase1_YellowOn();
void Phase1_YellowOff();
void Phase1_RedOn();
void Phase1_RedOff();

void Phase2_GreenOn();
void Phase2_GreenOff();
void Phase2_YellowOn();
void Phase2_YellowOff();
void Phase2_RedOn();
void Phase2_RedOff();

#define     INIT_SYSTEM         0
#define     PHASE1_GREEN        1
#define     PHASE1_YELLOW       2
#define     PHASE2_GREEN        3
#define     PHASE2_YELLOW       4
#define     WAIT                5
#define     ENABLE      1
#define     DISABLE     0
#define     MENU        6
#define     MENU1       7
#define     MENU2       8
#define     MENU3      29
#define     MENU4       30
#define     ENTER_PASSWORD  9
#define     WRONG_PASS       10
#define     TIMERDIS        11
#define     TRANDATA        12
#define     MENUCHANGE1     15
#define     MENUCHANGE2     16
#define     MENUCHANGE3     17
#define     CHANGEPASS1     19
#define     MENUAUTO        20
#define     MENUAUTO1           21
#define     MENUAUTO2           22
#define     MENUAUTO3           23
#define     AUTO1               24
#define     AUTO2               25
#define     AUTO3               26
#define     SETPHASE            28
#define     SETPHASEB           34
#define     MENUMANUAL          35
#define     MENUMANUAL2         36
#define     MENUMANUAL3         37
#define     SETPHASE1           38
#define     SETPHASE11          40
#define     SETPHASE2           39
#define     SETPHASE22          41
#define     CHECKPHASE1         42
#define     CHECKPHASE2         43

unsigned char statusOfLight = INIT_SYSTEM;
unsigned char timeOfGreenPhase1 = 25;
unsigned char timeOfRedPhase1 = 23;
unsigned char timeOfYellowPhase1 = 3;
unsigned char timeOfGreenPhase2 = 20;
unsigned char timeOfRedPhase2 = 28;
unsigned char timeOfYellowPhase2 = 3;

unsigned char timeOfLight = 0;
unsigned char timeOfLightRED = 0;
unsigned char cntOfLight = 0;
unsigned char cntOfLightRED =0;

void AppTrafficLight();
void UartDataReceiveProcess();
void UartDataReceiveProcess_ElectronicDeviceControl();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
unsigned char pass[3][4] = {
    {4,4,4,4},
    {5,5,5,5},
    {6,6,6,6}
};

unsigned char numberValue;

unsigned int Counter =10;
unsigned char arrayMapOfNumber [16] = {1,2,3,'A',4,5,6,'B',
                                       7,8,9,'C','*',0,'E','D'};

unsigned char arrayPassword[4];
unsigned char indexOfNumber = 0;
unsigned int setSelect=0;
unsigned int statusFSM = 0;
unsigned int statusQ1  = 0;
unsigned int passwordIndex = 0;

unsigned int bitEnableQues1 = ENABLE;
unsigned int bitEnableMENU1 = ENABLE;
unsigned int checktraffic   = 1;
unsigned char isButtonMode();
unsigned char isButtonModeHold();
unsigned char isButtonIncrease();
unsigned char isButtonDecrease();
unsigned char isButtonNumber();
unsigned char isButtonEnter();

unsigned char checkpass(int index);

void SendDataUART(void);
void CalibTime(void);
// upload UART
void UartDataReceiveProcess();
void setMENUQ1();
void setPass(int index);
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
    delay_ms(1000);
	while (1)
	{
        while (!flag_timer3);
        flag_timer3 = 0;
        scan_key_matrix_with_uart(); // 8 button
        DisplayDataReceive();
        UartDataReceiveProcess();
        
        AppTrafficLight();
        setMENUQ1();
        DisplayLcdScreenOld(); //Output process 14ms vs 10ms with no timer  
	}
}
// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_output(void)
{
   TRISD_OUT = 0x00;
    PORTD_OUT = 0x00;
}

void init_system(void)
{
    init_output();
    lcd_init();
    init_key_matrix_with_uart_i2c();
    init_interrupt();
    init_i2c();
    //init_uart();
    lcd_clear();
    LcdClearS();
    delay_ms(500);
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer3(46950);//dinh thoi 10ms
    SetTimer0_ms(2);
    SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
    //PORTAbits.RA0 = 1;
}
unsigned char isButtonMode()
{
    if (key_code[4] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonModeHold()
{
    if (key_code[4] == 10)
        return 1;
    else
        return 0;
}

unsigned char isButtonIncrease()
{
    if (key_code[5] == 1 || (key_code[5] > 10 && key_code[5]%2 == 1))
        return 1;
    else
        return 0;
}

unsigned char isButtonDecrease()
{
    if (key_code[6] == 1 || (key_code[6] > 10 && key_code[6]%2 == 1))
        return 1;
    else
        return 0;
}

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT = PORTD_OUT | arrayMapOfOutput[index];
	}


}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT = PORTD_OUT & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON)
    {
        CloseOutput(index);
        statusOutput[index] = OFF;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void TestOutput(void)
{
	int k;
	for (k=0;k<14 ;k++ )
	{
		OpenOutput(k);
		delay_ms(500);
		CloseOutput(k);
		delay_ms(500);
	}
}

void Phase1_GreenOn()
{
    OpenOutput(0);
}
void Phase1_GreenOff()
{
    CloseOutput(0);
}

void Phase1_YellowOn()
{
    OpenOutput(4);
}
void Phase1_YellowOff()
{
    CloseOutput(4);
}

void Phase1_RedOn()
{
    OpenOutput(6);
}
void Phase1_RedOff()
{
    CloseOutput(6);
}

void Phase2_GreenOn()
{
    OpenOutput(1);
}
void Phase2_GreenOff()
{
    CloseOutput(1);
}

void Phase2_YellowOn()
{
    OpenOutput(5);
}
void Phase2_YellowOff()
{
    CloseOutput(5);
}

void Phase2_RedOn()
{
    OpenOutput(7);
}
void Phase2_RedOff()
{
    CloseOutput(7);
}

void AppTrafficLight()
{
    cntOfLight = (cntOfLight + 1)%20;
    cntOfLightRED =(cntOfLight+1)%20;
    if (cntOfLight == 0)
    { 
        timeOfLight --;
    }
    if (cntOfLightRED == 0)
    {   timeOfLightRED--;
    }
    switch (statusOfLight)
    {
        case INIT_SYSTEM:
            if(bitEnableQues1==ENABLE && checktraffic == 1)
            {
            statusOfLight = PHASE1_GREEN;
            }
            break;
        case PHASE1_GREEN:
            LcdClearS();
            bitEnableQues1 = DISABLE;
            Phase1_GreenOn();
            Phase1_RedOff();
            Phase2_RedOn();
            LcdPrintStringS(0,0,"PHASE2_RED:   ");
            LcdPrintNumS(0,14,timeOfLightRED);
            LcdPrintStringS(1,0,"PHASE1_GREEN:   ");
            LcdPrintNumS(1,14,timeOfLight);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE1_YELLOW;
                Phase1_GreenOff();
                timeOfLight = timeOfYellowPhase1;
            }
            break;
        case PHASE1_YELLOW:
            LcdClearS();
            bitEnableQues1 = DISABLE;
            Phase1_YellowOn();
            Phase2_RedOn();
            LcdPrintStringS(0,0,"PHASE2_RED:  ");
            LcdPrintNumS(0,14,timeOfLightRED);
            LcdPrintStringS(1,0,"PHASE1_YELLOW:  ");
            LcdPrintNumS(1,14,timeOfLight);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE2_GREEN;
                Phase1_YellowOff();
                timeOfLight = timeOfGreenPhase2;
                timeOfLightRED = timeOfRedPhase1;
            }
            break;
        case PHASE2_GREEN:
            LcdClearS();
            bitEnableQues1 = DISABLE;
            Phase2_GreenOn();
            Phase2_RedOff();
            Phase1_RedOn();
            LcdPrintStringS(0,0,"PHASE1_RED:   ");
            LcdPrintNumS(0,14,timeOfLightRED);
            LcdPrintStringS(1,0,"PHASE2_GREEN:   ");
            LcdPrintNumS(1,14,timeOfLight);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE2_YELLOW;
                Phase2_GreenOff();
                timeOfLight = timeOfYellowPhase2;
            }
            break;
        case PHASE2_YELLOW:
            LcdClearS();
            bitEnableQues1 = DISABLE;
            Phase2_YellowOn();
            Phase1_RedOn();
            LcdPrintStringS(0,0,"PHASE1_RED:  ");
            LcdPrintNumS(0,14,timeOfLightRED);
            LcdPrintStringS(1,0,"PHASE2_YELLOW:  ");
            LcdPrintNumS(1,14,timeOfLight);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE1_GREEN;
                Phase2_YellowOff();
                timeOfLightRED = timeOfRedPhase2;
                timeOfLight = timeOfGreenPhase1;
            }
            break;
        case WAIT:
            LcdClearS();
            bitEnableQues1 = DISABLE;
            Phase1_GreenOff();
            Phase2_GreenOff();
            Phase1_RedOff();
            Phase2_RedOff();
            if (cntOfLight<=12)
            {
                Phase1_YellowOn();
                Phase2_YellowOn();
            }
            else
            {
                Phase1_YellowOff();
                Phase2_YellowOff();
            }
            break;        
        default:
            LcdClearS();
            bitEnableQues1 = ENABLE;
            statusOfLight = PHASE1_GREEN;
            break;
    }
}

void UartDataReceiveProcess()
{
    if(flagOfDataReceiveComplete == 1)
    {
        flagOfDataReceiveComplete = 0;
        if (dataReceive[4] == 0)
        {
            timeOfGreenPhase1 = dataReceive[0];
            timeOfYellowPhase1 = dataReceive[1];
            timeOfGreenPhase2 = dataReceive[2];
            timeOfYellowPhase2 = dataReceive[3];
        }
    }
    LcdPrintNumS(0,15,statusReceive);
}
void setMENUQ1()
{
//    if(bitEnableMENU1 == 0) return;   
    switch(statusQ1)
    {
    case INIT_SYSTEM:           
        if(isButtonMode()==1 && bitEnableMENU1==ENABLE){
            LcdClearS();
            statusQ1 = MENU;
            bitEnableQues1 = DISABLE;
            checktraffic =0;
        }
            break;
        case MENU:
            LcdClearS();
             bitEnableQues1 = DISABLE;
            checktraffic =0;
            LcdPrintStringS(0,7,"MENU");
            LcdPrintStringS(1,0,">");
            LcdPrintStringS(1,2,"1.AUTO MODE");
            if(isButtonMode())
            {   passwordIndex=1;
                statusQ1 = MENUAUTO;
                 bitEnableQues1 = DISABLE;
                checktraffic =0;
                indexOfNumber= 0;
                LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            statusOfLight = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            checktraffic =1;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENU3;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENU2;
                break;
            }      
            break;
        case MENU2:
            LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,0,">");
            LcdPrintStringS(0,2,"2.MANUAL MODE");
            LcdPrintStringS(1,2,"3.SPECIAL MODE");
            if(isButtonMode())
            {   passwordIndex=2;
                statusQ1 = SETPHASE;
                bitEnableQues1= DISABLE;
                indexOfNumber= 0;
            LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            statusOfLight = INIT_SYSTEM;
            bitEnableQues1 = ENABLE;
            checktraffic =1;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENU;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENU3;
                break;
            }      
            break;
        case MENU3:
            LcdClearS();
            bitEnableQues1= DISABLE;
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,2,"2.MANUAL MODE");
            LcdPrintStringS(1,0,">");
            LcdPrintStringS(1,2,"3.SPECIAL MODE");
            if(isButtonMode())
            {   passwordIndex=3;
                statusQ1 = ENTER_PASSWORD;
                 bitEnableQues1 = DISABLE;
                checktraffic =0;
                indexOfNumber= 0;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusOfLight = INIT_SYSTEM;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            checktraffic =1;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENU2;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENU4;
                break;
            }      
            break;
        case MENU4:
            LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,2,"3.SPECIAL MODE");
            LcdPrintStringS(1,0,">");
            LcdPrintStringS(1,2,"4.CHANGE PASS");
            if(isButtonMode())
            {   passwordIndex=4;
                statusQ1 = ENTER_PASSWORD;
                bitEnableQues1= DISABLE;
                checktraffic =0;
                indexOfNumber= 0;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusOfLight = INIT_SYSTEM;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            checktraffic =1;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENU3;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENU;
                break;
            }      
            break;
        case MENUAUTO:
            LcdClearS();
            bitEnableQues1= DISABLE;
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,4,"MENU AUTO");
            LcdPrintStringS(1,0,">");
            LcdPrintStringS(1,2,"1.AUTO 1");
            if(isButtonMode())
            {
                statusQ1 = AUTO1;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENUAUTO3;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENUAUTO2;
                break;
            }      
            break;
        case MENUAUTO2:
            LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,2,"2.AUTO 2");
            LcdPrintStringS(0,0,">");
            LcdPrintStringS(1,2,"3.AUTO 3");
            if(isButtonMode())
            {
                statusQ1 = AUTO2;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENUAUTO;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENUAUTO3;
                break;
            }      
            break;    
         case MENUAUTO3:
            LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,2,"2.AUTO 2");
            LcdPrintStringS(1,0,">");
            LcdPrintStringS(1,2,"3.AUTO 3");
            if(isButtonMode())
            {
                statusQ1 = AUTO3;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENUAUTO2;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENUAUTO;
                break;
            }      
            break;   
        case AUTO1:
            LcdClearS();
            LcdPrintStringS(0,2,"SET AUTO1");
            timeOfGreenPhase1 = 10;
            timeOfRedPhase2 =13;
            timeOfGreenPhase2 = 13;
            timeOfRedPhase1 = 16;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
        case AUTO2:
             LcdClearS();
            LcdPrintStringS(0,2,"SET AUTO2");
            timeOfGreenPhase1 = 30;
            timeOfRedPhase2 =35;
            timeOfYellowPhase1=5;
            timeOfGreenPhase2 = 30;
            timeOfRedPhase1 = 35;
            timeOfYellowPhase1=5;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
       case AUTO3:
             LcdClearS();
            LcdPrintStringS(0,2,"SET AUTO3");
            timeOfGreenPhase1 = 96;
            timeOfRedPhase2 =99;
            timeOfYellowPhase1=3;
            timeOfGreenPhase2 = 96;
            timeOfRedPhase1 = 99;
            timeOfYellowPhase1=3;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
        case SETPHASE:
             LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,2,"1.SET PHASE 1");
            LcdPrintStringS(0,0,">");
            LcdPrintStringS(1,2,"2.SET PHASE 2");
            if(isButtonMode())
            {
                statusQ1 = SETPHASE1;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = SETPHASEB;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = SETPHASEB;
                break;
            }      
            break; 
        case SETPHASEB:
             LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,2,"1.SET PHASE 1");
            LcdPrintStringS(1,0,">");
            LcdPrintStringS(1,2,"2.SET PHASE 2");
            if(isButtonMode())
            {
                statusQ1 = SETPHASE2;
                       LcdClearS();
                         break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = SETPHASE;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = SETPHASE;
                break;
            }      
            break;
        case SETPHASE1:
            LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,0,"PHASE2_RED:   ");
            LcdPrintNumS(0,14,timeOfRedPhase2);
            LcdPrintStringS(1,0,"PHASE1_GREEN:   ");
            LcdPrintNumS(1,14,timeOfGreenPhase1);
            if(isButtonMode())
            {
                statusQ1 = SETPHASE11;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   
                timeOfRedPhase2++;
                statusQ1=SETPHASE1;
                break;
            }
            if(isButtonDecrease())
            {   timeOfRedPhase2--;
                statusQ1 = SETPHASE1;
                break;
            }      
            break; 
        case SETPHASE11:
            LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,0,"PHASE2_RED:   ");
            LcdPrintNumS(0,14,timeOfRedPhase2);
            LcdPrintStringS(1,0,"PHASE1_GREEN:   ");
            LcdPrintNumS(1,14,timeOfGreenPhase1);
            if(isButtonMode())
            {
                statusQ1 = CHECKPHASE1;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   
                timeOfGreenPhase1++;
                statusQ1=SETPHASE11;
                break;
            }
            if(isButtonDecrease())
            {   timeOfGreenPhase1--;
                statusQ1 = SETPHASE11;
                break;
            }      
            break; 
        case CHECKPHASE1:
            LcdClearS();
            if(timeOfRedPhase2-timeOfGreenPhase1<=0)
            {   LcdClearS();
                
                LcdPrintStringS(0,3,'ERROR TIME');
                LcdPrintStringS(1,2,"YELLOW LIGHT");
                delay_ms(1000);
                statusQ1 = AUTO1;
            }
            else
            {   timeOfYellowPhase1 = timeOfRedPhase2-timeOfGreenPhase1;
             statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            }
           
            break;
         case SETPHASE2:
            LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,0,"PHASE1_RED:   ");
            LcdPrintNumS(0,14,timeOfRedPhase1);
            LcdPrintStringS(1,0,"PHASE2_GREEN:   ");
            LcdPrintNumS(1,14,timeOfGreenPhase2);
            if(isButtonMode())
            {
                statusQ1 = SETPHASE22;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   
                timeOfRedPhase1++;
                statusQ1=SETPHASE2;
                break;
            }
            if(isButtonDecrease())
            {   timeOfRedPhase1--;
                statusQ1 = SETPHASE2;
                break;
            }      
            break; 
        case SETPHASE22:
            LcdClearS();
//            bitEnableMENU1=DISABLE;
            LcdPrintStringS(0,0,"PHASE1_RED:   ");
            LcdPrintNumS(0,14,timeOfRedPhase1);
            LcdPrintStringS(1,0,"PHASE2_GREEN:   ");
            LcdPrintNumS(1,14,timeOfGreenPhase2);
            if(isButtonMode())
            {
                statusQ1 = CHECKPHASE2;
                       LcdClearS();
            break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   
                timeOfGreenPhase2++;
                statusQ1=SETPHASE22;
                break;
            }
            if(isButtonDecrease())
            {   timeOfGreenPhase2--;
                statusQ1 = SETPHASE22;
                break;
            }      
            break; 
        case CHECKPHASE2:
            LcdClearS();
            if(timeOfRedPhase1-timeOfGreenPhase2<=0)
            {   LcdClearS();
                
                LcdPrintStringS(0,3,'ERROR TIME');
                LcdPrintStringS(1,2,"YELLOW LIGHT");
                delay_ms(1000);
                statusQ1 = AUTO2;
            }
            else
            {   timeOfYellowPhase2 = timeOfRedPhase1-timeOfGreenPhase2;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            }
            
            break;
        case ENTER_PASSWORD:
//            LcdClearS();
//            bitEnableQues1=DISABLE;
            LcdPrintStringS(0,0,"ENTER PASSWORD  ");
//            timeDelay ++;

            if (indexOfNumber >= 4){
                            if (isButtonEnter()){
                                indexOfNumber = 0;
                                if(checkpass(0)==1 && passwordIndex==1){
                                    statusQ1 = MENUAUTO;
                                    LcdClearS();
                                    break;
                                } 
                                if(checkpass(1)==1 && passwordIndex==2){
                                    Counter=10;
                                    SetTimer0_ms(1000);
                                    statusQ1 = TRANDATA;
                                    break;
                                } 
                                if(checkpass(2)==1 && passwordIndex==3){
                                    statusQ1 = MENUCHANGE1;
                                    break;
                                }
//                                if(checkpass(3)==1 && passwordIndex ==4)
//                                {   statusQ1 = MENUCHANGE1;
//                                    break;
//                                }
                                else{
                                    statusQ1 = WRONG_PASS;
                                    SetTimer0_ms(1000);
                                    LcdClearS();
                                    break;
                                }
                            }
                
            } else {
                            if (isButtonNumber())
            {
                LcdPrintStringS(1,indexOfNumber,"*");
                arrayPassword [indexOfNumber] = numberValue;
                indexOfNumber ++;
//                timeDelay = 0;
            }
            }
                
//            if (timeDelay >= 100)
//                statusPassword = INIT_SYSTEM;

            break;
        case WRONG_PASS:
            LcdClearS();
            
            LcdPrintStringS(0,1,"WRONG PASSWORD");
            if(flag_timer0){
                LcdClearS();
                flag_timer0 = 0;
                statusOfLight = INIT_SYSTEM;
                statusQ1 = INIT_SYSTEM;
                bitEnableQues1=ENABLE;
            }
            break;
        case MENUCHANGE1:
            LcdClearS();
//            bitEnableMENU1 = ENABLE;
            LcdPrintStringS(1,0,">");
            LcdPrintStringS(0,2,"CHANGE PASS");
            LcdPrintStringS(1,2,"1.PASS AUTO");
           if(isButtonMode())
            {   
                statusQ1 = CHANGEPASS1;
                   LcdClearS();
                   indexOfNumber = 0;
                   passwordIndex = 1;
                        break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusFSM = INIT_SYSTEM;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENUCHANGE3;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENUCHANGE2;
                break;
            }      
            break;
        case MENUCHANGE2:
            LcdClearS();
//            bitEnableMENU1 = ENABLE;
            LcdPrintStringS(0,0,">");
            LcdPrintStringS(0,2,"2.PASS MANUAL");
            LcdPrintStringS(1,2,"3.PASS SPECIAL");
           if(isButtonMode())
            {   
                statusQ1 = CHANGEPASS1;
                indexOfNumber = 0;
                LcdClearS();
                passwordIndex = 2;
                break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusFSM = INIT_SYSTEM;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENUCHANGE1;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENUCHANGE3;
                break;
            }      
            break;
        case MENUCHANGE3:
            LcdClearS();
            bitEnableMENU1 = ENABLE;
            LcdPrintStringS(1,0,">");
            LcdPrintStringS(0,2,"2.PASS MANUAL");
            LcdPrintStringS(1,2,"3.PASS SPECIAL");
           if(isButtonMode())
            {   
                statusQ1 = CHANGEPASS1;
                indexOfNumber = 0;
                LcdClearS();
                passwordIndex = 3;
                break;
            }
            if(isButtonModeHold())
            {  LcdClearS();
            statusFSM = INIT_SYSTEM;
            statusQ1 = INIT_SYSTEM;
            bitEnableMENU1 = ENABLE;
            bitEnableQues1 = ENABLE;
            break;
            }
            if(isButtonIncrease())
            {   statusQ1 = MENUCHANGE2;
                break;
            }
            if(isButtonDecrease())
            {   statusQ1 = MENUCHANGE1;
                break;
            }      
            break;
        case CHANGEPASS1:

            LcdPrintStringS(0,0,"ENTER NEW PASS ");
            if (indexOfNumber >= 4){
                            if (isButtonEnter()){
                                setPass(passwordIndex-1);
                                LcdClearS();
                                statusFSM = INIT_SYSTEM;
                                statusQ1 = INIT_SYSTEM;
                                bitEnableMENU1 = ENABLE;
                                bitEnableQues1 = ENABLE;
                            }
                
            } else {
                            if (isButtonNumber())
            {
                LcdPrintNumS(1,indexOfNumber,numberValue);
                arrayPassword [indexOfNumber] = numberValue;
                indexOfNumber ++;
//                timeDelay = 0;
            }
            }
                
//            if (timeDelay >= 100)
//                statusPassword = INIT_SYSTEM;

            break;
    }
}

unsigned char isButtonNumber()
{
    unsigned char i;
    for (i = 0; i<=15; i++)
        if (key_code[i] == 1)
        {
            numberValue = arrayMapOfNumber[i];
            return 1;
        }
    return 0;
}

unsigned char isButtonEnter()
{
    if (key_code[4] == 1)
        return 1;
    else
        return 0;
}

unsigned char checkpass(int index){
    int i;
    for(i = 0; i <4; i++){
        if(arrayPassword[i] != pass[index][i]) return 0;
    }
    return 1;
}

void setPass(int index){
    int i;
    for(i = 0; i < 4; i++){
        pass[index][i] = arrayPassword[i];
    }
}



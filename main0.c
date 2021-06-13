#include"stdint.h"
#include "math.h"
#include "Define.h" // reallocate the path for Define. if necessary

void SystemInit(){}

//delay
 void delay(void){
  unsigned long volatile time;
  time = 145448;  // 0.1sec
  while(time){
 		time--; }
	}


#define RED 0x02 
#define BLUE 0x04
#define GREEN 0x08
	

	
	//Port F configuration	
void init_F(void){
      uint32_t delay ;
      SYSCTL_RCGCGPIO_R |= 0x00000020 ;
      delay = 1;
		  GPIO_PORTF_LOCK_R =0x4c4f434B;
	    GPIO_PORTF_CR_R =0x1F;
      GPIO_PORTF_AFSEL_R = 0 ;
      GPIO_PORTF_AMSEL_R = 0 ;
      GPIO_PORTF_DEN_R = 0x1F ;
      GPIO_PORTF_DIR_R = 0x0E ;
      GPIO_PORTF_PCTL_R = 0 ;   
      GPIO_PORTF_PUR_R =0x11;
}

void LCD_Command(char cmd)
{
      GPIO_PORTA_DATA_R=0; // put control signals (RS-->5th , RW-->6th , E-->7th) , RS=0 to command
      GPIO_PORTB_DATA_R=cmd; // Put the Command in Port B
      
        GPIO_PORTA_DATA_R|=0x80; // Put 1 to 7th ,, E-->7
        delay(3);
        GPIO_PORTA_DATA_R|=0; // Put 0 to 7th ,, E-->0
}

void LCD_Data(char data)
{
      GPIO_PORTA_DATA_R=0x20; // Put 1 to 5th ,, RS=1 , RW=0(to write) , E=0
      GPIO_PORTB_DATA_R=data; // Put the Command in Port B
      
        GPIO_PORTA_DATA_R|=0x80; // Put 1 to 7th ,, E=1
        delay(3);
        GPIO_PORTA_DATA_R|=0; // Put 0 to 7th ,, E=0
}
		
	//LCD Configuration
void LCD_init(void){
      uint32_t delay ;
      SYSCTL_RCGCGPIO_R |= 0x00000003 ; //for Port A and Port B
      delay = 1;
          GPIO_PORTB_LOCK_R =0x4c4f434B;
        GPIO_PORTB_CR_R =0xFF;
      GPIO_PORTB_AFSEL_R = 0 ;
      GPIO_PORTB_AMSEL_R = 0 ;
      GPIO_PORTB_DEN_R = 0xFF ;// we need all bits for lcd output
      GPIO_PORTB_DIR_R = 0xFF ;
      GPIO_PORTB_PCTL_R = 0 ;   
      GPIO_PORTB_PUR_R =0x11;
    
      delay = 1;
          GPIO_PORTA_LOCK_R =0x4c4f434B;
        GPIO_PORTA_CR_R =0xFF;
      GPIO_PORTA_AFSEL_R = 0 ;
      GPIO_PORTA_AMSEL_R = 0 ;
      GPIO_PORTA_DEN_R = 0xE0 ; // we only need the last 3 bits for control
      GPIO_PORTA_DIR_R = 0xE0 ;
      GPIO_PORTA_PCTL_R = 0 ;   
      GPIO_PORTA_PUR_R =0xE0; 
            
            LCD_Command(0x30); //wakeup
            LCD_Command(0x38); //to make the lcd work on 8 bits
            LCD_Command(0x06); //increment crussor to the right
            LCD_Command(0x01); //to clear the screen
            LCD_Command(0x0F); //to turn the screen on
            
}
}

void clear()
{
                LCD_Command(0x01);//clear
	
                LCD_Command(0x80); //force crussor to begin in the first line
	              LCD_Command(0x02);
}

 

int gt100(int distance)
{
    if(distance>=100)return 1;
    else return 0;
}

 

double calc_distance(double x1,double x2,double y1,double y2)
{
    return sqrt((x2-x1)*(x2-x1) +(y2-y1)*(y2-y1));
}

void UART_Init(){
    SYSCTL_RCGCUART_R |= 0x00000002 ;  //UART1
    SYSCTL_RCGCGPIO_R |= 0x00000004 ;  // port c 
	 
    UART1_CTL_R &= ~0x00000001 ;       // disable UART
    UART1_IBRD_R =  104;                     //int(80000000/(16 * 9600))       
    UART1_FBRD_R = 11 ; 
    UART1_LCRH_R = 0x00000070 ; // no parity , one stop
    UART1_CTL_R |= 0x00000301 ;       // enable UART
	 
    GPIO_PORTC_AFSEL_R = 0x30  ;      // pc4 ,pc5 
    GPIO_PORTC_DEN_R = 0x30  ;        // pc4 ,pc5 
    GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & 0xff00ffff )+ 0x00220000;     // pc4 -> u1Rx   pc5 -> u1Tx    
    GPIO_PORTC_AMSEL_R = ~0x30 ;    
}

int main()
{
    double total_distance= 0 ;
    double distance = calc_distance(80,10,130,40);// data to check calc_distance function. 
    int i;
    char data[]="distance is 100m";
    init_F();
    LCD_init();
	  
    while(1)
    {
	  for( i=0; i<10;i++)
	        {
	        delay();
         	}		 
			distance = calc_distance(5,2,10,4); // dummy data instead of GPS coordinates.
			total_distance+= distance;
	        if(gt100(total_distance)) 
		{ 
                     GPIO_PORTF_DATA_R = RED;  // Red led is on when the distance exceeds 100m.
		     clear() ;//clear and start from the beginning
   	              char_to_LCD(data);	// dummy numbers to check the function calculating distance
		       break; // exit while loop.
		}
		else{
                    GPIO_PORTF_DATA_R = 0x11;
		}
                 
                
    }
}
 

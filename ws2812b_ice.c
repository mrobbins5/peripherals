#include "ws2812b.h"


/*******************************************************************************
* Function Name: rotate_ws2812bs
********************************************************************************
* Summary:
*
*   Rotates the WS2812B structs are rotates in the array by one position.  This
*   can be done by copying each WS2812B stuct to the next highest index in the 
*   array.  The last WS2812B struct should be moved to base[0]
*
*
* Parameters:
*   base         Base address of the WS2812B_t array
*   num_leds     Size number WS2812B_t structs in the array
*
* Return:
*  void
*
*******************************************************************************/
void ws2812b_rotate(WS2812B_t *base,uint8_t num_leds)
{
	int i; 
	for(i = num_leds - 2; i >= 0; i--)
	{
			base[i+1].green = base[i].green; \
			base[i+1].red = base[i].red; 
			base[i+1].blue = base[i].blue; 
	}

	
	base[0].green = base[num_leds - 1].green;
	base[0].red = base[num_leds - 1].red;
	base[0].blue = base[num_leds - 1].blue;

	
	return; 
}

/*******************************************************************************
* Function Name: pulse_ws2812b
********************************************************************************
* Summary:
*
*   Pulses the RED LED for a each WS2812B LED in a series of WS2812B LEDs.
*
*   The function will contain a static variable named direction that indicates 
*   if the value RED color codes are currently decrementing or incrementing.  
*
*   If the direction is incrementing AND the value of the red field is less than
*   0xFF, the function will increment the red filed by 1 for each structure in 
*   the array.  
*
*   If the direction is incrementing and the value of the red filed is equal to
*   0xFF, the function will change the direction to be counting down and decrement
*   the red filed by 1 for each structure in the array.  
*
*   If the direction is decrementing AND the value of the red field is greater than
*   0x00, the function will decrement the red filed by 1 for each structure in 
*   the array.  
*
*   If the direction is decrementing and the value of the red filed is equal to
*   0x00, the function will change the direction to be counting up and increment
*   the red filed by 1 for each structure in the array. 
*
* Parameters:
*   base         Base address of the WS2812B_t array
*   num_leds     Size number WS2812B_t structs in the array
*
* Return:
*  void
*
*******************************************************************************/
void ws2812b_pulse( 
  WS2812B_t *base,
  uint8_t num_leds
)
{
	
	//1 for incrementing, 0 for decrementing
	int direction = 0; 	
	int i; 
	int j; 
	
	for(i = 0; i < 8; i++){
		
			if(direction && base[i].red < 0xFF){
				//increment the red filed by 1 for each structure in array 
				for(j = 0; j < 8; j++){
					base[j].red = base[j].red + 1; 
				}
 
			}
			if(direction && base[i].red == 0xFF){
				//change the direction to be counting down and decrement
				// the red filed by 1 for each structure in the array.  
				direction = 0; 
				for(j = 0; j < 8; j++){
					base[j].red = base[j].red - 1; 
				}
			}
			if(!direction && base[i].red > 0x00){
				//the function will decrement the red filed by 1 for each structure in 
				//the array.  
				for(j = 0; j < 8; j++){
					base[j].red = base[j].red - 1; 
				}
			}
			if(!direction && base[i].red == 0x00){
				
			//	function will change the direction to be counting up and increment
			// the red filed by 1 for each structure in the array. 
				direction = 1; 
				for(j = 0; j < 8; j++){
					base[j].red = base[j].red + 1; 
				}
			}
	}

	
	
	
}
	
	
	

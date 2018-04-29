#include "io_expander.h"
//*****************************************************************************
// Used to determine if io expander is busy writing the last transaction
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
// Returns
// I2C_OK is returned one the ip expander is ready to write the next byte
//*****************************************************************************
static i2c_status_t io_expander_wait_for_write(int32_t i2c_base)
{
  
  i2c_status_t status;
  
  if( !i2cVerifyBaseAddr(i2c_base) )
  {
    return  I2C_INVALID_BASE;
  }

  // set I2C address to slave and in Write Mode
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);

  // Poll while device is busy.  The  MCP24LC32AT will not ACK
  // writing an address while the write has not finished.
  do 
  {
    // byte sent doesn't matter
    status = i2cSendByte(i2c_base, 0xFF, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
    
    // Wait for address to finish transmitting
    while ( I2CMasterBusy(i2c_base)) {};
    
    // If the address was not ACKed, try again.
  } while (I2CMasterAdrAck(i2c_base) == false);

  return  status;
} 

//*****************************************************************************
// Writes a single byte of data out to the MCP24LC32AT MCP.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being written.  Only the lower
//                12 bits is used by the MCP
//
//    data:       Data written to the MCP.
//
// Returns
// status if the byte was written to the MCP.
//*****************************************************************************
static i2c_status_t io_expander_byte_write(uint32_t  i2c_base, uint8_t  address, uint8_t data)
{
  i2c_status_t status;
    
  // check that I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};
  
  // Set I2C address to be MCP23017
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
	
	if (status != I2C_OK) {
		return status;
	}
		
  // If the EEPROM is still writing the last byte written, wait
  io_expander_wait_for_write(i2c_base);
  
  // Send the address
  // TODO maybe should be upper and lower bytes of address, not sure if necessary
  i2cSendByte(i2c_base, address, I2C_MCS_START | I2C_MCS_RUN);
		
  // Send the data		
  i2cSendByte(i2c_base, data, I2C_MCS_RUN | I2C_MCS_STOP);
	
  return status;
}

//*****************************************************************************
// Reads one byte of data from the current address of the MCP23017  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being read.  Only the lower
//                12 bits is used by the MCP
//
//    data:       data read from the MCP is returned to a uint8_t pointer.
//
// Returns
// I2C_OK if the byte was read from the MCP.
//*****************************************************************************
static i2c_status_t io_expander_byte_read(uint32_t  i2c_base, uint8_t	address, uint8_t *data)
{
  i2c_status_t status;
  
  // check that I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  // If the io expander is still writing the last byte written, wait
  io_expander_wait_for_write(i2c_base);		
		
  // Set the I2C slave address to be MCP23017 and in write mode
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
	
  if (status != I2C_OK) {
	return status;
  }
  
  // Send the address
  i2cSendByte(i2c_base, address, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);		

  // Set the I2C slave address to be the MCP23017 and in Read Mode
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_READ);
		
  // Read the data returned by the MCP23017
  i2cGetByte(i2c_base, data , I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);

  return I2C_OK;
}


//*****************************************************************************
// Read the value of the up/down and left/right buttons
//*****************************************************************************
uint8_t io_expander_read_buttons(void)
{
	uint8_t buttons;
	
	io_expander_byte_read(IO_EXPANDER_I2C_BASE, MCP23017_GPIOB_R, &buttons);
	
	return buttons & 0x0F;
}

//*****************************************************************************
// Initialize the GPIO Expander and I2C 
//*****************************************************************************
bool io_expander_init(void)
{ 
  // Configure I2C GPIO Pins on TI Board  
  if (gpio_enable_port(IO_EXPANDER_GPIO_BASE) == false) {
		return false;
	}
  
  // Configure SCL 
  if (gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN) == false) {
			return false;
	}
  if (gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN) == false) {
		return false; 
	}
	if (gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PCTL_M, IO_EXPANDER_I2C_SCL_PIN_PCTL) == false) {
		return false;
	}

  
  // Configure SDA 
  if (gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN) == false) {
		return false;
	}
  if (gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN) == false) {
		return false;
	}
  if (gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN) == false) {
		return false;
	}
  if (gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL) == false) {
		return false;
  }
	
  //  Initialize I2C peripheral
  if (initializeI2CMaster(IO_EXPANDER_I2C_BASE) != I2C_OK) {
		return false;
	}
	
	// Initialize GPIO pins on MCP23017
	
	// set GPIOA on expander to be output
	io_expander_byte_write(IO_EXPANDER_I2C_BASE, MCP23017_IODIRA_R, 0x00);
	
	// set GPIOB on expander to be input, enable PUR, and enable polarity
	
	//sets the direction to all inputs
	io_expander_byte_write(IO_EXPANDER_I2C_BASE, MCP23017_IODIRB_R, 0xFF);
	
	
	//??
	io_expander_byte_write(IO_EXPANDER_I2C_BASE, MCP23017_GPPUB_R, 0xFF);
	
	
	//polarity?
	io_expander_byte_write(IO_EXPANDER_I2C_BASE, MCP23017_IPOLB_R, 0xFF);
	
	
	//Set the buttons
	
	
	
	//NVIC_SetPriority(I2C1_IRQn, 1);
		
  // Enable the Interrupt in the NVIC
  //NVIC_EnableIRQ(I2C1_IRQn);
	
	
  return true;
  
}

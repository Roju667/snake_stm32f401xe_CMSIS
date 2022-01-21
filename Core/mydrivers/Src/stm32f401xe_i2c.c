/*
 * stm32f401xe_I2C.c
 *
 *  Created on: 12 sty 2022
 *      Author: pawel
 */

#include "stm32f401xe_gpio.h""
#include "stm32f401xe_i2c.h"
#include "stm32f401xe_rcc.h"

/*
 * Start clock for GPIO
 *
 * @param[*p_i2cx] - base address of i2c peripheral
 * @return - void
 */
static void i2c_clock_enable(I2C_TypeDef *p_i2cx)
{
	if (p_i2cx == I2C1)
	{
		RCC_CLOCK_I2C1_ENABLE();
	}
	else if (p_i2cx == I2C2)
	{
		RCC_CLOCK_I2C2_ENABLE();
	}
	else if (p_i2cx == I2C3)
	{
		RCC_CLOCK_I2C3_ENABLE();
	}
}

/*
 * init i2c peripheral gpio pins
 *
 * @param[*p_i2cx] - i2c address
 * @param[alternate_pos] - pins alternative positions select
 * @return - void
 */
static void i2c_init_gpio_pins(I2C_TypeDef *p_i2cx, uint8_t alternate_pos)
{
	GPIO_Handle_t gpio_sda, gpio_scl;

	if (p_i2cx == I2C1)
	{
		// PB6 SCL
		gpio_scl.pGPIOx = GPIOB;
		gpio_scl.PinConfig.PinNumber = GPIO_PIN_6;

		// PB7 SDA
		gpio_sda.pGPIOx = GPIOB;
		gpio_sda.PinConfig.PinNumber = GPIO_PIN_7;

		// Alternate Function
		gpio_scl.PinConfig.AF = GPIO_PIN_AF_AF4;
		gpio_sda.PinConfig.AF = GPIO_PIN_AF_AF4;

		if (alternate_pos == 1)
		{
			// PB8 SCL
			gpio_scl.PinConfig.PinNumber = GPIO_PIN_8;
			// PB9 SDA
			gpio_sda.PinConfig.PinNumber = GPIO_PIN_9;
		}

	}

	if (p_i2cx == I2C2)
	{

		// PB10 SCL
		gpio_scl.pGPIOx = GPIOB;
		gpio_scl.PinConfig.PinNumber = GPIO_PIN_10;

		// PB11 SDA
		gpio_sda.pGPIOx = GPIOB;
		gpio_sda.PinConfig.PinNumber = GPIO_PIN_11;

		// Alternate Function
		gpio_scl.PinConfig.AF = GPIO_PIN_AF_AF4;
		gpio_sda.PinConfig.AF = GPIO_PIN_AF_AF4;

		if (alternate_pos == 1)
		{
			// PB3 SDA
			gpio_sda.PinConfig.PinNumber = GPIO_PIN_3;
			gpio_sda.PinConfig.AF = GPIO_PIN_AF_AF9;
		}

	}

	if (p_i2cx == I2C3)
	{

		// PA8 SCL
		gpio_scl.pGPIOx = GPIOA;
		gpio_scl.PinConfig.PinNumber = GPIO_PIN_8;

		// PB9 SDA
		gpio_sda.pGPIOx = GPIOB;
		gpio_sda.PinConfig.PinNumber = GPIO_PIN_4;

		// Alternate Function
		gpio_scl.PinConfig.AF = GPIO_PIN_AF_AF4;
		gpio_sda.PinConfig.AF = GPIO_PIN_AF_AF4;

		if (alternate_pos == 1)
		{
			RCC_CLOCK_GPIOC_ENABLE();
			// PC9 SDA
			gpio_sda.pGPIOx = GPIOC;
			gpio_sda.PinConfig.PinNumber = GPIO_PIN_9;

			// Alternate Function
			gpio_sda.PinConfig.AF = GPIO_PIN_AF_AF9;
		}

	}

	// Mode AF
	gpio_scl.PinConfig.Mode = GPIO_PIN_MODE_AF;
	gpio_sda.PinConfig.Mode = GPIO_PIN_MODE_AF;

	// Output type open drain
	gpio_scl.PinConfig.OutputType = GPIO_PIN_OT_OD;
	gpio_sda.PinConfig.OutputType = GPIO_PIN_OT_OD;

	// Output speed very high
	gpio_scl.PinConfig.OutputSpeed = GPIO_PIN_SPEED_VERYHIGH;
	gpio_sda.PinConfig.OutputSpeed = GPIO_PIN_SPEED_VERYHIGH;

	// Pull ups
	gpio_scl.PinConfig.PullUpPullDown = GPIO_PIN_PUPD_NOPULL;
	gpio_sda.PinConfig.PullUpPullDown = GPIO_PIN_PUPD_NOPULL;

	GPIO_InitPin(&gpio_sda);
	GPIO_InitPin(&gpio_scl);

}

/*
 * Init I2C peripheral
 *
 * @param[*p_handle_i2c] - handler to i2c structure
 * @return - void
 */
uint8_t i2c_init(i2c_handle_t *p_handle_i2c)
{
	// enable peripheral clock
	i2c_clock_enable(p_handle_i2c->p_i2cx);

	// init GPIO pins
	i2c_init_gpio_pins(p_handle_i2c->p_i2cx, 0);

	//reset I2C
	p_handle_i2c->p_i2cx->CR1 |= I2C_CR1_SWRST;
	p_handle_i2c->p_i2cx->CR1 &= ~(I2C_CR1_SWRST);

	// check frequency limits
	if (p_handle_i2c->i2c_config.abp1_freq_mhz < I2C_FREQUENCY_MINIMUM || p_handle_i2c->i2c_config.abp1_freq_mhz > I2C_FREQUENCY_MAXIMUM)
	{
		return I2C_ERROR_WRONG_FREQUENCY;
	}

	// set frequency (same as ABP1 frequency)
	p_handle_i2c->p_i2cx->CR2 &= ~(I2C_CR2_FREQ);
	p_handle_i2c->p_i2cx->CR2 |= (p_handle_i2c->i2c_config.abp1_freq_mhz << I2C_CR2_FREQ_Pos);

	// set speed
	uint16_t temp_ccr, temp_trise;
	// set slow mode, reset DUTY
	p_handle_i2c->p_i2cx->CCR &= ~(I2C_CCR_FS);
	p_handle_i2c->p_i2cx->CCR &= ~(I2C_CCR_DUTY);

	// CCR calculation for slow mode -> values are coming from RM CCR register and result is in [ns]
	// (Thigh + Tlow) / (CEOFF * PCLK)
	temp_ccr = (I2C_CCR_SM_THIGH + I2C_CCR_SM_TLOW) / (I2C_CCR_SM_COEFF * (1000 / p_handle_i2c->i2c_config.abp1_freq_mhz));

	// TRISE calculation for slow mode -> equation is from RM
	temp_trise = ((I2C_CCR_SM_TR_SCL * p_handle_i2c->i2c_config.abp1_freq_mhz) / 1000) + 1;

	// fast mode
	if (p_handle_i2c->i2c_config.speed != I2C_SPEED_SLOW)
	{
		// set fast mode
		p_handle_i2c->p_i2cx->CCR |= I2C_CCR_FS;
		// calculate CCR for fast mode with DUTY 0
		temp_ccr = (I2C_CCR_FM_THIGH + I2C_CCR_FM_TLOW) / (I2C_CCR_FM_COEFF_DUTY0 * (1000 / p_handle_i2c->i2c_config.abp1_freq_mhz));
		// calculate TRISE for fast mode
		temp_trise = ((I2C_CCR_FM_TR_SCL * p_handle_i2c->i2c_config.abp1_freq_mhz) / 1000) + 1;
		if (p_handle_i2c->i2c_config.speed == I2C_SPEED_FAST_DUTY1)
		{
			// set DUTY flag
			p_handle_i2c->p_i2cx->CCR |= I2C_CCR_DUTY;
			// calculate CCR with fast mode DUTY1
			temp_ccr = (I2C_CCR_FM_THIGH + I2C_CCR_FM_TLOW) / (I2C_CCR_FM_COEFF_DUTY1 * (1000 / p_handle_i2c->i2c_config.abp1_freq_mhz));
		}
	}
	p_handle_i2c->p_i2cx->CCR &= ~(I2C_CCR_CCR);
	p_handle_i2c->p_i2cx->CCR |= (temp_ccr << I2C_CCR_CCR_Pos);

	// write correct TRISE to the register
	p_handle_i2c->p_i2cx->TRISE &= ~(I2C_TRISE_TRISE);
	p_handle_i2c->p_i2cx->TRISE |= (temp_trise << I2C_TRISE_TRISE_Pos);

	// enable I2c
	p_handle_i2c->p_i2cx->CR1 |= I2C_CR1_PE;

	return 0;
}

/*
 * Send address on i2c line
 *
 * @param[*p_handle_i2c] - pointer to handler to i2c structure
 * @param[slave_address] - address to slave in 7 bit addressing mode
 * @param[mode] - send information if master is in reciever or transmitter mode @Mode
 * @return - void
 */
static void i2c_send_address(i2c_handle_t *p_handle_i2c, uint8_t slave_address, uint8_t mode)
{
	uint8_t temp_byte;
	//1.0 Set START BIT
	p_handle_i2c->p_i2cx->CR1 |= I2C_CR1_ACK;
	p_handle_i2c->p_i2cx->CR1 |= I2C_CR1_START;

	//1.1 Wait until SB flag is set
	while (!(I2C_SR1_SB & p_handle_i2c->p_i2cx->SR1))
		;
	//1.2 Clear SB by reading SR1
	temp_byte = p_handle_i2c->p_i2cx->SR1;
	//2. Put slave address in DR register -
	p_handle_i2c->p_i2cx->DR = (slave_address << 1);
	// If transmitting set slave addres LSB to 0, reciever 1
	p_handle_i2c->p_i2cx->DR &= ~(1U);
	p_handle_i2c->p_i2cx->DR |= mode;
	//3. ADDR bit set by hardware

	// wait until ADDR is set
	while (!(I2C_SR1_ADDR & p_handle_i2c->p_i2cx->SR1))
		;
	//4. ADDR is cleared by reading SR1 , Read SR2
	temp_byte = p_handle_i2c->p_i2cx->SR1;
	temp_byte = p_handle_i2c->p_i2cx->SR2;
}

/*
 * Transmit data in polling mode
 *
 * @param[*p_handle_i2c] - pointer to handler to i2c structure
 * @param[slave_address] - address to slave in 7 bit addressing mode
 * @param[mem_address] - slave memory register that has to be changed
 * @param[p_data_buffer] - pointer to data buffer that has to be send
 * @param[data_size] - amount of data to be send [in bytes]
 * @return - uint8_t - to return error
 */
uint8_t i2c_transmit(i2c_handle_t *p_handle_i2c, uint8_t slave_address, uint8_t mem_address, uint8_t *p_tx_data_buffer, uint32_t data_size)
{

	uint32_t tx_data_to_send = data_size;
	uint32_t tx_data_index = 0;

	i2c_send_address(p_handle_i2c, slave_address, I2C_MODE_TRANSMITTER);

//5. TxE bit is set when acknowledge bit is sent
	while (!(p_handle_i2c->p_i2cx->SR1 & I2C_SR1_TXE))
		;
//6. Write memory address to DR to clear TxE
	p_handle_i2c->p_i2cx->DR = mem_address;

//7. Data transfer
	while (tx_data_to_send > 0)
	{
		// wait until data register is empty
		while (!(p_handle_i2c->p_i2cx->SR1 & I2C_SR1_TXE))
			;

		// put data in data register
		p_handle_i2c->p_i2cx->DR = p_tx_data_buffer[tx_data_index];

		//change counters
		tx_data_to_send--;
		tx_data_index++;

//8. After last bit is written to DR register , Set STOP bit  and interface is going back to slave mode
		if (tx_data_to_send == 0)
		{
			// check if data transfer is finsihed
			while (!(p_handle_i2c->p_i2cx->SR1 & I2C_SR1_BTF))
				;
			// stop transfer
			p_handle_i2c->p_i2cx->CR1 |= I2C_CR1_STOP;

		}

	}

	return 0;
}

uint8_t i2c_recieve(i2c_handle_t *p_handle_i2c, uint8_t slave_address, uint8_t mem_address, uint8_t *p_rx_data_buffer, uint32_t data_size)
{
	i2c_send_address(p_handle_i2c, slave_address, I2C_MODE_RECIEVER);
	return 0;
}

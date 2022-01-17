/*
 * stm32f401xe_I2C.c
 *
 *  Created on: 12 sty 2022
 *      Author: pawel
 */

#include "stm32f401xe_I2C.h"
#include "stm32f401xe_GPIO.h"
#include "stm32f401xe_RCC.h"

/*
 * Start clock for GPIO
 *
 * @param[*pI2Cx] - base address of i2c peripheral
 * @return - void
 */
static void I2C_ClockEnable(I2C_TypeDef *pI2Cx)
{
	if (pI2Cx == I2C1)
	{
		RCC_CLOCK_I2C1_ENABLE();
	}
	else if (pI2Cx == I2C2)
	{
		RCC_CLOCK_I2C2_ENABLE();
	}
	else if (pI2Cx == I2C3)
	{
		RCC_CLOCK_I2C3_ENABLE();
	}
}

static void I2C_InitGPIOPins(I2C_TypeDef *pI2Cx, uint8_t AlternatePosition)
{
	GPIO_Handle_t GPIO_SDA, GPIO_SCL;

	if (pI2Cx == I2C1)
	{
		// PB6 SCL
		GPIO_SCL.pGPIOx = GPIOB;
		GPIO_SCL.PinConfig.PinNumber = GPIO_PIN_6;

		// PB7 SDA
		GPIO_SDA.pGPIOx = GPIOB;
		GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_7;

		// Alternate Function
		GPIO_SCL.PinConfig.AF = GPIO_PIN_AF_AF4;
		GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF4;

		if (AlternatePosition == 1)
		{
			// PB8 SCL
			GPIO_SCL.PinConfig.PinNumber = GPIO_PIN_8;
			// PB9 SDA
			GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_9;
		}


	}

	if (pI2Cx == I2C2)
	{

		// PB10 SCL
		GPIO_SCL.pGPIOx = GPIOB;
		GPIO_SCL.PinConfig.PinNumber = GPIO_PIN_10;

		// PB11 SDA
		GPIO_SDA.pGPIOx = GPIOB;
		GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_11;

		// Alternate Function
		GPIO_SCL.PinConfig.AF = GPIO_PIN_AF_AF4;
		GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF4;


		if (AlternatePosition == 1)
		{
			// PB3 SDA
			GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_3;
			GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF9;
		}


	}

	if (pI2Cx == I2C3)
	{

		// PA8 SCL
		GPIO_SCL.pGPIOx = GPIOA;
		GPIO_SCL.PinConfig.PinNumber = GPIO_PIN_8;

		// PB9 SDA
		GPIO_SDA.pGPIOx = GPIOB;
		GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_4;

		// Alternate Function
		GPIO_SCL.PinConfig.AF = GPIO_PIN_AF_AF4;
		GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF4;

		if (AlternatePosition == 1)
		{
			RCC_CLOCK_GPIOC_ENABLE();
			// PC9 SDA
			GPIO_SDA.pGPIOx = GPIOC;
			GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_9;

			// Alternate Function
			GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF9;
		}

	}

	// Mode AF
	GPIO_SCL.PinConfig.Mode = GPIO_PIN_MODE_AF;
	GPIO_SDA.PinConfig.Mode = GPIO_PIN_MODE_AF;

	// Output type open drain
	GPIO_SCL.PinConfig.OutputType = GPIO_PIN_OT_OD;
	GPIO_SDA.PinConfig.OutputType = GPIO_PIN_OT_OD;

	// Output speed very high
	GPIO_SCL.PinConfig.OutputSpeed = GPIO_PIN_SPEED_VERYHIGH;
	GPIO_SDA.PinConfig.OutputSpeed = GPIO_PIN_SPEED_VERYHIGH;

	// Pull ups
	GPIO_SCL.PinConfig.PullUpPullDown = GPIO_PIN_PUPD_NOPULL;
	GPIO_SDA.PinConfig.PullUpPullDown = GPIO_PIN_PUPD_NOPULL;

	GPIO_InitPin(&GPIO_SDA);
	GPIO_InitPin(&GPIO_SCL);

}

/*
 * Init I2C peripheral
 *
 * @param[*phI2C] - handler to i2c structure
 * @return - void
 */
uint8_t I2C_Init(I2C_Handle_t *phI2C)
{
	// enable peripheral clock
	I2C_ClockEnable(phI2C->pI2Cx);

	// init GPIO pins
	I2C_InitGPIOPins(phI2C->pI2Cx, 0);

	//reset I2C
	phI2C->pI2Cx->CR1 |= I2C_CR1_SWRST;
	phI2C->pI2Cx->CR1 &= ~(I2C_CR1_SWRST);

	// check frequency limits
	if (phI2C->I2CConfig.ABP1FrequencyMHz < I2C_FREQUENCY_MINIMUM || phI2C->I2CConfig.ABP1FrequencyMHz > I2C_FREQUENCY_MAXIMUM)
	{
		return I2C_ERROR_WRONG_FREQUENCY;
	}

	// set frequency (same as ABP1 frequency)
	phI2C->pI2Cx->CR2 &= ~(I2C_CR2_FREQ);
	phI2C->pI2Cx->CR2 |= (phI2C->I2CConfig.ABP1FrequencyMHz << I2C_CR2_FREQ_Pos);

	// set speed
	uint16_t _tempCCR, _tempTRISE;
	// set slow mode, reset DUTY
	phI2C->pI2Cx->CCR &= ~(I2C_CCR_FS);
	phI2C->pI2Cx->CCR &= ~(I2C_CCR_DUTY);


	// CCR calculation for slow mode -> values are coming from RM CCR register and result is in [ns]
	// (Thigh + Tlow) / (CEOFF * PCLK)
	_tempCCR = (I2C_CCR_SM_THIGH + I2C_CCR_SM_TLOW) / (I2C_CCR_SM_COEFF * (1000 / phI2C->I2CConfig.ABP1FrequencyMHz));

	// TRISE calculation for slow mode -> equation is from RM
	_tempTRISE = ((I2C_CCR_SM_TR_SCL * phI2C->I2CConfig.ABP1FrequencyMHz) / 1000) + 1;

	// fast mode
	if (phI2C->I2CConfig.Speed != I2C_SPEED_SLOW)
	{
		// set fast mode
		phI2C->pI2Cx->CCR |= I2C_CCR_FS;
		// calculate CCR for fast mode with DUTY 0
		_tempCCR = (I2C_CCR_FM_THIGH + I2C_CCR_FM_TLOW) / (I2C_CCR_FM_COEFF_DUTY0 * (1000 / phI2C->I2CConfig.ABP1FrequencyMHz));
		// calculate TRISE for fast mode
		_tempTRISE = ((I2C_CCR_FM_TR_SCL * phI2C->I2CConfig.ABP1FrequencyMHz) / 1000) + 1;
		if (phI2C->I2CConfig.Speed == I2C_SPEED_FAST_DUTY1)
		{
			// set DUTY flag
			phI2C->pI2Cx->CCR |= I2C_CCR_DUTY;
			// calculate CCR with fast mode DUTY1
			_tempCCR = (I2C_CCR_FM_THIGH + I2C_CCR_FM_TLOW) / (I2C_CCR_FM_COEFF_DUTY1 * (1000 / phI2C->I2CConfig.ABP1FrequencyMHz));
		}
	}
	phI2C->pI2Cx->CCR &= ~(I2C_CCR_CCR);
	phI2C->pI2Cx->CCR |= (_tempCCR << I2C_CCR_CCR_Pos);

	// write correct TRISE to the register
	phI2C->pI2Cx->TRISE &= ~(I2C_TRISE_TRISE);
	phI2C->pI2Cx->TRISE |= (_tempTRISE << I2C_TRISE_TRISE_Pos);


	// enable I2c
	phI2C->pI2Cx->CR1 |= I2C_CR1_PE;

	return 0;
}

uint8_t I2C_Transmit(I2C_Handle_t *phI2C, uint8_t SlaveAddres, uint8_t MemAddress, uint8_t *pDataBuffer,uint32_t DataSize)
{
	uint8_t _temp8reg;
	uint32_t _txDataToSend = DataSize;
	uint32_t _txDataIndex = 0;
//1.0 Set START BIT
	phI2C->pI2Cx->CR1 |= I2C_CR1_ACK;
	phI2C->pI2Cx->CR1 |= I2C_CR1_START;

//1.1 Wait until SB flag is set
	while (!(I2C_SR1_SB & phI2C->pI2Cx->SR1))
		;
//1.2 Clear SB by reading SR1
	_temp8reg = phI2C->pI2Cx->SR1;
//2. Put slave address in DR register - If transmitting set slave addres LSB to 0, reciever 1
	phI2C->pI2Cx->DR = (SlaveAddres << 1);
//3. ADDR bit set by hardware

	// wait until ADDR is set
	while (!(I2C_SR1_ADDR & phI2C->pI2Cx->SR1))
		;
//4. ADDR is cleared by reading SR1 , Read SR2
	_temp8reg = phI2C->pI2Cx->SR1;
	_temp8reg = phI2C->pI2Cx->SR2;
//5. TxE bit is set when acknowledge bit is sent
	while (!(phI2C->pI2Cx->SR1 & I2C_SR1_TXE))
		;
//6. Write memory address to DR to clear TxE
	phI2C->pI2Cx->DR = MemAddress;

//7. Data transfer
	while(_txDataToSend > 0)
	{
		// wait until data register is empty
		while (!(phI2C->pI2Cx->SR1 & I2C_SR1_TXE))
			;

		// put data in data register
		phI2C->pI2Cx->DR = pDataBuffer[_txDataIndex];

		//change counters
		_txDataToSend--;
		_txDataIndex++;


//8. After last bit is written to DR register , Set STOP bit  and interface is going back to slave mode
		if(_txDataToSend == 0)
		{
			// check if data transfer is finsihed
			while (!(phI2C->pI2Cx->SR1 & I2C_SR1_BTF))
				;
			// stop transfer
			phI2C->pI2Cx->CR1 |= I2C_CR1_STOP;


		}

	}


	return 0;
}

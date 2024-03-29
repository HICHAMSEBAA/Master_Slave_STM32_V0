/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "MY_NRF24.h"
#include "stdio.h"
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Define the transmission pipe address
uint64_t TxpipeAddrs = 0x11223344AA;

// Define the receiving pipe address
uint64_t RxpipeAddrs = 0x112233447A;

// Define an array to store transmission data with an initial value "SEND"
char myTxData[32] = "SEND To My Data";

// Define an array to store acknowledgment payload
char AckPayload[32];

// Define an array to store received data
char myRxData[32];

// Define an acknowledgment payload with an initial value "Ack by Master !!"
char myAckPayload[32] = "Ack by Master !!";


int count = 0;

// Function to configure NRF24 module for transmit mode without acknowledgment
void Tx_Mode(void) {
	// Print information about entering transmit mode without acknowledgment
//	printf("________________________Tx Mode________________________ \n\r");

	// Stop listening for incoming data
	NRF24_stopListening();

	// Set writing pipe address to TxpipeAddrs
	NRF24_openWritingPipe(TxpipeAddrs);

	// Enable auto acknowledgment
	NRF24_setAutoAck(true);

	// Set channel to 52
	NRF24_setChannel(52);

	// Set payload size to 32 bytes
	NRF24_setPayloadSize(32);

	// Enable dynamic payloads
	NRF24_enableDynamicPayloads();

	// Enable acknowledgment payloads
	NRF24_enableAckPayload();

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, RESET);
}

/*
 * Function: Send_Data
 * Description: Sends data using NRF24 module and waits for acknowledgment from Node 1.
 * Parameters: None
 * Returns: bool - Indicates whether data was sent successfully or not
 */
bool Send_Data(void) {
	// Variable to track the status of data transmission
	bool send_stat = false;

	// Attempt to write data to NRF24 module
	if (NRF24_write(myTxData, 32)) {
		// If data is successfully written, read acknowledgment payload
		NRF24_read(AckPayload, 32);
		char myDataack[80];
		sprintf(myDataack, "AckPayload:  %s \r\n", AckPayload);
		printf("%s \r\n", myDataack);
		send_stat = true;
		// // Check if acknowledgment payload matches the expected acknowledgment
		// if (strcmp(AckPayload, AckPayload) == 0) {
		// 	// If acknowledgment matches, data request was completed successfully
		// 	printf("%s \r\n", AckPayload);
		// 	printf("The data request was completed successfully\r\n");
		// 	send_stat = true;
		// } else {
		// 	// If acknowledgment doesn't match, indicate failure
		// 	printf("No AckPayload from Node 1\r\n");
		// 	send_stat = false;
		// }
	}

	// Delay before returning to avoid immediate retries
	HAL_Delay(50);

	// Return the status of data transmission
	return send_stat;
}

// Function to configure NRF24 module for receiving mode without acknowledgment
void Rx_Mode(void) {
	// Print information about changing settings to receiver mode with acknowledgment
	//printf("________________________Rx Mode________________________ \n\r");

	// Enable auto acknowledgment
	NRF24_setAutoAck(true);

	// Set channel to 52
	NRF24_setChannel(52);

	// Set payload size to 32 bytes
	NRF24_setPayloadSize(32);

	// Open reading pipe with address RxpipeAddrs
	NRF24_openReadingPipe(1, RxpipeAddrs);

	// Enable dynamic payloads
	NRF24_enableDynamicPayloads();

	// Enable acknowledgment payloads
	NRF24_enableAckPayload();

	// Start listening for incoming data
	NRF24_startListening();

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
}

/*
 * Function: Receive_Data
 * Description: Receives data from Node 1 using NRF24 module.
 * Parameters: None
 * Returns: bool - Indicates whether data was received successfully or not
 */
bool Receive_Data(void) {
	// Variable to track the status of data reception
	bool receive_stat = false;

	// Check if there is data available to read
	if (NRF24_available()) {
		// Read data from NRF24 module
		NRF24_read(myRxData, 32);

		// Send acknowledgment payload to Node 1
		NRF24_writeAckPayload(1, myAckPayload, 32);

		count++;
		// Print the received data
		printf("%s : %d\r\n", myRxData, count);

		// Set receive_stat to true to indicate successful data reception
		receive_stat = true;
	}

	// Delay for 10 milliseconds before returning to avoid immediate retries
	HAL_Delay(10);

	// Return the status of data reception
	return receive_stat;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_I2C1_Init();
	MX_SPI2_Init();
	/* USER CODE BEGIN 2 */
	// Initialize NRF24 module
	NRF24_Init();

	// Print information about entering transmit mode without acknowledgment
	printf("//**** TRANSMIT - ACK ****//   \n\r");
	printf(
			"________________________After change Setting________________________ \n\r");

	// Configure NRF24 module for transmit mode without acknowledgment
	Tx_Mode();

	// Print current radio settings
	printRadioSettings();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		/*
		 * Description: This block of code manages the data transmission and reception process.
		 *              It attempts to send data using the Send_Data function and then switches to
		 *              receive mode to listen for acknowledgment from Node 1. If the acknowledgment
		 *              is received successfully, it switches back to transmit mode.
		 */

		// Check if data transmission was successful
		if (Send_Data()) {
		    // Switch NRF24 module to receive mode
		    Rx_Mode();

		    // Print a message indicating that receive mode is activated
		    //printf("Rx Mode is Activated\r\n");

		    // Loop indefinitely until acknowledgment is received or a timeout occurs
		    int coun = 0;
		    while (coun < 5) {
				coun++;
		        // Check if data is received successfully
		        if (Receive_Data()) {
		            // Switch NRF24 module back to transmit mode
		            Tx_Mode();

		           coun++; // Print a message indicating that transmit mode is activated
//		            printf("Tx Mode is Activated\r\n");

		            // Delay for 1000 milliseconds before breaking out of the loop
		            HAL_Delay(500);

		            // Exit the loop
		            break;
		        }else if (coun == 5)
				{
		        	// Switch NRF24 module back to transmit mode
					Tx_Mode();
				}
				
		        // If data reception fails, wait for a short period before retrying
		        HAL_Delay(50);

		        
		    }
		} else {
		    // If data transmission fails, print a failure message
//		    printf("The data request Failed\r\n");

		    // Delay for 1000 milliseconds before continuing
		    HAL_Delay(500);
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 180;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

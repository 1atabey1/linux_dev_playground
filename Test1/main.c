#include "morse.h"
#include <xmc_common.h>
#include <xmc_gpio.h>
#include <xmc_ccu4.h>
#include <xmc_scu.h>


/* CCU4 config */
#define SLICE_PTR         CCU42_CC43
#define MODULE_PTR        CCU42
#define MODULE_NUMBER     (2U)
#define SLICE_NUMBER      (3U)
#define CAPCOM_MASK       (SCU_GENERAL_CCUCON_GSC42_Msk) /**< Only CCU42 */

/* GPIO configuration */
#define GPIO_LED2     XMC_GPIO_PORT1,  0U
#define GPIO_LED1     XMC_GPIO_PORT1,  1U
#define GPIO_BUTTON1  XMC_GPIO_PORT1, 14U
#define GPIO_BUTTON2  XMC_GPIO_PORT1, 15U

/* Utility macros */
#ifndef max
  #define max(a,b) ((a > b) ? a : b)
#endif
#ifndef min
  #define min(a,b) ((a < b) ? a : b)
#endif

/* General defines */
#define PERIOD_100MS 1U
#define SENDSTRING "I CAN MORSE"
#define PAUSEBETWEENTX_MS 5000U

XMC_CCU4_SLICE_COMPARE_CONFIG_t g_timer_object =
{
  .timer_mode 		     = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
  .monoshot   		     = true,
  .shadow_xfer_clear   = 0U,
  .dither_timer_period = 0U,
  .dither_duty_cycle   = 0U,
  .prescaler_mode	     = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
  .mcm_enable		       = 0U,
  .prescaler_initval   = XMC_CCU4_SLICE_PRESCALER_512,
  .float_limit		     = 0U,
  .dither_limit		     = 0U,
  .passive_level 	     = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
  .timer_concatenation = 0U
};

/* CCU Slice Capture Initialization Data */
XMC_CCU4_SLICE_CAPTURE_CONFIG_t g_capture_object =
{
  .fifo_enable 		     = false,
  .timer_clear_mode    = XMC_CCU4_SLICE_TIMER_CLEAR_MODE_NEVER,
  .same_event          = false,
  .ignore_full_flag    = false,
  .prescaler_mode	     = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
  .prescaler_initval   = XMC_CCU4_SLICE_PRESCALER_512,
  .float_limit		     = 0,
  .timer_concatenation = false
};

/* function prototypes */
void ccu4InitTimer();
void setAndEnableNVIC();
void setAndTransferPeriod(uint16_t);
void main_10Hz_Task();

/* global variables */
const XMC_GPIO_CONFIG_t out_config = \
      {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};
const XMC_GPIO_CONFIG_t in_config = \
      {.mode=XMC_GPIO_MODE_INPUT_TRISTATE,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};

char stringCache[1000];
uint8_t transmissionDone = 0U;

// Interrupt handler
void CCU42_0_IRQHandler(void)
{
  XMC_CCU4_SLICE_ClearEvent(SLICE_PTR, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
  main_10Hz_Task();
}

int main(void) {
  XMC_GPIO_Init(GPIO_LED1,&out_config);

  char * tempStr = convString2Morse(SENDSTRING);
  uint16_t k = 0U;
  while (tempStr[k] != '\0')
  {
    stringCache[k] = tempStr[k];
    k++;
  }
  
  ccu4InitTimer();

  while (1U);

  return 0U;
}

void main_10Hz_Task()
{
  static volatile uint16_t i=0U;
  static volatile uint8_t currStp=0U;
  static volatile uint32_t counter=0U;
  static volatile uint8_t numPeriods;
  
  if (stringCache[i] != '\0' && currStp == 0U)
  {
    numPeriods =  convMorse2NumPeriods(stringCache[i]);
    i++;
  }
  else if(currStp == 0U && !transmissionDone)
  {
    counter = 0U;
    transmissionDone = 1U;
    XMC_GPIO_SetOutputLow(GPIO_LED1);
  }
  
  // Pause for 5s after each transmission
  if(transmissionDone)
  {
    if(counter < (PAUSEBETWEENTX_MS / 100U) - 2)
    {
      counter++;
    }
    else
    {
      i = 0U;
      transmissionDone = 0U;
    }
  }
  else
  {
    if(currStp < numPeriods)
    {
      //multiple of 10 indicates pauses
      if (numPeriods % 10U == 0U)
      {
        XMC_GPIO_SetOutputLow(GPIO_LED1);
        currStp += 10U;
      }
      else
      {
        XMC_GPIO_SetOutputHigh(GPIO_LED1);
        currStp++;
      }
    }
    else
    {
      XMC_GPIO_SetOutputLow(GPIO_LED1);
      currStp = 0U;
    }
  }

  
}

void ccu4InitTimer()
{
  XMC_CCU4_SLICE_EVENT_CONFIG_t config;

  config.duration = XMC_CCU4_SLICE_EVENT_FILTER_5_CYCLES;
  config.edge     = XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_RISING_EDGE;
  config.level    = XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH; /* Not needed */
  config.mapped_input = XMC_CCU4_SLICE_INPUT_I;

  /* Ensure fCCU reaches CCU42 */
  XMC_CCU4_SetModuleClock(MODULE_PTR, XMC_CCU4_CLOCK_SCU);

  XMC_CCU4_Init(MODULE_PTR, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);

  /* Get the slice out of idle mode */
  XMC_CCU4_EnableClock(MODULE_PTR, SLICE_NUMBER);

  /* Start the prescaler and restore clocks to slices */
  XMC_CCU4_StartPrescaler(MODULE_PTR);

  /* Initialize the Slice */
  XMC_CCU4_SLICE_CompareInit(SLICE_PTR, &g_timer_object);

  /* Enable compare match and period match events */
  XMC_CCU4_SLICE_EnableEvent(SLICE_PTR, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
  XMC_CCU4_SLICE_EnableEvent(SLICE_PTR, XMC_CCU4_SLICE_IRQ_ID_COMPARE_MATCH_UP);

  /* Connect period match event to SR0 */
  XMC_CCU4_SLICE_SetInterruptNode(SLICE_PTR, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);
  
  setAndEnableNVIC();
  setAndTransferPeriod(PERIOD_100MS);

  /* Configure Event-1 and map it to Input-I */
  XMC_CCU4_SLICE_ConfigureEvent(SLICE_PTR, XMC_CCU4_SLICE_EVENT_1, &config);

  /* Map Event-1 to Start function */
  XMC_CCU4_SLICE_StartConfig(SLICE_PTR, XMC_CCU4_SLICE_EVENT_1, XMC_CCU4_SLICE_START_MODE_TIMER_START_CLEAR);

  /* Set repeat mode */
  XMC_CCU4_SLICE_SetTimerRepeatMode(SLICE_PTR, XMC_CCU4_SLICE_TIMER_REPEAT_MODE_REPEAT);

  /* Generate an external start trigger */
  XMC_SCU_SetCcuTriggerHigh(CAPCOM_MASK);
}

void setAndEnableNVIC()
{
  /* Set priority */
  NVIC_SetPriority(CCU42_0_IRQn, 10U);
  /* Enable IRQ */
  NVIC_EnableIRQ(CCU42_0_IRQn);
}

void setAndTransferPeriod(uint16_t time_100ms)
{
  // time_ms is the final interrupt period we want to achieve
  uint16_t period;

  // Prescaler set to 512
  period = time_100ms * (SystemCoreClock / (10U * 512U));
  /* Program value into PR and CR */
  XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE_PTR, period); // 23447U (?)
  XMC_CCU4_SLICE_SetTimerCompareMatch(SLICE_PTR, period);
  /* Enable shadow transfer */
  XMC_CCU4_EnableShadowTransfer(MODULE_PTR, XMC_CCU4_SHADOW_TRANSFER_SLICE_3);
}

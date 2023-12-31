/***************************************************************************************************
                            ExploreEmbedded Copyright Notice    
****************************************************************************************************
 * File:   01-TaskSwitching
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: File contains the free rtos example to demonstarte the task switching.

This code has been developed and tested on ExploreEmbedded boards.  
We strongly believe that the library works on any of development boards for respective controllers. 
Check this link http://www.exploreembedded.com/wiki for awesome tutorials on 8051,PIC,AVR,ARM,Robotics,RTOS,IOT.
ExploreEmbedded invests substantial time and effort developing open source HW and SW tools, to support consider 
buying the ExploreEmbedded boards.
 
The ExploreEmbedded libraries and examples are licensed under the terms of the new-bsd license(two-clause bsd license).
See also: http://www.opensource.org/licenses/bsd-license.php

EXPLOREEMBEDDED DISCLAIMS ANY KIND OF HARDWARE FAILURE RESULTING OUT OF USAGE OF LIBRARIES, DIRECTLY OR
INDIRECTLY. FILES MAY BE SUBJECT TO CHANGE WITHOUT PRIOR NOTICE. THE REVISION HISTORY CONTAINS THE INFORMATION 
RELATED TO UPDATES.
 

Permission to use, copy, modify, and distribute this software and its documentation for any purpose
and without fee is hereby granted, provided that this copyright notices appear in all copies 
and that both those copyright notices and this permission notice appear in supporting documentation.
**************************************************************************************************/

#include <Arduino_FreeRTOS.h>

void setup()
{
  
  Serial.begin(9600);
  Serial.println(F("In Setup function"));

  /* Create two tasks with priorities 1 and 2. An idle task is also created, 
     which will be run when there are no tasks in RUN state */

  xTaskCreate(MyTask1, "Task1", 200, NULL, 1, 1);
  xTaskCreate(MyTask2, "Task2", 200, NULL, 3, 1);
  //xTaskCreate(print_task,"task3",128,"Task1 functioning",1,NULL);
  //xTaskCreate(print_task,"task4",128,"Task2 functioning",1,NULL);
  xTaskCreate(MyIdleTask, "IdleTask", 200, NULL, 0, NULL);
  vTaskStartScheduler();
}


void loop()
{
  // DO nothing
  /*TaskHandle_t MyIdleTask;
  UBaseType_t uxHighWaterMark;
  // Print actual size of stack has used
  for (;;) {  
  uxHighWaterMark = uxTaskGetStackHighWaterMark(MyIdleTask);
  Serial.println(uxHighWaterMark);
} */
}


/* Task1 with priority 1 */
static void MyTask1(void* pvParameters)
{
  while(1)
  { 
    Serial.println("Task1");
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}


/* Task2 with priority 2 */
static void MyTask2(void* pvParameters)
{
  while(1)
  {    
    Serial.println(F("Task2"));
    vTaskDelay(500/portTICK_PERIOD_MS);
    Serial.println(F("Task3"));
    vTaskDelay(500/portTICK_PERIOD_MS); 
  }
}


/* Idle Task with priority Zero */ 
static void MyIdleTask(void* pvParameters)
{
  while(1)
  {
    Serial.println(F("Idle state"));
    delay(50);
  }
}

static void print_task(void *pvParameters)
{
  unsigned char *p;
  while(1) {
    p=(unsigned char *)pvParameters;
    //Serial.println(p);
    Serial.println(F("\r\n"));
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}


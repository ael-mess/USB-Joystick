/*
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2010  OBinou (obconseil [at] gmail [dot] com)
  Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the RelayBoard program. This file contains the main tasks of
 *  the project and is responsible for the initial application hardware configuration.
 */

#include "PAD.h"


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
    SetupHardware();

    GlobalInterruptEnable();

    for (;;)
    {
        USB_USBTask();
        PAD_Task();
    }
}


/** Configures the board hardware and chip peripherals for the project's functionality. */
void SetupHardware(void)
{
    /*serial Initialization*/
    Serial_Init(9600,0);

    /* Hardware Initialization */
    USB_Init();
}


/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the Atmega320p device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    /* Setup PAD Report Endpoints */
    Endpoint_ConfigureEndpoint(IN_EPADDR, EP_TYPE_INTERRUPT, EPSIZE, 1);
    Endpoint_ConfigureEndpoint(OUT_EPADDR, EP_TYPE_INTERRUPT, EPSIZE, 1);

    /* Turn on Start-of-Frame events for tracking PAD report period expiry */
    USB_Device_EnableSOFEvents();

}


/** Sends the next PAD report to the host, via the Atmega320p data endpoint. */
void SendNextReport(void)
{

    uint8_t KeyReport;

    /* Check if byte are sent from Atmega320p*/
    if(Serial_IsCharReceived())
    {
        /* Save Byte sent from Atmega320p*/
        KeyReport = Serial_ReceiveByte();
        
        /* Select the Atmega320p Report Endpoint */
        Endpoint_SelectEndpoint(IN_EPADDR);

        /* Check if Atmega320p Endpoint Ready for Read/Write and if we should send a new report */
        if (Endpoint_IsReadWriteAllowed())
        {
            /* Write Report Data */
            Endpoint_Write_Stream_LE(&KeyReport, sizeof(KeyReport), NULL);
    
            /* Finalize the stream transfer to send the last packet */
            Endpoint_ClearIN();
        }
    }
}

/** Reads the next LED status report from the host from the LED data endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
    /* Select the Atmega320p LED Report Endpoint */
    Endpoint_SelectEndpoint(OUT_EPADDR);

    /* Check if Atmega320p LED Endpoint contains a packet */
    if (Endpoint_IsOUTReceived())
    {
        /* Check to see if the packet contains data */
        if (Endpoint_IsReadWriteAllowed())
        {
            /* Read in the LED report from the host */
            uint8_t LEDReport = Endpoint_Read_8();
            
            /* Send byte to Atmega320p*/
            Serial_SendByte	(LEDReport);
        }

        /* Handshake the OUT Endpoint - clear endpoint and ready for next report */
        Endpoint_ClearOUT();
    }
}

/** Function to manage PAD report generation and transmission to the host, when in report mode. */
void PAD_Task(void)
{
    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    /* Send the next keypress report to the host */
    SendNextReport();

    /* Process the LED report sent from the host */
    ReceiveNextReport();
}

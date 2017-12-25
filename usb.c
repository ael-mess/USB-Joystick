#include "usb.h"


/****************************************************************
* Function:    get_device_p                                     *
* Parameters:  context, vendor id, product id                   *
* Returns:     none                                             *
* Description: check thepresence of the device in the context   *
****************************************************************/
libusb_device * get_device_p(libusb_context *context, uint8_t idvendor, uint8_t idproduct)
{
    libusb_device **list;
    ssize_t found = 0;
    ssize_t count=libusb_get_device_list(context,&list);
    if(count<0) {perror("libusb_get_device_list"); exit(-1);}
    ssize_t i=0;
    
    libusb_device *device=NULL;
    for(i=0;i<count;i++){
        device=list[i];
        struct libusb_device_descriptor desc;
        int status=libusb_get_device_descriptor(device,&desc);
        if(status!=0) continue;
        if(desc.idVendor==idvendor && desc.idProduct==idproduct){
            found = 1;
            i = count+1;
            uint8_t bus=libusb_get_bus_number(device);
            uint8_t address=libusb_get_device_address(device);
            printf("Device Found @ (Bus:Address) %d:%d\n",bus,address);
            printf("    Vendor ID 0x0%x\n",desc.idVendor);
            printf("    Product ID 0x0%x\n",desc.idProduct);
            
        }
    }
    if(found == 0) { 
        printf("Device not Found @ Vendor ID:0x0%x Product ID:0x0%x\n",idvendor,idproduct);
        device = NULL;
    }
    return device;

    libusb_free_device_list(list,1);
}


/****************************************************************
* Function:    print_config_p                                   *
* Parameters:  **configuration                                  *
* Returns:     none                                             *
* Description: browse the struct config and print if            *
****************************************************************/
void print_config_p(struct libusb_config_descriptor **config)
{ 
    printf("bLength : %d\nbDescriptorType : %d\nwTotalLength : %d\nbNumInterfaces : %d\nbConfigurationValue : %d\niConfiguration : %d\nbmAttributes : 0x0%x\nMaxPower : %d\nextra : %s\nextra_length : %d\n", (*config)->bLength, (*config)->bDescriptorType, (*config)->wTotalLength, (*config)->bNumInterfaces, (*config)->bConfigurationValue, (*config)->iConfiguration, (*config)->bmAttributes, (*config)->MaxPower, (*config)->extra, (*config)->extra_length);
    
    const struct libusb_interface *interface = (*config)->interface;
    int i, j, k;
    for(i=0; i<(*config)->bNumInterfaces; i++)
    {
        printf("interface[%d] :\n  num_altsetting : %d\n", i, interface[i].num_altsetting);
        for(j=0; j<interface[i].num_altsetting; j++)
        {
            printf("  altsetting[%d] :\n    bLength : %d\n    bDescriptorType : %d\n    bInterfaceNumber : %d\n    bAlternateSetting : %d\n    bNumEndpoints : %d\n    bInterfaceClass : %d\n    bInterfaceSubClass : %d\n    bInterfaceProtocol : %d\n    iInterface : %d\n    extra : %s\n    extra_length : %d\n", j, interface[i].altsetting[j].bLength, interface[i].altsetting[j].bDescriptorType, interface[i].altsetting[j].bInterfaceNumber, interface[i].altsetting[j].bAlternateSetting, interface[i].altsetting[j].bNumEndpoints, interface[i].altsetting[j].bInterfaceClass, interface[i].altsetting[j].bInterfaceSubClass, interface[i].altsetting[j].bInterfaceProtocol, interface[i].altsetting[j].iInterface, interface[i].altsetting[j].extra, interface[i].altsetting[j].extra_length);
            
            for(k=0; k<interface[i].altsetting[j].bNumEndpoints; k++)
            {
                const struct libusb_endpoint_descriptor *endpoint = interface[i].altsetting[j].endpoint;
                printf("    endpoint[%d] :\n      bLength : %d\n      bDescriptorType : %d\n      bEndpointAddress : 0x%x\n      bmAttributes : %d\n      wMaxPacketSize : %d\n      bInterval : %d\n      bRefresh : %d\n      bSynchAddress : %d\n      extra : %s\n      extra_length : %d\n", k, endpoint[k].bLength, endpoint[k].bDescriptorType, endpoint[k].bEndpointAddress, endpoint[k].bmAttributes, endpoint[k].wMaxPacketSize, endpoint[k].bInterval, endpoint[k].bRefresh, endpoint[k].bSynchAddress, endpoint[k].extra, endpoint[k].extra_length);
            }
        }
    }
}


/****************************************************************
* Function:    detach_kernel_p                                  *
* Parameters:  *handle                                          *
* Returns:     none                                             *
* Description: browse the configuration and unclaim interfaces  *
****************************************************************/
void detach_kernel_p(libusb_device_handle *handle)
{
    libusb_device *device=libusb_get_device(handle);
    struct libusb_config_descriptor *config = NULL;
    int status=libusb_get_active_config_descriptor(device, &config); //must be freed
    if(status!=0) {perror("libusb_get_active_config_descriptor"); exit(-1);}
    
    const struct libusb_interface *interface = config->interface;
    int i, j;
    for(i=0; i<config->bNumInterfaces; i++)
    {
        for(j=0; j<interface[i].num_altsetting; j++)
        {
            if(libusb_kernel_driver_active(handle,interface[i].altsetting[j].bInterfaceNumber)){
            status=libusb_detach_kernel_driver(handle,interface[i].altsetting[j].bInterfaceNumber);
            if(status!=0){ perror("libusb_detach_kernel_driver"); exit(-1); }
            }
        }
    }
    
    //print_config_p(&config);
    libusb_free_config_descriptor(config);
}


/****************************************************************
* Function:    configure_and_claim_interface_p                  *
* Parameters:  *handle, *endpoint to save                       *
* Returns:     none                                             *
* Description: set the active configuration then browse it to   *
*              claim interfaces and save interrupt endpoints    *
****************************************************************/
void configure_and_claim_interface_p(libusb_device_handle *handle, struct libusb_endpoint_descriptor *endpointsaved)
{    
    libusb_device *device=libusb_get_device(handle);
    struct libusb_config_descriptor *config = NULL;
    int status=libusb_get_config_descriptor(device, 0, &config); //must be freed
    if(status!=0) {perror("libusb_get_config_descriptor"); exit(-1);}
    
    //print_config_p(&config);
    printf("configuration value set : %d\n", config->bConfigurationValue);
    status=libusb_set_configuration(handle,config->bConfigurationValue);
    if(status!=0){ perror("libusb_set_configuration"); exit(-1); }
    
    const struct libusb_interface *interface = config->interface;
    printf("claimed interface :\n");
    int i, j, k, edp=0;
    for(i=0; i<config->bNumInterfaces; i++)
    {
        for(j=0; j<interface[i].num_altsetting; j++)
        {
            status=libusb_claim_interface(handle, interface[i].altsetting[j].bInterfaceNumber); //must be realesed
            if(status!=0){ perror("libusb_claim_interface"); exit(-1); }
            printf("  interface[%d] : altsetting[%d] : bInterfaceNumber %d\n", i, j, interface[i].altsetting[j].bInterfaceNumber);
            
            for(k=0; k<interface[i].altsetting[j].bNumEndpoints; k++)
            {
                const struct libusb_endpoint_descriptor *endpoint = interface[i].altsetting[j].endpoint;
                
                printf("    endpoint[%d] : bDescriptorType : 0x0%x ; bEndpointAddress : 0x%x ; bmAttributes : %d\n", k, endpoint[k].bDescriptorType, endpoint[k].bEndpointAddress, endpoint[k].bmAttributes);
                    
                if(j==0)
                {
                    int it=endpoint[k].bmAttributes;
                    while(it>3) it=it>>1;
                    if(it==3)
                    {
                        endpointsaved[edp] = endpoint[k];
                        edp++;
                        printf("           (saved)\n");
                    
                    }
                }
            }
        }
    }
    
    libusb_free_config_descriptor(config);
}


/****************************************************************
* Function:    release_interface_p                              *
* Parameters:  *handle                                          *
* Returns:     none                                             *
* Description: browse the configuration and unclaim interfaces  *
****************************************************************/
void release_interface_p(libusb_device_handle *handle)
{    
    libusb_device *device=libusb_get_device(handle);
    struct libusb_config_descriptor *config = NULL;
    int status=libusb_get_config_descriptor(device, 0, &config); //must be freed
    if(status!=0) {perror("libusb_get_config_descriptor"); exit(-1);}
    
    const struct libusb_interface *interface = config->interface;
    int i, j;
    for(i=0; i<config->bNumInterfaces; i++)
    {
        for(j=0; j<interface[i].num_altsetting; j++)
        {
            status=libusb_release_interface(handle, interface[i].altsetting[j].bInterfaceNumber);
            if(status!=0){ perror("libusb_release_interface"); exit(-1); }
        }
    }
    
    libusb_free_config_descriptor(config);
}


/****************************************************************
* Function:    transfer_interrupt_in_p                          *
* Parameters:  *handle, interrupt endpoint                      *
* Returns:     char received                                    *
* Description: receive data from atmega328p using endpoint      *
****************************************************************/
unsigned char transfer_interrupt_in_p(libusb_device_handle *handle, struct libusb_endpoint_descriptor endpoint)
{
    uint8_t endpoint_in = endpoint.bEndpointAddress;
    int size = 16;
    unsigned char data;
    
    int timeout = 0;
    int bytes_in = 0;
    int status=libusb_interrupt_transfer(handle, endpoint_in, &data, size, &bytes_in, timeout);
    if(status!=0){ perror("libusb_interrupt_transfer"); exit(-1); }
    return data;
}


/****************************************************************
* Function:    transfer_interrupt_out_p                         *
* Parameters:  *handle, interrupt endpoint, int to send         *
* Returns:     none                                             *
* Description: send data to atmega328p using endpoint to light  *
*              up and lignt off the led                         *
****************************************************************/
void transfer_interrupt_out_p(libusb_device_handle *handle, struct libusb_endpoint_descriptor endpoint, int led)
{
    uint8_t endpoint_out = endpoint.bEndpointAddress;
    int size = 1;
    unsigned char data = led;
    
    int timeout = 10;
    int bytes_in = 0;
    int status=libusb_interrupt_transfer(handle, endpoint_out, &data, size, &bytes_in, timeout);
    if(status!=0){ perror("libusb_interrupt_transfer"); exit(-1); }
}


#include "usb.h"
#include "pacman-1.3/pacman.h"

volatile int cmd = 0; // transfer_interrupt_in_p output

/****************************************************************************
* Function:    main()                                                       *
* Parameters:  argc, argv (passed from command line)                        *    
* Returns:     Success                                                      *
* Description: initiate the program and call the subfunctions and the game  *
****************************************************************************/

int main(int argc, char *argv[100])
{
    int dad = 0; // to close the contexte by the dad process
    
    // open contexte
    libusb_context *context;
    int status=libusb_init(&context);
    if(status!=0) {perror("libusb_init"); exit(-1);}
    
    // get the device and open it
    libusb_device *device = get_device_p(context, idv, idp);
    if(device!=NULL){
        libusb_device_handle *handle;
        status=libusb_open(device,&handle); //must be closed
        if(status!=0){ perror("libusb_open"); exit(-1); }
        
        //detach the kernel and claim interfaces
        struct libusb_endpoint_descriptor endpointsaved[2];
        detach_kernel_p(handle);
        configure_and_claim_interface_p(handle, endpointsaved);
        
        // create two process for the game and the transfer function
        int fdp[2];
        pipe(fdp);
        pid_t pid = fork();
        if(pid != 0)
        {
            // light up the led
            transfer_interrupt_out_p(handle, endpointsaved[1], 1);
            while(cmd != 1) 
            {
                // redirect the interupt output to pipe input
                close(fdp[0]);
                dup2(fdp[1],1);
                close(fdp[1]);
                cmd = transfer_interrupt_in_p(handle, endpointsaved[0]);
                printf("%d\n", cmd);
            }
            // light off the led
            transfer_interrupt_out_p(handle, endpointsaved[1], 0);
            
            wait(NULL);
            
            // release interface and close the device
            release_interface_p(handle);
            libusb_close(handle);
            dad = 1;
        }
        else
        {
            // redirect the pipe output to game input
            close(fdp[1]);
            dup2(fdp[0],0);
            close(fdp[0]);
            // start the game
            main_pac_man(argc, &argv[100]);
        }
        
    }
        
    // exit the context
    if(dad == 1) libusb_exit(context);

    return 0;
}


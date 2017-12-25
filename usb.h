#ifndef _USB_H_
#define _USB_H_

    #include <stdio.h>
    #include <stdlib.h>
    #include <libusb-1.0/libusb.h>

    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <fcntl.h>


    # define idv 0x0002
    # define idp 0x0002


    libusb_device * get_device_p(libusb_context *, uint8_t, uint8_t);
    void print_config_p(struct libusb_config_descriptor **);
    void detach_kernel_p(libusb_device_handle *);
    void configure_and_claim_interface_p(libusb_device_handle *, struct libusb_endpoint_descriptor *);
    void release_interface_p(libusb_device_handle *);
    unsigned char transfer_interrupt_in_p(libusb_device_handle *, struct libusb_endpoint_descriptor);
    void transfer_interrupt_out_p(libusb_device_handle *, struct libusb_endpoint_descriptor, int);

#endif

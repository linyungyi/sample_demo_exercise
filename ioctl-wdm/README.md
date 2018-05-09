
WDM driver with IOCTL operation 
=====

In this sample, I dynamically install  driver, sioctl.sys, when ioctlapp.exe runs at the beginning and uninstall it when it terminated. IOCTL related implements are wrapped in the ioctlLibrary.dll. Sioctl is a simple WDM driver to handle requests by CRL_CODE, which simulates i2c operation, without any real hardware device.

**Note** This sample driver is not a Plug and Play driver. This is a minimal driver meant to demonstrate a feature of the operating system. Neither this driver nor its sample programs are intended for use in a production environment. Instead, they are intended for educational purposes and as a skeleton driver.


Run the sample
--------------

To test this driver, copy the test app, Ioctlapp.exe, and the driver to the same directory, and run the application. The application will automatically load the driver, if it's not already loaded, and interact with the driver. When you exit the application, the driver will be stopped, unloaded and removed.

you might occur some privilege issue, please follow these steps:
1. turn off Secure Boot in the UEFI
2. open cmd with administrator to enable TESTSIGNING and then reboot system
Bcdedit.exe -set TESTSIGNING ON

To rollback all steps when you finish testing to be production evironment again.


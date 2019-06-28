# Device-Drivers

These are a self-development set of projects for device driver development experiments.

Char driver - It is a driver template, which can be used for any kind of character driver development. Currently it
can take a character and print a character. For usage check out its README file.

basic usb driver - It is a basic usb driver, showing how the usb driver functions and various APIs that get called. It has to be further developed into a full fledged usb storage driver, using the SCSI subsystem APIs. Because, in the linux kernel, the 
USB subsystems are horizontal drivers and need verticals like SCSI to come up to the user interface.

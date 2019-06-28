
--When you insert you usb driver, to see it into action, the actual usb driver
has to removed, using "rmmod". For example, if the USB driver is for the stora
ge device, then the driver will usb-storage, with a dependency on uas module.
So if you rmmod the usb-storage, it will be removed but the moment you plug the
pen drive, it will come up again, and your driver will not be used. 

So to make sure your driver is used, there are many ways, one way is to blacklist the usb-storage driver.

To blacklist => vi /etc/modprobe.d/blacklist.conf

then just add the line "blacklist <driver name>", and now when you plug your 
device, automatically your driver will be used. Just make sure the product Id and the vendor Id given in the program is correct. Else it will fail.

To see some action, when you plug, check out "dmesg" command. In the logs you can see what happened and what messages got displayed, and in what order various functions got called

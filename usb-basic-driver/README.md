
--When you insert using "insmod basic-usb.ko" or "rmmod basic-usb.ko" the usb driver, checkout the dmesg command for the registering and
deregistering messages.

--to see the driver into action, the actual usb driver has to removed, using "rmmod". 
For example, if the USB driver is for the  storage device, then the driver will use "usb-storage" 
kernel module, with a dependency on "uas" module. So if you rmmod the "usb-storage", it will be 
removed but the moment you plug the pen drive, it will come up again, and your driver will not be used.

So to make sure your driver is used, there are many ways, one way is to blacklist the usb-storage driver.

To blacklist => vi /etc/modprobe.d/blacklist.conf

then just add the line "blacklist <driver name>", and now when you plug your  device, automatically your 
driver will be used. Just make sure the product Id and the vendor Id given in the program is correct. Else it will fail.

To see some action, when you plug in or plug out, check out "dmesg" command. In the logs, you can see what happened 
and what messages got displayed, and in what order various functions got called.

On inserting the pen drive, you may find /dev/pen0 getting created as a character device node. You may try to read using cat command or write using echo but it will fail, because the data has to be formatted in a specific way, which I am still trying to figure out ;) ...

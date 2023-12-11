# Arduino_cruise_control
A Arduino cruise control solution for vehicles without electronic throttle devices like old diesel trucks.


### Remarks
* this code is not in anyway finished to represent a commercial product.  
* this code contains fragamens of a level sensor. These fragments are deactivated.
* as the hardware will probably run in a 24V (or 12V) environment, you will need at least one voltage dividers (resistors) on the brake pedal input.



# Basic Information
The code is based on the follwing hardware:
* Arduinio Uno (or clone)  
* Arduino Display Shield (like hd44780)  
* 60kg clas RC-Car servo (like ds5160)
* a 5V 5A power supply for the ardino and the servo
* Hall effect sensor (KY-024)
* Some switches and push-buttons for controll of the device
    * Switch to cut power (for emergencies)
    * Red button to stop the cruise contorl (this button is usess the same input pin as the brake pedal switch)
    * Green button to start the cruise control
* A brake pedal switch with voltage divider.

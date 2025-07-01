# Atomatic_Valve_Actuator_Arduino_Code
The sketch below is the Arduino program code for a sound sensor-based automatic valve actuator project
# Sketch Programming
Using the Arduino IDE, the program code was developed by first including the necessary libraries for the LCD, stepper motor, and buzzer. These libraries provide pre-defined functions and methods essential for controlling each component effectively, streamlining the coding process and ensuring reliable communication with the hardware.
Following the library inclusions, we declared a series of global variables that would be used throughout the program. These include pin assignments for key hardware components such as the sound sensor, emergency stop button, status LEDs, valve actuator, stepper motor, and buzzer. Additional variables were also defined to configure the LCD (including its I2C address, number of columns, and rows), as well as to store dynamic values such as detected sound levels, rotation count, and current system status.
The next phase involved creating and defining core control functions and logic blocks. These functions handle system behavior during both the setup() and loop() stages of program execution, enabling initialization, user interaction handling, and real-time control of the actuator system.
Finally, we implemented the following key control instances to govern system behaviour:
i.	Instance for starting up the circuit and the motor: The circuit startup is controlled via an ON/OFF switch, while the motor is controlled via sound trigger or the push button.
ii.	Instance of delay prior to system rotation: This delay matches the pump-set acceleration time, allowing the system to stabilize before engaging the valve actuator.
iii.	Instance of emergency interrupt function: used to trigger emergency interruptions based on critical conditions or user input, ensuring safety and controlled halts in operation.
iv.	Instance to control LED: LEDs are used to visually indicate system states: blue during startup, blinking green during delay, stable green during normal operation, and blinking red during emergency mode.
v.	Instance to beep the buzzer: The buzzer signals key events, including startup, emergency conditions, and the completion of each valve revolution.
vi.	Instance that records and displays every revolution completed: This ensures accurate tracking of valve position, supporting precise opening and closing operations.
vii.	Instances of the LiquidCrystal_I2C, Stepper, and Tone classes: This creates objects to interface with the LCD, stepper motor, and buzzer, respectively, allowing for real-time system feedback and control via the display and audio cues.
viii.	Instance to assign speed to the motor in RPM: Based on the NEMA 24 motor’s specification (200 steps per revolution, 1.8° step angle), the motor was programmed to rotate at 150 RPM, ensuring appropriate speed and torque for valve actuation.
These elements collectively form the backbone of the software architecture, ensuring the seamless operation of the Arduino-based automatic valve actuator system.
The program code for the control algorithm is attached to the appendix section of this report
# Operation
i.	Pressing the ON/OFF switch supplies power to the Arduino controller via an external adapter. This action initializes the controller and configures the input and output pins connected to the various components. 
ii.	Upon initialization, the LCD screen displays the message “Welcome to JK Booster,” accompanied by a beep from the buzzer. After a five-second delay, the screen updates to show “Ready to Open”, indicating the system is fully initialized and awaiting activation.
iii.	The circuit continuously monitors inputs from both the sound sensor and the start push button. If the sound sensor detects the motor starting sound (at a level of 90 dB), or if the start push button is pressed, the system initiates the activation sequence. The start push button is connected to an Interrupt Service Routine (ISR) within the control algorithm, enabling it to override the sound sensor input and serve as an alternative activation trigger.
iv.	The motor enters a stop mode while the system initiates a ten-second delay. During this delay, the LCD sequentially displays “System in Delay Mode…” and “Starting CW…” for five (5) seconds. Simultaneously, the LED module blinks green to indicate the delay status.
v.	After the delay, the motor receives a control signal to begin clockwise rotation, and the LED module switches to green. 
vi.	For every two complete rotations, the buzzer emits a corresponding number of beeps. Once beep after the second rotation, two after the fourth, and three upon completing the sixth.
vii.	As the motor rotates, it transmits rotational energy through a pair of gears to drive the actuating handles. During this process, the LCD screen transitions from displaying “Valve Opening” and “Valve Fully Opened, Ready to Close” after completing the sixth rotation respectively. The system automatically returns to stop mode. 
viii.	If no sound is detected by the sound sensor, or if the start push button is pressed again, the system immediately initiates the valve closing sequence without delay.
ix.	During the closing sequence, the LCD screen displays “Valve Closing,” and the stepper motor rotates counterclockwise for six complete revolutions.
x.	Once the valve is fully closed, the LCD displays “Valve Fully Closed, Ready to Open,” and the system awaits the next activation signal from either the sound sensor or the push button.
xi.	If the emergency push button is pressed at any point during the activation sequence, the system enters emergency mode.
xii.	In emergency mode, the LED module switches to red, the buzzer emits a continuous tone, and the LCD screen displays either “Emergency Mode, Valve Partially Opened” (if the motor was rotating clockwise) or “Emergency Mode, Valve Partially Closed” (if the motor was rotating counterclockwise).
xiii.	Pressing the start button again exits the emergency mode, and the system resumes the previous operation, either continuing to open (clockwise rotation) or close (counterclockwise rotation) the valve.

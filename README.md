# Nucleo_F303K8_Stepper_final
Improved version of Nucleo_F303K8_Stepper

1. The old FIR had a couple of errors.  It sort-of filtered, but this version has been corrected and checked against Mathematica.
2. Dead-zone improvements: for instance, if the joystick has been in the central dead-zone for a pre-determined amount of time, the stepper motor coils are powered down.

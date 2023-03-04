# COUCHBOT!

Couchbot was started a long long time ago in a building that no longer exists. Then, it was abandoned. A brave group of souls tried to revive it, but the batteries exploded. For more info about the before lore read the old readme.docx

Moving on, the couch originally ran on python, but the code was stupid and undocumented. So a new generation of brave explorers decided to breathe new life into couch, using an **Arduino**. The original Arduino 1.0 code was written entirely by Dhruv Bansal in February and March 2023. The project to change to an Arduino was led by then Couch Bot head Faith. This Readme will document the most recent tech and code specs on the Arduino files.


# Important Notes

Arnav Surjan is not allowed to drive the couch!!!
Do not touch the joysticks while turning on the couch!

## High Level Code Overview

The code is comprised of 4 parts. 
1. First the input method. This polls the controller every 20 ms and takes the input y value from both joysticks and writes them to a global variable. More info can be found in the controller test code under test stuff in GitHub.
2. Second is the timer interrupt that goes off every 50 ms. This calls the math functions then writes to the motor controllers.
3. Third is the math functions. We use a system of moving averages to turn a change of velocity with an undefined slope to a gentle curve. Essentially we take the average of the last 6 inputs, then calculate the average of the last 6 averages, then the averages of the last 6 averages^2 to calculate an output to put to the controllers. (original code used 6, current code may use 3 or some other number)
4. Motor controllers. We are using the Sabertooth 2x60. They are extremely powerful and come with their own Arduino library. More info about this can be found under tests and then motor tests in the GitHub.
## Libraries

In Arduino get the USB host shield v2 library. Then use the patch from https://github.com/AlanFord/Logitech_F310_and_Arduino

For the motor controllers, download the drivers from 
https://www.dimensionengineering.com/info/arduino
but these ones should already be in the GitHub and not require downloads.

## Circuit Diagram

Currently incomplete, working on it!

## Current Objectives

Implement Rotary Encoders to dynamiclly change max speed and the throttle on the right side because the left side is less powerful. This means we are currently veering left.

Implement a speaker to create a horn using controller buttons.

Implement a digital e-stop

### Update 3/3/23
Changelog (per Jake): - Examined couch to check for water damage , pi was good, nothing else seemed to have water damage - Couch seemed to be having charging issues and would spark on occasion Cause: Battery Charger was always connected across main neg and main pos of the batteries, fixed by adding a charging switch that will connect and disconnect the charger from the batteries, ziptied to the charger - In doing that, redid main connections (wrapped in pink electrical tape) using correct size bolts and nuts, but were affixed in a way that two of the brackets had to be removed. Extraneous brackets are now in the Couch box TODO: - New castor wheel - 3D printed/laser cut charging switch mount - Wiring cleanup (Later) - General re-tightening of nuts and bolts Jake TODO: - Order sprocket/socket wrench set for the office. 
Jake: "Big thanks to Arnav, Dhruv, and Eddie these past two days"

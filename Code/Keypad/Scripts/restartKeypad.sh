#!/bin/bash

if [ "$(pidof Keypad_Driver)" ]
then
  # process was found
  sudo kill `pgrep Keypad_Driver`;
else
  # process not found
  echo;
fi
sudo /usr/local/bin/Keypad_Driver &
#sudo /home/pi/Utility/Keypad/Test_00/Build/bin/Keypad_Driver_13
#sudo /home/pi/Utility/Keypad/Test_00/Build/bin/Keypad_Driver_V12 &
#:)
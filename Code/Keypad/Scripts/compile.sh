#!/bin/bash

printf "\n\tCompiling: Keypad_Driver\n\n";

g++ -Wall -o /home/pi/Utility/Keypad/Build/bin/Keypad_Driver /home/pi/Utility/Keypad/Build/source/Keypad_Driver_V13.c -lwiringPi;

printf "\n\tDone Compiling\n\n";

if [ "$(pidof Keypad_Driver)" ]
then
  # process was found
  sudo kill `pgrep Keypad_Driver`;
else
  # process not found
  echo;
fi

printf "\n\tRemove . . .\n\n";
sudo rm /usr/local/bin/Keypad_Driver;
printf "\n\tCopy . . .\n\n";
sudo cp /home/pi/Utility/Keypad/Build/bin/Keypad_Driver /usr/local/bin/Keypad_Driver;
printf "\n\tStart . . .\n\n";
sudo /usr/local/bin/Keypad_Driver &

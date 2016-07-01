#!/bin/bash

if [ "$(pidof Keypad_Driver)" ]
then
  # process was found
  sudo kill `pgrep Keypad_Driver`;
else
  # process not found
  echo;
fi
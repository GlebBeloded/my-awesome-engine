import os
import subprocess
import sys
import time
import pyautogui


def keyboard_input(key):

    pyautogui.press(key)

    output = process.stdout.readline()
    output = output.decode(sys.stdout.encoding)
    if key+"_pressed" not in output:
        return os.EX_OSERR

    output = process.stdout.readline()
    output = output.decode(sys.stdout.encoding)
    if key+"_released" not in output:
        return os.EX_OSERR

    return os.EX_OK


module_root = os.path.dirname(os.path.abspath(sys.argv[0]))
module_root = module_root[:module_root.rfind("/")]

gameBinaryPath = module_root+"/build/03-engine-loop/game"


process = subprocess.Popen(gameBinaryPath, shell=True, stdout=subprocess.PIPE)
time.sleep(5)

if keyboard_input("w") != os.EX_OK:
    process.kill()
    exit(os.EX_IOERR)

if keyboard_input("a") != os.EX_OK:
    process.kill()
    exit(os.EX_IOERR)

if keyboard_input("s") != os.EX_OK:
    process.kill()
    exit(os.EX_IOERR)

if keyboard_input("d") != os.EX_OK:
    process.kill()
    exit(os.EX_IOERR)

pyautogui.press("esc")

exit(process.poll())

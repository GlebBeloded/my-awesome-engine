import os, subprocess, sys

module_root = os.path.dirname(os.path.abspath(sys.argv[0]))
module_root = module_root[:module_root.rfind("/")]

files_list = ["/build/02-sdl-link/sdl-link-dynamic","/build/02-sdl-link/sdl-link-static-debug"]

if os.path.isfile(module_root + files_list[0]) is not True:
        print("File", str(module_root + files_list[0]), "does not exist",sep=' ')
        exit(os.EX_OSFILE)

dynamic_check = subprocess.run(["ldd", module_root+files_list[0]],stdout=subprocess.PIPE)
if bytes("libSDL2",sys.stdout.encoding) not in dynamic_check.stdout:
    exit(os.EX_OSERR)

if os.path.isfile(module_root + files_list[1]) is True:
    dynamic_check = subprocess.run(["ldd", module_root+files_list[1]],stdout=subprocess.PIPE)
    if bytes("libSDL2",sys.stdout.encoding) in dynamic_check.stdout:
        exit(os.EX_OSERR)

exit(os.EX_OK)
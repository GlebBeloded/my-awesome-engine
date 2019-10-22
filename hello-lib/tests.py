import os, subprocess, sys

def output_check(string):
    username = os.getenv("USER", "User")
    
    output = subprocess.check_output(string)
    return output == bytes("Hello, " + username + "\n", sys.stdout.encoding)

module_root = os.path.abspath(sys.argv[0])[:-len("tests.py")]

files_list = ["build/bin/hello-bin-static","build/bin/hello-bin-dynamic",
                "build/lib/libhello-lib-dynamic.so","build/lib/libhello-lib-static.a"]

for file_path in files_list:
    if os.path.isfile(module_root + file_path) is not True:
        print("File", str(module_root + file_path), "does not exist",sep=' ')
        exit(os.EX_OSFILE)

if output_check(module_root+files_list[0]) is False \
    or output_check(module_root+files_list[1]) is False:
    print("bad output")
    exit(os.EX_OSERR)

static_check = subprocess.run(["ldd", module_root+files_list[0]],stdout=subprocess.PIPE)
if bytes("not a dynamic executable",sys.stdout.encoding) not in static_check.stdout:
    exit(os.EX_OSERR)

dynamic_check = subprocess.run(["ldd", module_root+files_list[1]],stdout=subprocess.PIPE)
if bytes("libhello-lib-dynamic.so",sys.stdout.encoding) not in dynamic_check.stdout:
    exit(os.EX_OSERR)

exit(os.EX_OK)
import os, subprocess, sys


project_root = os.path.abspath(sys.argv[0])[:-len("tests.py")]

modules = []
for file in os.listdir(project_root):
    if os.path.isdir(project_root + file):
        modules.append(os.path.abspath(project_root + file))

modules.remove(project_root+"cmake")
modules.remove(project_root+"build")
modules.remove(project_root+".git")
modules.remove(project_root+".vscode")


for module in modules:
    result = subprocess.run(["python3",module+"/tests.py"], stdout = subprocess.PIPE)
    if result.returncode is not os.EX_OK:
        print(os.path.basename(module),"module test failed", sep=" ")
        exit(os.EX_OSERR)

exit(os.EX_OK)
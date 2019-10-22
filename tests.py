import os, subprocess, sys

print(project_root)
modules = []
for file in os.listdir(project_root):
    if os.path.isdir(project_root + file):
        modules.append(os.path.abspath(project_root + file))

for module in modules:
    result = subprocess.run(["python3",module+"/tests.py"], stdout = subprocess.PIPE)
    if result.returncode is not os.EX_OK:
        print(os.path.basename(module),"module test failed", sep=" ")
        exit(os.EX_OSERR)

exit(os.EX_OK)
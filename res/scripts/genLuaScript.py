import sys
import os

def gather_all_native_function_binds():
    bound_funcs = []
    root_dir=('../../source/')
    for folder, dirs, files in os.walk(root_dir):
        for file in files:
            full_path = os.path.join(folder, file)
            try:
                with open(full_path, 'r') as f:                    
                    for line in f:
                        if 'BindNativeFunctionToLua("' in line:
                            bound_funcs.append(line.split('"')[3])                            
            except:
                pass
    return bound_funcs

def generate_script(script_name):
    # Get script type
    print("Enter Continuous or Single-Execution type (C/S)?")
    print(">>> ", end = '')

    script_type = get_input()

    file = open(script_name, 'w')
    bound_funcs = gather_all_native_function_binds()
    file.write("-- Native functions exported:\n")
    for func_name in bound_funcs:
        file.write("-- " + func_name + "\n")

    file.write("\n")
    
    function_name = "Update"
    if script_type.lower() == "s" or script_type.lower() == "single":
        function_name = "Execute"
    
    file.write("function " + function_name + "(entityId, dt, dtAccum)\n")
    file.write("\t-- TODO\n")
    file.write("end\n")
    file.close()

# Get correct version of input func
if sys.version_info[0] >= 3:
    get_input = input
else:
    get_input = raw_input
    
# Get script name
print("Enter script name")
print(">>> ", end = '')
script_name = get_input()

if script_name[-4:].lower() != ".lua":
    script_name = script_name + ".lua"

if os.path.isfile(script_name):
    print("Overwrite existing file (Y/N)?")
    print(">>> ", end = '')

    confirmation = get_input()

    if (confirmation.lower() == "y" or confirmation.lower() == "yes"):
        generate_script(script_name)
    else:
        print("Script generation aborted")
else:
    generate_script(script_name)
    
    

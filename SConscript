#
# main building script
#

Import('genv')

test_script = "tests/SConscript"
test_files = genv.SConscript(test_script)
genv.Install(genv["out"], test_files)


import os, cmd

for i in range(1,6):
    command = "./afl-fuzz -i testcases/others/elf/ -o ../../deter/readelf/ETMFuzz_layer2_afl_cpu3-" + str(i) + " -c 3 -l 24 -E 2 ../../../elf_patch/patch_programs/readelf.patch -a @@"
    os.system(command)
# Stalker

#### 1.Compilation and Installation

Navigate to the directory `Stalker/AFL-ETM` and compile Stalker using make `AFL_NO_X86=1` to obtain the **afl-fuzz** program.

#### 2.Kernel Replacement

Replace the original Kernel on the Arm Juno R2 development board with the kernel found in the `Stalker/SOFTWARE` directory.

#### 3.Instrumenting Forkserver

For testing dynamically linked programs, download `patchelf` and use it to replace the `RPATH` of the target program's dynamic linker to `Stalker/lib`. The command is:

```text
patchelf --set-rpath <Stalker/lib directory> <target program>
```

#### 4.Fuzzing the Programs

In the `Stalker/AFL-ETM` directory, use the following command to test the program:

```text
./afl-fuzz -i <test case directory> -o <output directory> -E 3 -c <specified CPU ID> -l <specified test duration in hours> <tested program command, replace test cases with @@>
```

For example:

```text
./afl-fuzz -i testcases/others/elf -o output -E 3 -c 3 -l 24 readelf -a @@
```

# Usage
In the dsat directory, run the `make` command. 

Then, in order to run the files, run `out/app $(DIRECTORIES) | out/view` where DIRECTORIES are all the relative paths to the .cnf files.

It is also possible to execute the program the following way:
```bash
$ out/app $(DIRECTORIES) &
<info>
$ out/view <info>
```
Where one must input manually the info provided by the app program.



# Commands
```bash
make
make rebuild
make all
make clean
make DEBUG=1
```

## Explanation
### make
Runs the rebuild target

### make rebuild
Runs both the clean and all targets

### make all
Run this command to compile all necessary files. The resulting executables will be in the out/folder and should not be moved.

### make clean
Removes all executables from the out directory
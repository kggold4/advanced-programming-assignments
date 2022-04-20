# Advanced Programming Assignment 1

This assignment purpose is to implement the same linux command of ```tree -pugs``` in C language

### Running Example (tree linux command):
[tree pugs]("https://github.com/kggold4/advanced-programming-ex2/screenshots/tree_pugs.png")

The code of this task is in the ```stree.c``` file, to compile this code in linux run:

```
gcc stree.c -o stree
```

Or can use the Makefile command:
```
make all
```

To clean program files (e.g exe and *.o files) use the clean command of the Makefile:
```
make clean
```

To run the program you can give a directory path as argument that the algorithm will work on or not (will work at the current directory), for example:

```
./stree
```

Will show the full tree of directories and file of the current directory path, or:
```
./stree <other-path>
```
Will show the full tree of directories and file of the given directory path.

### Running Example (stree [my code]):
[stree]("https://github.com/kggold4/advanced-programming-ex2/screenshots/stree.png")
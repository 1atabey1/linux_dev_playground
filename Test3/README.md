# Python Extension and Embedding of CPython Test

This is a small proof-of-concept for trying out some Python-C and C-Python interfacing.

Mostly based upon tutorials from https://docs.python.org/3/c-api/index.html and https://docs.python.org/3/extending/embedding.html# and https://docs.python.org/3/extending/extending.html

## What does this testcode do?
### Extending
The `testpkg` module can be built and installed by running `python3 setup.py install` which will give you the option to import it into a python script and use the `testpkg.test` function that it exports.
That function is just a simple wrapper for `printf` but it demonstrates how one could create a python module that interfaces to such C (or C++) functions.
*Note: when used in PyCharm, it will automatically generate some binary skeletons for enabling autocomplete and documentation for the extension module, just as if it were a python module*
### Embedding
The `main()` function in *testpkgmodule.c* is a demonstration of how one could call python code from within arbitrary C code. After building the app by running `make` it can be called with a python function as
an argument and will then execute it. E.g.:
```
./build/my_little_test mylittletest helloc
```
will execute the `helloc()` function in the script `mylittletest.py` which in turn calls the C-function `test_print()` that is defined in *testpkgmodule.c*.
This can also easily be debugged by opening the binary in vscode and then pressing *F5* (make sure the (gdb) launch config is active).

## My thoughts
Seems nice for bigger wrappers or generally more involved C-Python integration efforts. If instead the only goal is to e.g. run code from a C library I'd stick to using `ctypes` (easily import .dll or .so, see https://github.com/hardbyte/python-can/blob/develop/can/interfaces/pcan/basic.py for a relevant example) or `cffi` or similar for other simple interactions.
However, a big plus when directly extending the CPython interpreter is that we dont lose the ability for debugging our C-code (in contrary, we even gain the ability to easily debug in vscode) which is especially nice for e.g. SIL applications.
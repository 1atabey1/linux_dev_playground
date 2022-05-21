#define PY_SSIZE_T_CLEAN // Always include this first
#include <Python.h>      // Always include this second
#include <stdio.h>
#include <unistd.h>

/* Prototype for the method we are exporting*/
static PyObject *
test_print(PyObject *self, PyObject *args);

/* This method table tells the interpreter what methods we're exporting */
static PyMethodDef TestMethods[] = {
    {"test",       // Method name
     test_print,   // Function pointer
     METH_VARARGS, // Type of arguments
     /* Method docstring */
     "testpkg.test(text) -> None \n\n Print something.\n :param text: text to print \n :returns: None\n"},
    {NULL, NULL, 0, NULL} /* Sentinel (always there) */
};

/* This is where we define the module */
static PyModuleDef testmodule = {
    PyModuleDef_HEAD_INIT,
    "testpkg",        /* module name */
    "This is a test", /* module documentation */
    -1,               /* size of per-interpreter state of the module
                         or -1 if the module keeps state in global variables */
    TestMethods       /* Method table */
};

/* Init function, creates the actual module - is called when importing from python script */
PyMODINIT_FUNC
PyInit_testpkg(void)
{
    return PyModule_Create(&testmodule);
}

/* Main function, only relevant for embedding(running py-interpreter in C) not for extending(creating a C-module for python) */
int main(int argc, char *argv[])
{
    /* Get program name (optional) */
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL)
    {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    /* Pass argv[0] to the Python interpreter  (optional) */
    Py_SetProgramName(program);

    /* Check arguments (optional) */
    if (argc < 3)
    {
        fprintf(stderr, "Usage: call pythonfile funcname [args]\n");
        return 1;
    }

    /* Extend the interpreter with this module 
       this is only necessary if you want to use the module in a script */
    if (PyImport_AppendInittab("testpkg", PyInit_testpkg) == -1)
    {
        fprintf(stderr, "Error: could extend in-built modules table\n");
        exit(1);
    }

    /* Initialize the Python interpreter. Required.
       If this step fails it will be a fatal error. */
    Py_Initialize();

    /* Append cwd to sys path to find script */
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Current working dir: %s\n", cwd);
    PyRun_SimpleString("import sys \nimport os \nsys.path.append(os.getcwd()) \n");

    /* Import py script */
    PyObject *moduleName = PyUnicode_FromString(argv[1]);
    PyObject *module = PyImport_Import(moduleName); // Always absolute import
    Py_DECREF(moduleName);                          // DECREF decreases the refcount (delete strong reference if 0)

    if (module != NULL)
    {
        /* Get the function object from the script */
        PyObject *pFunc = PyObject_GetAttrString(module, argv[2]);
        /* Create object for the return value */
        PyObject *pValue;
        /* Try to call the function if it is a valid callable */
        if (pFunc && PyCallable_Check(pFunc))
        {
            /* Assemble arguments */
            PyObject *pArgs = PyTuple_New(argc - 3);
            for (int i = 0; i < argc - 3; ++i)
            {
                pValue = PyLong_FromLong(atoi(argv[i + 3]));
                if (!pValue)
                {
                    Py_DECREF(pArgs);
                    Py_DECREF(module);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                PyTuple_SetItem(pArgs, i, pValue);
            }
            /* This is the actual call */
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL)
            {
                printf("Last result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(module);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return 1;
            }
        }
        else
        {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(module);
    }
    else
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
        return 1;
    }

    /* Say hello :) */
    printf("Hello from C\n");

    /* Clean up and exit */
    PyMem_RawFree(program);
    return 0;
}

/* This is the method we are exporting*/
static PyObject *
test_print(PyObject *self, PyObject *args)
{
    const char *text;

    if (!PyArg_ParseTuple(args, "s", &text))
    {
        return NULL;
    }
    printf("%s", text);
    return PyLong_FromLong(1234);
    // Py_RETURN_NONE; // Use this macro to return nothing
}
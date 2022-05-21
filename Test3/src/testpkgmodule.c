#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>

static PyObject *
test_print(PyObject *self, PyObject *args);

static PyMethodDef TestMethods[] = {
    {"test", test_print, METH_VARARGS,
    "testpkg.test(text) -> None \n\n Print something.\n :param text: text to print \n :returns: None\n"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyModuleDef testmodule = {
    PyModuleDef_HEAD_INIT,
    "testpkg",          /* module name */
    "This is a test",   /* module documentation */
    -1,                 /* size of per-interpreter state of the module
                           or -1 if the module keeps state in global variables */
    TestMethods
};

PyMODINIT_FUNC
PyInit_testpkg(void)
{
    return PyModule_Create(&testmodule);
}

int main(int argc, char *argv[])
{
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL){
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module before Py_Initialize */
    if (PyImport_AppendInittab("spam", PyInit_testpkg) == -1)
    {
        fprintf(stderr, "Error: could extend in-built modules table\n");
        exit(1);
    }

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter. Required.
       If this step fails it will be a fatal error. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       import its. */
    PyObject *pmodule = PyImport_ImportModule("test");
    if(!pmodule) {
        PyErr_Print();
        fprintf(stderr, "Error: could not import module 'spam'\n");
    }

    PyMem_RawFree(program);
    return 0;
}

static PyObject *
test_print(PyObject *self, PyObject *args)
{
    const char *text;
    int sts = 12;

    if (!PyArg_ParseTuple(args, "s", &text))
    {
        return NULL;
    }
    printf("%s", text);
    return PyLong_FromLong(sts);
    // Py_RETURN_NONE;
}
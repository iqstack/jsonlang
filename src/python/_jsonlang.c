/*
Copyright 2015 Cloud M2 Inc. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

----------------------------
Copyright 2015 Google Inc. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stdlib.h>
#include <stdio.h>

#include <Python.h>

#include "../libjsonlang.h"

static PyObject* evaluate_file(PyObject* self, PyObject* args, PyObject *keywds)
{
    const char *filename;
    char *out;
    unsigned max_stack = 500, gc_min_objects = 1000, max_trace = 20;
    double gc_growth_trigger = 2;
    int debug_ast = 0, error;
    PyObject *ext_vars = NULL;
    struct jsonlangVm *vm;
    static char *kwlist[] = {"filename", "max_stack", "gc_min_objects", "gc_growth_trigger", "ext_vars", "debug_ast", "max_trace", NULL};

    (void) self;

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|IIdOiI", kwlist,
                                     &filename,
                                     &max_stack, &gc_min_objects, &gc_growth_trigger, &ext_vars, &debug_ast, &max_trace)) {
        return NULL;
    }

    vm = jsonlang_make();
    jsonlang_max_stack(vm, max_stack);
    jsonlang_gc_min_objects(vm, gc_min_objects);
    jsonlang_max_trace(vm, max_trace);
    jsonlang_gc_growth_trigger(vm, gc_growth_trigger);
    jsonlang_debug_ast(vm, debug_ast);
    if (ext_vars != NULL) {
        PyObject *key, *val;
        Py_ssize_t pos = 0;

        while (PyDict_Next(ext_vars, &pos, &key, &val)) {
            const char *key_ = PyString_AsString(key);
            if (key_ == NULL) {
                jsonlang_destroy(vm);
                return NULL;
            }
            const char *val_ = PyString_AsString(val);
            if (val_ == NULL) {
                jsonlang_destroy(vm);
                return NULL;
            }
            jsonlang_ext_var(vm, key_, val_);
        }
    }

    out = jsonlang_evaluate_file(vm, filename, &error);
    if (error) {
        PyErr_SetString(PyExc_RuntimeError, out);
        jsonlang_realloc(vm, out, 0);
        jsonlang_destroy(vm);
        return NULL;
    } else {
        PyObject *ret = PyString_FromString(out);
        jsonlang_realloc(vm, out, 0);
        jsonlang_destroy(vm);
        return ret;
    }
}

static PyObject* evaluate_snippet(PyObject* self, PyObject* args, PyObject *keywds)
{
    const char *filename, *src;
    char *out;
    unsigned max_stack = 500, gc_min_objects = 1000, max_trace = 20;
    double gc_growth_trigger = 2;
    int debug_ast = 0, error;
    PyObject *ext_vars = NULL;
    struct jsonlangVm *vm;
    static char *kwlist[] = {"filename", "src", "max_stack", "gc_min_objects", "gc_growth_trigger", "ext_vars", "debug_ast", "max_trace", NULL};

    (void) self;

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "ss|IIdOiI", kwlist,
                                     &filename, &src,
                                     &max_stack, &gc_min_objects, &gc_growth_trigger, &ext_vars, &debug_ast, &max_trace)) {
        return NULL;
    }

    vm = jsonlang_make();
    jsonlang_max_stack(vm, max_stack);
    jsonlang_gc_min_objects(vm, gc_min_objects);
    jsonlang_max_trace(vm, max_trace);
    jsonlang_gc_growth_trigger(vm, gc_growth_trigger);
    jsonlang_debug_ast(vm, debug_ast);
    if (ext_vars != NULL) {
        PyObject *key, *val;
        Py_ssize_t pos = 0;

        while (PyDict_Next(ext_vars, &pos, &key, &val)) {
            const char *key_ = PyString_AsString(key);
            if (key_ == NULL) {
                jsonlang_destroy(vm);
                return NULL;
            }
            const char *val_ = PyString_AsString(val);
            if (val_ == NULL) {
                jsonlang_destroy(vm);
                return NULL;
            }
            jsonlang_ext_var(vm, key_, val_);
        }
    }

    out = jsonlang_evaluate_snippet(vm, filename, src, &error);
    if (error) {
        PyErr_SetString(PyExc_RuntimeError, out);
        jsonlang_realloc(vm, out, 0);
        jsonlang_destroy(vm);
        return NULL;
    } else {
        PyObject *ret = PyString_FromString(out);
        jsonlang_realloc(vm, out, 0);
        jsonlang_destroy(vm);
        return ret;
    }
}

static PyMethodDef module_methods[] = {
    {"evaluate_file", (PyCFunction)evaluate_file, METH_VARARGS | METH_KEYWORDS,
     "Interpret the given jsonlang file."},
    {"evaluate_snippet", (PyCFunction)evaluate_snippet, METH_VARARGS | METH_KEYWORDS,
     "Interpret the given jsonlang code."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_jsonlang(void)
{
    Py_InitModule3("_jsonlang", module_methods, "A Python interface to jsonlang.");
}

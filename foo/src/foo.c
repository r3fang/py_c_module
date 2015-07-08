#include <Python.h>
#include <zlib.h>
#include <stdio.h>
#include "kseq.h"
#include "uthash.h"
KSEQ_INIT(gzFile, gzread)

static PyObject *foo_test(PyObject *self, PyObject *args)
{
	test();
	return Py_BuildValue("");
}

static PyObject *foo_strlen(PyObject *self, PyObject *args)
{
	char *s;
	/* Parsing input paramters */
	if (!PyArg_ParseTuple(args, "s", &s)){
		return NULL;
	}
	return Py_BuildValue("i", strlen(s));
}

static PyObject *foo_FastaReader(PyObject *self, PyObject *args){
	char *fasta_file;
	if (!PyArg_ParseTuple(args, "s", &fasta_file)){
		return NULL;
	}
	/* Initlize variable*/
	gzFile fp; kseq_t *seq; int l;
	fp = gzopen(fasta_file, "r");
	if(!fp){
		return NULL;
	}
	seq = kseq_init(fp);
	/* Create a python list to contain the (name, seq)*/
	PyObject *pylist = PyList_New(0);
	while ((l = kseq_read(seq)) >= 0) {
		PyObject *tmp = Py_BuildValue("(s,s)", seq->name.s, seq->seq.s);
		PyList_Append(pylist, tmp);
		Py_DECREF(tmp);
	}
	return pylist;
}

static PyObject *foo_index(PyObject *self, PyObject *args){
	char *fasta_file;
	int k; 
	if (!PyArg_ParseTuple(args, "si", &fasta_file, &k)){
		return NULL;
	}
	/* Initlize variable*/
	gzFile fp; kseq_t *seq; int l;
	fp = gzopen(fasta_file, "r");
	if(!fp){
		return NULL;
	}
	seq = kseq_init(fp);
	/* Create a python list to contain the (name, seq)*/
	char *name;
	char *s;
	while ((l = kseq_read(seq)) >= 0) {
		name = seq->name.s;
		s = seq->seq.s;		
	}
	return Py_BuildValue("");
}

static PyObject *foo_ReverseComplement(PyObject *self, PyObject *args){
	/* Reverse complement of given DNA sequence*/
	char *s;
	if (!PyArg_ParseTuple(args, "s", &s)){
		return NULL;
	}
	int n, c, d;
	n=strlen(s);
	char* r;
	r = (char*)malloc(n * sizeof(char));
	for (c = n - 1, d = 0; c >= 0; c--, d++){
		switch(toupper(s[c])){
			case 'A':
				r[d] = 'T';
				break;
			case 'T':
				r[d] = 'A';
				break;
			case 'C':
				r[d] = 'G';
				break;
			case 'G':
				r[d] = 'C';
				break;
			default:
				r[d] = s[d];
				break;
		}
	}
	r[n] = '\0';
	return Py_BuildValue("s", r);
}

static PyObject *foo_kmer_match(PyObject *self, PyObject *args){
	char *ref;
	char *quary;
	if (!PyArg_ParseTuple(args, "ss", &quary, &ref)){
		return NULL;
	}
	if(strlen(quary) > strlen(ref))
		return NULL;
	int ref_len = strlen(ref);
	int quary_len = strlen(quary);
	int i;
	char* buff;
	buff = (char*)malloc(quary_len * sizeof(char));
	for(i=0; i<ref_len-quary_len; i++){
		memcpy(buff, &ref[i], quary_len);
		buff[quary_len] = '\0';
		if(strcmp(buff, quary) == 0){
			free(buff);
			return Py_BuildValue("i", i);			
		}
	}
	return Py_BuildValue("i", -1);
}

static PyObject *foo_totalIter(PyObject *self, PyObject *args)
{
	PyObject* seq;
	PyObject* item;
	double result = 0.0;
	
	/* get one argument as an iterator */
	if(!PyArg_ParseTuple(args, "O", &seq))
		return 0;
	
	seq = PyObject_GetIter(seq);
	if (!seq)
		return 0;
	/* process data sequentially */
	while((item=PyIter_Next(seq))){
		PyObject *fitem;
		fitem = PyNumber_Float(item);
		if(!fitem) {
			Py_DECREF(seq);
			Py_DECREF(item);
			PyErr_SetString(PyExc_TypeError, "all items must be numbers");
			return 0;
		}
		result += PyFloat_AS_DOUBLE(fitem);
		Py_DECREF(fitem);
		Py_DECREF(item);
	}
	Py_DECREF(seq);
	return Py_BuildValue("d", result);
}	

/* docstring for functions */
static char strlen_docs[] = 
	"Length of a string.";
static char FastaReader_docs[] = 
	"Fasta parser.";
static char totalIter_docs[] = 
	"Sum of a sequence of number.";
static char ReverseComplement_docs[] = 
	"Reverse complement of given DNA sequence.";
static char kmer_match_docs[] = 
	"If given kmer occurs in ref seq.";
static char index_docs[] = 
	"Index reference DNA sequence.";
static char test_docs[] = 
	"Test of hash table.";

static char foo_docs[] = 
	"A collections of non-sense functions.";

/* Method Mapping Fucntion of the module */
static PyMethodDef foo_funcs[] = {
	{"strlen", (PyCFunction)foo_strlen, METH_VARARGS, strlen_docs},
	{"FastaReader", (PyCFunction)foo_FastaReader, METH_VARARGS, FastaReader_docs},
	{"totalIter", (PyCFunction)foo_totalIter, METH_VARARGS, totalIter_docs},
	{"ReverseComplement", (PyCFunction)foo_ReverseComplement, METH_VARARGS, ReverseComplement_docs},	
	{"kmer_match", (PyCFunction)foo_kmer_match, METH_VARARGS, kmer_match_docs},	
	{"index", (PyCFunction)foo_index, METH_VARARGS, index_docs},	
	{"test", (PyCFunction)foo_test, METH_VARARGS, test_docs},	
	{NULL}
};

/* Initialization Function */
PyMODINIT_FUNC initfoo() {
	Py_InitModule3("foo", foo_funcs,
					foo_docs);
}

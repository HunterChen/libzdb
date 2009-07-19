/*
 * Copyright (C) 2004-2009 Tildeslash Ltd. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Config.h"

#include <stdio.h>

#include "URL.h"
#include "ResultSet.h"


/**
 * Implementation of the ResultSet interface 
 *
 * @file
 */


/* ----------------------------------------------------------- Definitions */


#define T ResultSet_T
struct T {
        Rop_T op;
        ResultSetImpl_T I;
};


/* ------------------------------------------------------- Private methods */


static inline int getIndex(T R, const char *name) {
        if (name && *name) {
                int i;
		int columns = ResultSet_getColumnCount(R);
                for (i = 1; i <= columns; i++)
                        if (Str_isByteEqual(name, ResultSet_getColumnName(R, i)))
                                return i;
	}
        THROW(SQLException, "Invalid column name '%s'", name ? name : "null");
        return -1;
}


/* ----------------------------------------------------- Protected methods */


#ifdef PACKAGE_PROTECTED
#pragma GCC visibility push(hidden)
#endif

T ResultSet_new(ResultSetImpl_T I, Rop_T op) {
	T R;
	assert(I);
	assert(op);
	NEW(R);
	R->I = I;
	R->op = op;
	return R;
}


void ResultSet_free(T *R) {
	assert(R && *R);
        (*R)->op->free(&(*R)->I);
	FREE(*R);
}

#ifdef PACKAGE_PROTECTED
#pragma GCC visibility pop
#endif


/* ------------------------------------------------------------ Properties */


int ResultSet_getColumnCount(T R) {
	assert(R);
	return R->op->getColumnCount(R->I);
}


const char *ResultSet_getColumnName(T R, int columnIndex) {
	assert(R);
	return R->op->getColumnName(R->I, columnIndex);
}


long ResultSet_getColumnSize(T R, int columnIndex) {
	assert(R);
	return R->op->getColumnSize(R->I, columnIndex);
}


/* -------------------------------------------------------- Public methods */


int ResultSet_next(T R) {
        return R ? R->op->next(R->I) : false;
}


const char *ResultSet_getString(T R, int columnIndex) {
	assert(R);
	return R->op->getString(R->I, columnIndex);
}


const char *ResultSet_getStringByName(T R, const char *columnName) {
	return ResultSet_getString(R, getIndex(R, columnName));
}


int ResultSet_getInt(T R, int columnIndex) {
	assert(R);
        const char *s = R->op->getString(R->I, columnIndex);
	return s ? Str_parseInt(s) : 0;
}


int ResultSet_getIntByName(T R, const char *columnName) {
	return ResultSet_getInt(R, getIndex(R, columnName));
}


long long int ResultSet_getLLong(T R, int columnIndex) {
	assert(R);
        const char *s = R->op->getString(R->I, columnIndex);
	return s ? Str_parseLLong(s) : 0;
}


long long int ResultSet_getLLongByName(T R, const char *columnName) {
	return ResultSet_getLLong(R, getIndex(R, columnName));
}


double ResultSet_getDouble(T R, int columnIndex) {
	assert(R);
        const char *s = R->op->getString(R->I, columnIndex);
	return s ? Str_parseDouble(s) : 0.0;
}


double ResultSet_getDoubleByName(T R, const char *columnName) {
	return ResultSet_getDouble(R, getIndex(R, columnName));
}


const void *ResultSet_getBlob(T R, int columnIndex, int *size) {
	assert(R);
	return R->op->getBlob(R->I, columnIndex, size);
}


const void *ResultSet_getBlobByName(T R, const char *columnName, int *size) {
	return ResultSet_getBlob(R, getIndex(R, columnName), size);
}


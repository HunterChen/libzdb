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
#include <string.h>
#include <sqlite3.h>

#include "ResultSetStrategy.h"
#include "SQLiteResultSet.h"


/**
 * Implementation of the ResultSet/Strategy interface for SQLite. 
 * Accessing columns with index outside range throws SQLException
 *
 * @file
 */


/* ------------------------------------------------------------- Definitions */


const struct Rop_T sqlite3rops = {
	"sqlite",
        SQLiteResultSet_free,
        SQLiteResultSet_getColumnCount,
        SQLiteResultSet_getColumnName,
        SQLiteResultSet_next,
        SQLiteResultSet_getColumnSize,
        SQLiteResultSet_getString,
        SQLiteResultSet_getBlob,
        SQLiteResultSet_readData
};

#define T ResultSetImpl_T
struct T {
        int keep;
        int maxRows;
	int currentRow;
	int columnCount;
	sqlite3_stmt *stmt;
};

#define TEST_INDEX(RETVAL) \
        int i; assert(R); i= columnIndex - 1; if (R->columnCount <= 0 || \
        i < 0 || i >= R->columnCount) THROW(SQLException, "Column index is out of range");


/* ----------------------------------------------------- Protected methods */


#ifdef PACKAGE_PROTECTED
#pragma GCC visibility push(hidden)
#endif

T SQLiteResultSet_new(void *stmt, int maxRows, int keep) {
	T R;
	assert(stmt);
	NEW(R);
	R->stmt = stmt;
        R->keep = keep;
        R->maxRows = maxRows;
        R->columnCount = sqlite3_column_count(R->stmt);
	return R;
}


void SQLiteResultSet_free(T *R) {
	assert(R && *R);
        if ((*R)->keep)
                sqlite3_reset((*R)->stmt);
        else
                sqlite3_finalize((*R)->stmt);
	FREE(*R);
}


int SQLiteResultSet_getColumnCount(T R) {
	assert(R);
	return R->columnCount;
}


const char *SQLiteResultSet_getColumnName(T R, int column) {
	assert(R);
	column--;
	if (R->columnCount <= 0 ||
	   column < 0           ||
	   column > R->columnCount)
                return NULL;
	return sqlite3_column_name(R->stmt, column);
}


int SQLiteResultSet_next(T R) {
        int status;
	assert(R);
        if (R->maxRows && (R->currentRow++ >= R->maxRows))
                return false;
#if defined SQLITEUNLOCK && SQLITE_VERSION_NUMBER >= 3006012
	status = sqlite3_blocking_step(R->stmt);
#else
        EXEC_SQLITE(status, sqlite3_step(R->stmt), SQL_DEFAULT_TIMEOUT);
#endif
        return (status == SQLITE_ROW);
}


long SQLiteResultSet_getColumnSize(T R, int columnIndex) {
        TEST_INDEX(-1)
        return sqlite3_column_bytes(R->stmt, i);
}


const char *SQLiteResultSet_getString(T R, int columnIndex) {
        TEST_INDEX(NULL)
	return sqlite3_column_text(R->stmt, i);
}


const void *SQLiteResultSet_getBlob(T R, int columnIndex, int *size) {
        const void *blob;
        TEST_INDEX(NULL)
        blob = sqlite3_column_blob(R->stmt, i);
        *size = sqlite3_column_bytes(R->stmt, i);
        return (void*)blob;
}


int SQLiteResultSet_readData(T R, int columnIndex, void *b, int l, long off) {
        long r;
        int size;
        const void *blob;
        TEST_INDEX(0)
        blob = sqlite3_column_blob(R->stmt, i);
        size = sqlite3_column_bytes(R->stmt, i);
        if (off>size)
                return 0;
        r = off+l>size?size-off:l;
        memcpy(b, blob + off, r);
        return r;
}

#ifdef PACKAGE_PROTECTED
#pragma GCC visibility pop
#endif


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
#ifndef SQLITERESULTSET_INCLUDED
#define SQLITERESULTSET_INCLUDED
#include <stdlib.h>
/* SQLite timed retry macro */
#define EXEC_SQLITE(status, action, timeout) \
        do { long t = (timeout * USEC_PER_MSEC); int x = 0;\
        do { status = (action); } while (((status == SQLITE_BUSY) || (status == SQLITE_LOCKED))\
        && (x++ <= 16) && ((Util_usleep(t/(rand() % 10 + 1)))));} while (0)
#define T ResultSetImpl_T
T SQLiteResultSet_new(void *stmt, int maxRows, int keep);
void SQLiteResultSet_free(T *R);
int SQLiteResultSet_getColumnCount(T R);
const char *SQLiteResultSet_getColumnName(T R, int column);
int SQLiteResultSet_next(T R);
long SQLiteResultSet_getColumnSize(T R, int columnIndex);
const char *SQLiteResultSet_getString(T R, int columnIndex);
const void *SQLiteResultSet_getBlob(T R, int columnIndex, int *size);
int SQLiteResultSet_readData(T R, int columnIndex, void *b, int l, long off);
#undef T
#endif

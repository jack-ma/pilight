/*
	Copyright (C) 2013 CurlyMo

	This file is part of pilight.

    pilight is free software: you can redistribute it and/or modify it under the
	terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later
	version.

    pilight is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with pilight. If not, see	<http://www.gnu.org/licenses/>
*/

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include <pwd.h>

int webserver_gc(void);
uid_t webserver_name2uid(char const *name);
void *webserver_start(void *param);
void *webserver_clientize(void *param);
void *webserver_broadcast(void *param);
void webserver_create_header(unsigned char **p, const char *message, char *mimetype, unsigned int len);
char *webserver_mimetype(const char *str);
void webserver_create_minimal_header(unsigned char **p, const char *message, unsigned int len);

#endif

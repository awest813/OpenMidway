/*
 * OS-specific Helpers
 *
 * Copyright (C) 2020-2021 Matt Borgerson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xemu-os-utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>
#include <glib/gprintf.h>

static char *read_file_if_possible(const char *path)
{
	FILE *fd = fopen(path, "rb");
	if (fd == NULL) {
		return NULL;
	}

	if (fseek(fd, 0, SEEK_END) != 0) {
		fclose(fd);
		return NULL;
	}

	long size_long = ftell(fd);
	if (size_long < 0) {
		fclose(fd);
		return NULL;
	}
	size_t size = (size_t)size_long;

	if (fseek(fd, 0, SEEK_SET) != 0) {
		fclose(fd);
		return NULL;
	}

	char *buf = malloc(size + 1);
	if (buf == NULL) {
		fclose(fd);
		return NULL;
	}

	size_t nread = fread(buf, 1, size, fd);
	fclose(fd);
	if (nread != size) {
		free(buf);
		return NULL;
	}

	buf[size] = '\0';
	return buf;
}

/* Parse PRETTY_NAME from os-release (quoted or unquoted, per freedesktop.org). */
static char *parse_pretty_name_from_os_release(const char *contents)
{
	const char *key = "PRETTY_NAME=";
	char *line = strstr(contents, key);
	char *end;
	size_t len;
	char *out;

	if (line == NULL) {
		return NULL;
	}

	line += strlen(key);
	if (*line == '"') {
		line++;
		end = strchr(line, '"');
		if (end == NULL) {
			return NULL;
		}
		len = (size_t)(end - line);
	} else {
		end = line;
		while (*end && *end != '\n' && *end != '\r') {
			end++;
		}
		while (end > line && isspace((unsigned char)end[-1])) {
			end--;
		}
		len = (size_t)(end - line);
	}

	if (len == 0) {
		return NULL;
	}

	out = g_malloc(len + 1);
	memcpy(out, line, len);
	out[len] = '\0';
	return out;
}

const char *xemu_get_os_info(void)
{
	static const char *os_info = NULL;
	static int attempted_init = 0;

	if (!attempted_init) {
		char *pretty = NULL;
		char *os_release_file = read_file_if_possible("/etc/os-release");

		if (os_release_file != NULL) {
			pretty = parse_pretty_name_from_os_release(os_release_file);
			free(os_release_file);
		}

		os_info = g_strdup(pretty ? pretty : "Unknown Distro");
		g_free(pretty);
		attempted_init = 1;
	}

	return os_info;
}

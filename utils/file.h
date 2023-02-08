#pragma once

/// @return Negative errno on failure, otherwise zero.
int file_read_fully(const char* path, char **bufp);

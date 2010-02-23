#ifndef COMMON_H
#define COMMON_H 1

#include "oreore.h"

struct oreore *oreore_alloc(const char *);
void oreore_free(struct oreore *);

bool oreore_strstarts(const char *, const char *);
int oreore_copy_path(const char __user *, char **, ssize_t );
int oreore_dup_path(const char __user *, char **, ssize_t );
int oreore_tidy_path(char *);

#endif

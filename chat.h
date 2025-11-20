#ifndef CHAT_H
#define CHAT_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_USERS 200
#define MAX_GROUPS 50
#define MAX_NAME 50
#define MAX_MSG 1024

/* public API for the simple in-memory chat simulator */
void chat_init(void);
void chat_shutdown(void);
void chat_register_user(void);
void chat_user_session(const char *name);
void chat_show_users(void);
void chat_show_groups(void);

#endif /* CHAT_H */

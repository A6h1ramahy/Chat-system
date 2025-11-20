#include "chat.h"

int main(void)
{
    chat_init();
    char buf[128];

    for (;;) 
    {
        printf("\n+--------------------------------------------------------+\n");
        printf("| 1.Register  2.Login  3.Show users 4.Show groups 5.Exit |\n");
        printf("+--------------------------------------------------------+\n");
        printf("Choice: ");

        if (!fgets(buf, sizeof buf, stdin)) break;
        int c = atoi(buf);

        if (c == 1) chat_register_user();
        
        else if (c == 2) {
            char name[MAX_NAME];
            printf("Username: ");
            if (!fgets(name, sizeof name, stdin)) continue;

            /* small inline trim */
            char *p = name;
            while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) p++;
            
            size_t len = strlen(name);
            while (len && (name[len - 1] == '\n' || name[len - 1] == '\r' || name[len - 1] == ' ' || name[len - 1] == '\t')) name[--len] = 0;
            
            /* shift left if needed */
            if (p != name) memmove(name, p, strlen(p) + 1);
            
            if (strlen(name) == 0) { printf("Invalid\n"); continue; }

            /* findUser is internal to chat.c, so call chat_user_session which checks name */
            chat_user_session(name);
        } 
        else if (c == 3) chat_show_users();
        
        else if (c == 4) chat_show_groups();
        
        else if (c == 5) { chat_shutdown(); printf("Bye\n"); break; }
        
        else printf("Invalid\n");
    }

    return 0;
}


//! PROBLEM STATEMENT
/*
    Build a terminal- or web-based program that enables instant messaging between multiple users.
    Represent users and their connections as a graph, allowing messages to be sent efficiently between friends or group members.
    The program should maintain message history, deliver messages in real time, and notify users when friends are online.
    Using graphs helps manage friend networks and group chats effectively.
*/

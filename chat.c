#include "chat.h"

/* --- internal data structures --- */
typedef struct Node {
    char name[MAX_NAME];
    struct Node *next;
} Node;

typedef struct Message {
    char *text;
    struct Message *next;
} Message;

typedef struct {
    char name[MAX_NAME];
    int online;
    Node *friends;    /* singly-linked list of friend names */
    Message *history; /* delivered messages (chronological) */
    Message *pending; /* messages received while offline */
} User;

typedef struct {
    char name[MAX_NAME];
    char owner[MAX_NAME];
    Node *members; /* singly-linked list of member names */
} Group;

/* static storage */
static User users[MAX_USERS];
static int userCount = 0;
static Group groups[MAX_GROUPS];
static int groupCount = 0;

/* --- helpers --- */
static void trim(char *s)
{
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[--n] = 0;

    char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);

    n = strlen(s);
    while (n && isspace((unsigned char)s[n - 1])) s[--n] = 0;
}

static int findUser(const char *name)
{
    if (!name || !*name) return -1;
    for (int i = 0; i < userCount; i++)
        if (strcmp(users[i].name, name) == 0) return i;
    return -1;
}

static int findGroup(const char *name)
{
    if (!name || !*name) return -1;
    for (int i = 0; i < groupCount; i++)
        if (strcmp(groups[i].name, name) == 0) return i;
    return -1;
}

static int list_contains(Node *n, const char *name)
{
    while (n) {
        if (strcmp(n->name, name) == 0) return 1;
        n = n->next;
    }
    return 0;
}

static void node_add(Node **head, const char *name)
{
    //add at start(sll)
    if (list_contains(*head, name)) return;
    Node *p = malloc(sizeof *p);
    if (!p) return;
    strncpy(p->name, name, MAX_NAME - 1);
    p->name[MAX_NAME - 1] = 0;
    p->next = *head;
    *head = p;
}

static void node_remove(Node **head, const char *name)
{
    Node **pp = head;
    while (*pp) {
        if (strcmp((*pp)->name, name) == 0) {
            Node *t = *pp;
            *pp = t->next;
            free(t);
            return;
        } else {
            pp = &(*pp)->next;
        }
    }
}

/* message helpers */
static void msg_add(Message **head, const char *txt)
{
    Message *m = malloc(sizeof *m);
    if (!m) return;
    m->text = malloc(strlen(txt) + 1);
    if (!m->text) { free(m); return; }
    strcpy(m->text, txt);
    m->next = NULL;

    if (!*head) { *head = m; return; }

    /* append to end for chronological order */
    Message *p = *head;
    while (p->next) p = p->next;
    p->next = m;
}

static void msg_move_all(Message **from, Message **to)
{
    if (!*from) return;
    if (!*to) { *to = *from; *from = NULL; return; }
    Message *p = *to;
    while (p->next) p = p->next;
    p->next = *from;
    *from = NULL;
}

static void msg_print_all(Message *m)
{
    while (m) {
        printf("%s\n", m->text);
        m = m->next;
    }
}

/* --- core messaging --- */
static void append_history_user_index(int user_idx, const char *line)
{
    if (user_idx < 0 || user_idx >= userCount) return;
    msg_add(&users[user_idx].history, line);
}

static void deliver_to(const char *from, const char *to, const char *msg)
{
    int idx = findUser(to);
    if (idx < 0) return;
    char line[2048];
    snprintf(line, sizeof line, "%s -> You: %s", from, msg);
    if (users[idx].online) {
        append_history_user_index(idx, line);
    } else {
        msg_add(&users[idx].pending, line);
    }
}

static void deliver_group_to(const char *group, const char *from, const char *to, const char *msg)
{
    int idx = findUser(to);
    if (idx < 0) return;
    char line[2048];
    snprintf(line, sizeof line, "[%s] %s -> You: %s", group, from, msg);
    if (users[idx].online) {
        append_history_user_index(idx, line);
    } else {
        msg_add(&users[idx].pending, line);
    }
}

/* --- public UI-like actions (mirrors original behaviour) --- */
void chat_init(void)
{
    /* nothing to do for in-memory version */
    userCount = 0;
    groupCount = 0;
}

void chat_shutdown(void)
{
    /* free dynamic memory (friends lists, messages, groups) */
    for (int i = 0; i < userCount; i++) {
        Node *n = users[i].friends;
        while (n) { Node *t = n; n = n->next; free(t); }

        Message *m = users[i].history;
        while (m) { Message *t = m; m = m->next; free(t->text); free(t); }

        m = users[i].pending;
        while (m) { Message *t = m; m = m->next; free(t->text); free(t); }
    }

    for (int i = 0; i < groupCount; i++) {
        Node *n = groups[i].members;
        while (n) { Node *t = n; n = n->next; free(t); }
    }

    userCount = 0; groupCount = 0;
}

/* interactive register (keeps same simple prompt behaviour) */
void chat_register_user(void)
{
    char name[MAX_NAME];
    printf("+--------------------------------------------+\n");
    printf("|            REGISTER NEW USER               |\n");
    printf("+--------------------------------------------+\n");

    printf("Choose username: ");
    if (!fgets(name, sizeof name, stdin)) return;
    trim(name);
    if (!*name) { printf("Invalid\n"); return; }
    if (findUser(name) != -1) { printf("Exists\n"); return; }
    if (userCount >= MAX_USERS) { printf("Limit\n"); return; }

    strncpy(users[userCount].name, name, MAX_NAME - 1);
    users[userCount].name[MAX_NAME - 1] = 0;
    users[userCount].online = 0;
    users[userCount].friends = NULL;
    users[userCount].history = NULL;
    users[userCount].pending = NULL;
    userCount++;

    printf("Registered '%s'\n", name);
}

static void show_friends(const char *me)
{
    int i = findUser(me);
    if (i < 0) { printf("Not found\n"); return; }
    Node *p = users[i].friends;
    if (!p) { printf("(no friends)\n"); return; }

    printf("+----------- Friends -----------+\n");
    printf("| Friends of %-19s|\n", me);
    printf("+-------------------------------+\n");
    while (p) {
        printf("| - %-27s |\n", p->name);
        p = p->next;
    }
    printf("+-------------------------------+\n");
}

static void add_friend(const char *me)
{
    char fre[MAX_NAME];
    printf("Friend to add: ");
    if (!fgets(fre, sizeof fre, stdin)) return;
    trim(fre);
    if (!*fre || strcmp(fre, me) == 0) { printf("Invalid\n"); return; }
    int fidx = findUser(fre);
    if (fidx < 0) { printf("No such user\n"); return; }
    int i = findUser(me);
    if (list_contains(users[i].friends, fre)) { printf("Already friend\n"); return; }
    node_add(&users[i].friends, fre);
    node_add(&users[fidx].friends, me);
    printf("Now friends: %s <-> %s\n", me, fre);
}

static void append_history(const char *user, const char *line)
{
    int idx = findUser(user);
    if (idx < 0) return;
    append_history_user_index(idx, line);
}

static void send_msg(const char *me)
{
    char to[MAX_NAME], msg[MAX_MSG];
    printf("Send to: ");
    if (!fgets(to, sizeof to, stdin)) return; trim(to);
    if (findUser(to) < 0) { printf("No such user\n"); return; }
    if (!list_contains(users[findUser(me)].friends, to)) { printf("You can message only friends\n"); return; }

    printf("Message: ");
    if (!fgets(msg, sizeof msg, stdin)) return; trim(msg);
    if (!*msg) { printf("Empty\n"); return; }

    char sline[2048];
    snprintf(sline, sizeof sline, "You -> %s: %s", to, msg);
    append_history(me, sline);
    deliver_to(me, to, msg);
    printf("Sent.\n");
}

static void view_history(const char *me)
{
    int idx = findUser(me);
    if (idx < 0) { printf("Not found\n"); return; }
    Message *m = users[idx].history;
    if (!m) { printf("(no history)\n"); return; }

    printf("+-------- Message History --------+\n");
    printf("| History for %-20s|\n", me);
    printf("+---------------------------------+\n");
    msg_print_all(m);
    printf("+---------------------------------+\n");
}

static void deliver_pending(const char *me)
{
    int idx = findUser(me);
    if (idx < 0) return;
    if (!users[idx].pending) return;

    printf("\n+------ Offline messages ------+\n");
    msg_print_all(users[idx].pending);
    /* move pending to history so that they are preserved during runtime */
    msg_move_all(&users[idx].pending, &users[idx].history);
    printf("+-----------------------------+\n\n");
}

/* groups */
static void create_group(const char *me)
{
    char g[MAX_NAME];
    printf("Group name: ");
    if (!fgets(g, sizeof g, stdin)) return; trim(g);
    if (!*g || findGroup(g) != -1) { printf("Invalid or exists\n"); return; }
    if (groupCount >= MAX_GROUPS) { printf("limit\n"); return; }

    strncpy(groups[groupCount].name, g, MAX_NAME - 1);
    groups[groupCount].name[MAX_NAME - 1] = 0;
    strncpy(groups[groupCount].owner, me, MAX_NAME - 1);
    groups[groupCount].owner[MAX_NAME - 1] = 0;
    groups[groupCount].members = NULL;
    node_add(&groups[groupCount].members, me);
    groupCount++;

    printf("Created #%s\n", g);
}

static void join_group(const char *me)
{
    char g[MAX_NAME];
    printf("Group to join: ");
    if (!fgets(g, sizeof g, stdin)) return; trim(g);
    int gi = findGroup(g);
    if (gi < 0) { printf("No such group\n"); return; }
    if (list_contains(groups[gi].members, me)) { printf("Already member\n"); return; }
    node_add(&groups[gi].members, me);
    printf("Joined #%s\n", g);
}

static void leave_group(const char *me)
{
    char g[MAX_NAME];
    printf("Group to leave: ");
    if (!fgets(g, sizeof g, stdin)) return; trim(g);
    int gi = findGroup(g);
    if (gi < 0) { printf("No such group\n"); return; }
    if (!list_contains(groups[gi].members, me)) { printf("Not a member\n"); return; }
    if (strcmp(groups[gi].owner, me) == 0) { printf("Owner cannot leave; delete group instead\n"); return; }
    node_remove(&groups[gi].members, me);
    printf("Left #%s\n", g);
}

void chat_show_groups(void)
{
    if (groupCount == 0) { printf("(no groups)\n"); return; }
    for (int i = 0; i < groupCount; i++) {
        printf(" #%s (owner:%s) members:", groups[i].name, groups[i].owner[0] ? groups[i].owner : "(none)");
        Node *m = groups[i].members;
        int first = 1;
        while (m) { if (!first) printf(","); printf(" %s", m->name); first = 0; m = m->next; }
        printf("\n");
    }
}

static void send_group_msg(const char *me)
{
    char g[MAX_NAME], msg[MAX_MSG];
    printf("Group: ");
    if (!fgets(g, sizeof g, stdin)) return; trim(g);
    int gi = findGroup(g);
    if (gi < 0) { printf("No such group\n"); return; }
    if (!list_contains(groups[gi].members, me)) { printf("Not a member\n"); return; }

    printf("Message: ");
    if (!fgets(msg, sizeof msg, stdin)) return; trim(msg);
    Node *m = groups[gi].members;
    if (!*msg) { printf("Empty\n"); return; }

    char own_line[2048];
    snprintf(own_line, sizeof own_line, "[%s] You -> group: %s", g, msg);
    append_history(me, own_line);

    while (m) {
        if (strcmp(m->name, me) != 0) deliver_group_to(g, me, m->name, msg);
        m = m->next;
    }
    printf("Group message sent\n");
}

/* main user session (exposed to main.c) */
void chat_user_session(const char *me)
{
    int idx = findUser(me);
    if (idx < 0) 
    {
        printf("No user found, please Register\n");
        return;
    }

    users[idx].online = 1;
    printf("\n==================================\n");
    printf("   Welcome, %s\n", me);
    printf("==================================\n");

    deliver_pending(me);

    while (1) {
        printf("\n+---------------- %s MENU ----------------+\n", me);
        printf("| 1.Profile  2.Friends  3.Add Friend      |\n");
        printf("| 4.Send     5.History  6.Create Group    |\n");
        printf("| 7.Join     8.Leave    9.List Groups     |\n");
        printf("| 10.Send Group           11.Logout       |\n");
        printf("+-----------------------------------------+\n");
        printf("Choice: ");

        char buf[16]; if (!fgets(buf, sizeof buf, stdin)) return; int c = atoi(buf);
        if (c == 1) printf("User: %s\n", me);
        else if (c == 2) show_friends(me);
        else if (c == 3) add_friend(me);
        else if (c == 4) send_msg(me);
        else if (c == 5) view_history(me);
        else if (c == 6) create_group(me);
        else if (c == 7) join_group(me);
        else if (c == 8) leave_group(me);
        else if (c == 9) chat_show_groups();
        else if (c == 10) send_group_msg(me);
        else if (c == 11) { users[idx].online = 0; printf("Logged out\n"); return; }
        else printf("Bad choice\n");
    }
}

void chat_show_users(void)
{
    if (userCount == 0) { printf("(no users)\n"); return; }
    for (int i = 0; i < userCount; i++)
        printf(" - %s %s\n", users[i].name, users[i].online ? "(online)" : "");
}

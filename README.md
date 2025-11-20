📬 Chat System – Terminal-Based Messaging App

A lightweight terminal-based instant messaging system written in C.
Users can register, log in, add friends, join groups, send messages, and maintain message history — all inside the terminal.

The system internally uses graph-like relationships (friends list & group membership lists) using linked lists to model user connections.

✨ Features
👤 User System

Register new users

Login / Logout

Online/offline status

View all registered users

🤝 Friend Network (Graph Representation)

Add friends (bidirectional connection)

View friend list

Message sending only allowed between friends

Internally stored using singly linked lists, forming an undirected graph

💬 Messaging

Send direct messages to friends

Real-time delivery if the user is online

Stores offline messages and delivers them upon next login

Maintains full message history

👥 Groups

Create groups

Join/leave groups

Group owner permissions

Send group messages

Messages delivered to all group members

Groups internally modeled as linked-list membership graphs

🏗️ Internal Architecture
Data Structures

User

username

online status

friend list (graph edges)

message history

pending (offline) messages

Group

group name

owner

member list (linked list)

Graph Concept

Friends = undirected edges

Group members = hyperedges

Message routing uses adjacency relationships

Files

chat.h

chat.c

main.c

🚀 How to Compile & Run
gcc main.c chat.c -o chat
./chat

📒 Menu Overview
Main Menu
1. Register
2. Login
3. Show Users
4. Show Groups
5. Exit

User Session Menu
1. Profile
2. Friends
3. Add Friend
4. Send Message
5. View History
6. Create Group
7. Join Group
8. Leave Group
9. List Groups
10. Send Group Message
11. Logout

📂 Project Flow

main.c

Displays main menu

Handles login & registration

Redirects to user session

chat.c

Implements all chat logic

Manages users, groups, messages, graph operations

chat.h

Contains all declarations & constants

🧠 Graph Concept (DSA)

Your project uses a graph without explicitly calling it a "graph":

Each user has a linked list of friends
→ This forms an adjacency list graph

Each group has an internal linked list of members
→ Can be viewed as a graph structure connecting multiple nodes

These graphs enable efficient:

Messaging

Friendship validation

Group broadcasting

✔️ Features Completed

User management

Friend graph

Group chat system

Offline message queue

Dynamic memory cleanup

History preservation (runtime only)

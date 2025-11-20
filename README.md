# 📬 Chat System – Terminal-Based Messaging App

A lightweight **terminal-based instant messaging system** written in C.  
Users can register, log in, add friends, join groups, send messages, and maintain message history — all inside the terminal.

The system internally uses **graph-like relationships** (friends list & group membership lists) using linked lists to model user connections.

---

## ✨ Features

### 👤 User System
- Register new users  
- Login / Logout  
- Online/offline status  
- View all registered users  

### 🤝 Friend Network (Graph Representation)
- Add friends (bidirectional connection)  
- View friend list  
- Message sending only allowed between friends  
- Internally stored using **singly linked lists**, forming an undirected graph

### 💬 Messaging
- Send direct messages to friends  
- Real-time delivery if the user is online  
- Stores **offline messages** and delivers them upon next login  
- Maintains full **message history**

### 👥 Groups
- Create groups  
- Join/leave groups  
- Group owner permissions  
- Send group messages  
- Messages delivered to all group members  
- Groups internally modeled as linked-list membership graphs

---

## 🏗️ Internal Architecture

### Data Structures
- **User**
  - username  
  - online status  
  - friend list (graph edges)  
  - message history  
  - pending (offline) messages  

- **Group**
  - group name  
  - owner  
  - member list (linked list)

- **Graph Concept**
  - Friends = undirected edges  
  - Group members = hyperedges  
  - Message routing uses adjacency relationships  

### Files
- `chat.h`  
- `chat.c`  
- `main.c`

---

## 🚀 How to Compile & Run

```bash
gcc main.c chat.c -o chat
./chat

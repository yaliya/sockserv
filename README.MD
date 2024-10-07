# TCP Socket Server written in C

## Requirements
- Build essentials (of course)
- libconfig
- make

``cd src && make``

## Running

Execute the program
``./tcpio``

Using netcat connect (check config.conf for details):
```nc 127.0.0.1 4444```

## Commands

- ```/nick``` Set your nickname
- ```/list``` List all connected users
- ```/msg``` Send a global message
- ```/quit``` Leave server
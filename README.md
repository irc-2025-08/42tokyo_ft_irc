# ft_irc
for development

### start server
```
make
./ircserv <port> <password>
```

### install irc client (irssi)
```
sudo apt install irssi
irssi
```

### test with irssi
```
/connect localhost <port>
/RAWLOG OPEN debug.log
```

### compare with other server
```
/connect DALNET
/RAWLOG OPEN debug2.log
```

### useful irc commands
- `/nick <new_nickname>`: Change nickname
- `/join #<channel_name>`: Join a channel
- `/part #<channel_name>`: Leave a channel
- `/msg <nickname> <message>`: Send a private message to a user
- `/quit <reason>`: Disconnect from the server

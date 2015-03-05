# 8006-a3 (BanHammer!)
A C++ Intrusion Prevention System designed to monitor arbitrary log files with regex and perform system actions.

Setup
-----------------
Banhammer works by creating filters, actions, and associations between the two for monitoring log files and executing system actions
Banhammer can only execute actions at the privilege level it was started in.


There are three main configuration locations for BanHammer, outlined here

```
BanHammer
├── action.d
│   └── *.act
├── banhammer.conf
└── filter.d
    └── *.fil
```

`action.d` contains action definitions in the form of `.act` files
`filter.d` contains filter definitions, `.fil`
and `banhammer.conf` sets up the associations between the two

Filters
-----------------
Filters contain regular expressions that match lines in a log file. Each of these lines will be counted as an attempt.

Directive | Description
----------|------------
`[params]` | Parameters that are global to each filter line. Currently only datetime_regex and datetime_format are used.
`[regex]` | Contains the regex that matches the log file lines. The line after this is a list of names for the groups in the regex.

All sections are necessary but can be in any order

Actions
-----------------
Action definitions may contain any of these detectives:

Directive | Description
----------|------------
`[before]` | Any commands in this section are executed when the action definition is first read into banhammer. Even if this action is used more than once, this section will only be executed the first time
`[act]` | Commands in this section are executed once a linked filter's conditions are met
`[unact]` | These commands are executed after the actions bantime has elapsed since the act section was executed

While not all sections are necessary, they must be in the presented order

Main Config
-----------------
The main config links filters with actions, as well as provides refining arguments, creating rules

All the following parameters must be included in each rule

```
[rulename]
filter=filtername
action=actionname
logfile=/path/to/log.file
period=300
attempts=4
bantime=3600
```

License
-----------------
See [License](LICENSE)

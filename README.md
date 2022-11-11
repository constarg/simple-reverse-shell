# reverse-shell
This program is for educational purposes only.

# Installation requirements
The following packages must be installed.<br>
```
sudo apt-get install build-essential git gcc
```

# Download & Build

First download the program from GitHub and go to the terminal-tool-builder-lib folder.

```
% git clone https://github.com/rounnus/terminal-tool-builder-lib.git
% cd terminal-tool-builder-lib/
```

After installation the program must be built. In order to build the program, the following instruction must be
followed.<br>

```
% make
```

# Usage

```
revershell [OPTION]...
  client [IP] [PORT], Try to connect on a server with ip address [ IP ] on port [ PORT ].
  server [PORT], Create a new connection and listen on port [ PORT ].
```

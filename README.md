# Simple Reverse Shell

[![GitHub license](https://img.shields.io/badge/Licence-MIT-brightgreen)](https://github.com/constarg/simple-reverse-shell/blob/main/LICENSE)
[![Release](https://img.shields.io/badge/Release-1.0-brightgreen)](https://github.com/constarg/simple-reverse-shell/releases/tag/v1.0)
[![Purpose](https://img.shields.io/badge/Purpose-Educational-brightgreen)](https://github.com/constarg/simple-reverse-shell/releases/tag/v1.0)
[![OS](https://img.shields.io/badge/OS-Linux-brightgreen)](https://github.com/constarg/simple-reverse-shell)

# What a reverse shell is?

A reverse shell, also known as a remote shell or “connect-back shell,” takes advantage of the target system’s vulnerabilities to initiate a shell session and then access the victim’s computer. The goal is to connect to a remote computer and redirect the input and output connections of the target system’s shell so the attacker can access it remotely.

Reverse shells allow attackers to open ports to the target machines, forcing communication and enabling a complete takeover of the target machine. Therefore it is a severe security threat. This method is also commonly used in penetration tests.<br><br>
Source: `https://www.imperva.com/learn/application-security/reverse-shell/`

# Disclaimer

As a student who likes the field of security, I always wondered how one could programmatically implement such a tool. The tool is a simple, well commented project for anyone who is purely interested in the educational part only!! like me. Therefore, I bear no responsibility, in case someone, somehow, uses this tool, for any malicious purpose. I hope you like my project and found it interesting, keep learning.

# GitHub Policy on active-malware-or-exploits

https://docs.github.com/en/site-policy/acceptable-use-policies/github-active-malware-or-exploits

# Installation requirements
The following packages must be installed.<br>
```
sudo apt-get install build-essential git gcc
```

# Download & Build

First download the program from GitHub and go to the simple-reverse-shell folder.

```
% git clone https://github.com/constarg/simple-reverse-shell.git
% cd simple-reverse-shell/
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
  server [PORT], Listen on port [ PORT ].
```
In the above usage example, the server plays the role of the system to be attacked and the client plays the role of the attacker. So by running the server on a certain computer, it forces the system to open a port to listen for a client. The client (attacker) can then, if he has access to that computer over the network, run the program as a client and connect to the port that that computer is listening on. <br>

# Diagram 

With a diagram as shown below, I believe it will be fully understood.

![image](https://user-images.githubusercontent.com/38585824/201480795-199ce0aa-72f3-4ed5-bf8e-29b70b2c0119.png)




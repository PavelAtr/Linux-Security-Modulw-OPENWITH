# Linux-Security-Modulw-OPENWITH
Allows files to be opened only by certain programs

Enabled when compiling the kernel with the build option CONFIG_OPENWITH=y. By default, in the system, permission to open a file is controlled by file permissions, and the user can open the file with any program. Sometimes there is a need to allow the user to open a file with only one or a few specified programs so that he can work with the file in a strictly prescribed way. To do this, TinySystem implements a check at the kernel level for the xattr attribute user.openwith. Users can install it on their files themselves. For example we have:

pavel@tinysystem:~$ getfattr test -n user.openwith

file: test

user.openwith="/usr/bin/cat /usr/bin/nano"

Let's try to open it:

pavel@tinysystem:~$ cat test

cat: test: Operation not allowed

(Not successful)

pavel@tinysystem:~$ /usr/bin/cat test

erw

wer

we

rwe

r

ewr

(Successfully)

pavel@tinysystem:~$ /usr/bin/cp test test2

/usr/bin/cp: unable to open 'test' for reading: Operation not allowed

(Not successful, protects indirectly and from copying)

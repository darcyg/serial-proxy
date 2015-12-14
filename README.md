# serial-proxy

`serial-proxy` is a Linux daemon that will proxy data from any number of
serial devices (RS232) to their respective pseudo-terminals (virtual serial
ports). This allows more than one application to receive raw data from a single
serial port.

    /dev/ttyS1 (Garmin GPS)  /dev/ttyUSB1 (Temperature readings)
         |                         |
        / \                       /|\
       /   \                     / | \
      /     \                   /  |  \
    App1   App2              App3 App4 App5

## Build Dependencies

- C++11 compiler
- cmake
- debhelper
- liblogcpp-dev
- libboost-program-options-dev
- libinicpp-dev

`liblogcpp-dev` and `libinicpp-dev` can be found:

    # i386 only
    deb http://repo.faze.io/ trusty testing

    $ sudo apt-get update
    $ sudo apt-get install liblogcpp-dev libinicpp-dev

Or you can build them yourself (they are vanilla C++ libraries):

- https://github.com/rkubik/inih
- https://github.com/rkubik/logcpp

## Build

1. cmake

In the root of the project.

    mkdir build
    cd build
    cmake ..
    make
    make install

2. debuild

In the root of the project.

    debuild -us -uc -b -j4
    debuild clean

Which will produce two debian packages, the runtime daemon and a debugging
package.

## Configuration

`serial-proxy` uses a configuration directory of ini files to read serial port
configuration. Here is a sample stanza for two physical serial ports:

    [/dev/ttyS1]
    virtuals = a,b,c
    baudrate = 9600
    stopbits = 1
    bytesize = 8
    parity = none
    control = none

    [/dev/ttyS2]
    virtuals = a,b,c
    baudrate = 38400

If any of the serial settings are missing from the ini file the defaults below
will be used in their place.

    baudrate = 9600
    stopbits = 1
    bytesize = 8
    parity = none
    control = none

Only one virtual serial port is able to be selected as a `writer` to
communicate with the physical serial port. For the above examples, no virtual
serial ports are able to communicate with their upstream master. To enable this
feature add a `writer =` entry to the INI stanza.

    [/dev/ttyS1]
    virtuals = a,b,c
    writer = a
    baudrate = 9600
    stopbits = 1
    bytesize = 8
    parity = none
    control = none

The above config will read data from `/dev/ttyS1` and write the data to `a`,
`b`, and `c`. Only `a` can communicate with the upstream `/dev/ttyS1` serial
port.

## Usage

Run as a daemon:

    service serial-proxy restart

Run in foreground and output debug messages to screen:

    sproxyd --config /etc/serial-proxy/conf.d --console --loglevel debug

## Examples

    # Verify physical serial port is writing data
    $ cat /dev/ttyUSB0
    �~�30��0��������������������^C

    # Create configuration file for serial port
    $ cat > /etc/serial-proxy/config.ini <<- EOM
    > [/dev/ttyUSB0]
    > virtuals = app1,app2
    > baudrate = 9600
    EOM

    # Start serial-proxy
    $ service serial-proxy start

    # Check existence of virtual serial ports
    $ ls -la /dev/ttyUSB0.*
    lrwxrwxrwx 1 root root 11 /dev/ttyUSB0.app1 -> /dev/pts/25
    lrwxrwxrwx 1 root root 11 /dev/ttyUSB0.app2 -> /dev/pts/26

    # Verify that data is coming through
    $ cat /dev/ttyUSB0.app1
    �~�30��0��������������������^C

    $ cat /dev/ttyUSB0.app2
    �~�30��0��������������������^C

    # Configure your other apps that previously used /dev/ttyUSB0 with either
    # /dev/ttyUSB0.app1 or /dev/ttyUSB0.app2

## TODO

- Add unit tests
- Currently only raw (non-canonical) mode is supported. A configuration field
  could easily be added to let the user choose canonical vs. non-canonical.

## License

MIT

# ping2csv
Command-line utility that converts ping dumps into CSV format.

The program reads a file produced by the Windows/Linux `ping` command, converts it into e.g. comma-separated values and prints it into standard output. Ping round-trip times are stored as integer numbers, where the ping time in milliseconds is multiplied by 100. Example of the output format of this program:

    address,ping_num,ping_ms*100
    140.82.121.3,2,3060
    140.82.121.3,3,3040
    140.82.121.3,4,3040

## Compiling

Clone the repository with

`git clone https://github.com/art-rasa/ping2csv.git`

Then compile the code with

`gcc -o ping2csv ping2csv.c`

Then either place the resulting binary file `ping2csv` (or a link to it) into your system's PATH. 

## Usage

Basic usage for printing the data into the terminal:

`$ ping2csv -<w|l> "<inputfile>" "<separator>`

Output may be redirected into a file (or another program) in the usual way:

`$ ping2csv -<w|l> "<inputfile>" "<separator>" > "[outputfile]"`

`$ ping2csv -<w|l> "<inputfile>" "<separator>" | "[otherprogram]"`

The command-line flag `-w` instructs the program to use the Windows ping format when processing the input file. The other option `-l` instructs the program to use the Linux format. Ping files from other operating systems are not tested. This flag is mandatory.

`<inputfile>` is a text file produced by piping the output from the `ping` command into a file. This argument is mandatory.

`<separator>` is a single character that is used to separate the data fields in the output file. Typical choices are "," (comma character) or "|" (pipe character). This argument is mandatory.

`[outputfile]`, `[otherprogram]` specifies the file (or program) where the converted and redirected data will be stored/piped. This argument is optional.

## Example of use (in Linux)

Create a sample ping dump:

`$ ping -c 5 github.com > testfile.txt`

Viewing its contents:

`$ cat testfile.txt`

    PING github.com (140.82.121.3) 56(84) bytes of data.
    64 bytes from lb-140-82-121-3-fra.github.com (140.82.121.3): icmp_seq=1 ttl=54 time=30.6 ms
    64 bytes from lb-140-82-121-3-fra.github.com (140.82.121.3): icmp_seq=2 ttl=54 time=30.4 ms
    64 bytes from lb-140-82-121-3-fra.github.com (140.82.121.3): icmp_seq=3 ttl=54 time=30.4 ms
    64 bytes from lb-140-82-121-3-fra.github.com (140.82.121.3): icmp_seq=4 ttl=54 time=30.3 ms
    64 bytes from lb-140-82-121-3-fra.github.com (140.82.121.3): icmp_seq=5 ttl=54 time=30.4 ms

    --- github.com ping statistics ---
    5 packets transmitted, 5 received, 0% packet loss, time 4004ms
    rtt min/avg/max/mdev = 30.314/30.403/30.562/0.085 ms

Converting into CSV "Comma-Separated Values" format:

`$ ping2csv -l "testfile.txt" ","`

    address,ping_num,ping_ms*100
    140.82.121.3,2,3060
    140.82.121.3,3,3040
    140.82.121.3,4,3040
    140.82.121.3,5,3030
    140.82.121.3,6,3040













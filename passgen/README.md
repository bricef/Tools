# Password Generation Utilities

## Using dice lists

Run the `dicegen.sh` executable with the number of word:

```
$ ./dicegen.sh 5
tarpammo
rogant
skaar
hravalo
fichar
```

## Using the PGP word list

You can generate a passphrase using the PGP word list as well
```
$ ./pgpwords.py --gen
60 43 95 9e e0 23 3b e1 3a 63 b7 26 8d 5c 59 b9
facial       decimal      preclude     onlooker
tapeworm     cannonball   clockwork    tolerance
cleanup      galveston    seabird      caretaker
optic        fascinate    endow        proximate
```

The `pgpwords.py` util can also convert between pgp words and hex:

```
usage: pgpwords.py [-h] [--gen [N]] [--towords HEXSTRING] [--tohex WORDS]

optional arguments:
  -h, --help           show this help message and exit
  --gen [N]            Generate and display an N byte phrase. N=16 by default.
  --towords HEXSTRING  Accept hex-encoded data and output pgp words. Use - for
                       stdin.
  --tohex WORDS        Accept pgp words and output hex-encoded data argument.
                       Use - for stdin.
```

import sys

out_file = open(sys.argv[2], "wb")
for l in open(sys.argv[1]):
    name = l.rstrip().encode("ascii").upper()[:16]
    out_file.write(name + b"\0"*(16-len(name)))
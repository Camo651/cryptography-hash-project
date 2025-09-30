#!/usr/bin/env sh
FILE=rand_5GiB.bin
s=$(date +%s%N)
openssl dgst -sha256 "$FILE" >/dev/null
e=$(date +%s%N)
awk -v s="$s" -v e="$e" 'BEGIN{printf "openssl: %.3f s\n",(e-s)/1e9}'

s=$(date +%s%N)
./sha256_MDHK -k "$FILE" >/dev/null
e=$(date +%s%N)
awk -v s="$s" -v e="$e" 'BEGIN{printf "mdhk:    %.3f s\n",(e-s)/1e9}'

s=$(date +%s%N)
./sha256 "$FILE" >/dev/null
e=$(date +%s%N)
awk -v s="$s" -v e="$e" 'BEGIN{printf "sha:    %.3f s\n",(e-s)/1e9}'


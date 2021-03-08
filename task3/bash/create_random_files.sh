#! /bin/bash
for n in {1..3}; do
	head -c 100M </dev/urandom >"./server_files"/file$( printf %03d "$n" ).bin
done

if [ "$2" == "src" ]; then
	mkdir "$1" 2> /dev/null
	cp Makefile.proto README ChangeLog *.c *.h secpwgen.1 "$1"
	exit 0
fi

echo "usage: $0 <name> <src|bin>"
exit 1

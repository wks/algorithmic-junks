# This is the helper script for runc/runc++/debugc/debugc++.
# Mechanisms for compiling/running/debugging a given C/C++ program are
# provided in this script.

BASENAME=$(basename $0)

case $BASENAME in
    runc)           PL=C;           DEBUG=FALSE     ;;
    debugc)         PL=C;           DEBUG=TRUE      ;;
    runc++)         PL=CPP;         DEBUG=FALSE     ;;
    debugc++)       PL=CPP;         DEBUG=TRUE      ;;
    runc99)         PL=C99;         DEBUG=FALSE     ;;
    debugc99)       PL=C99;         DEBUG=TRUE      ;;
    runc++11)       PL=CPP11;       DEBUG=FALSE     ;;
    debugc++11)     PL=CPP11;       DEBUG=TRUE      ;;
    *)
        echo "Cannot run as $BASENAME"
        exit 1
        ;;
esac

prog=$1
shift

out=$(mktemp)

#FLAGS= # upstream scripts may have defined this

if [[ $DEBUG == TRUE ]]; then
    FLAGS="$FLAGS -g"
fi

case $PL in
    C)
        COMPILER=gcc
        FLAGS="$FLAGS -std=gnu90"
        ;;
    CPP)
        COMPILER=g++
        FLAGS="$FLAGS -std=gnu++98"
        ;;
    C99)
        COMPILER=gcc
        FLAGS="$FLAGS -std=gnu99"
        ;;
    CPP11)
        COMPILER=g++
        FLAGS="$FLAGS -std=gnu++11"
        ;;
    *)
        echo "Error: Unsupported programming language: $PL"
        exit 1
        ;;
esac

DEBUGGER=gdb

if $COMPILER $FLAGS -o $out $prog; then
    if [[ $DEBUG == TRUE ]]; then
        gdb --quiet --eval-command=run --args $out "$@"
    else
        $out "$@"
    fi
    rm $out

fi

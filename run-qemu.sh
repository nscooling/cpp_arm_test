# QEMU Invocation
# see https://www.qemu.org/docs/master/system/invocation.html

TARGET=Application.elf

USAGE="Usage $0 [gdb] [serial] [elf-image]"

if [[ $1 = --help || $1 = -? || $1 = '-h' ]]; then
    echo "$USAGE" >&2
    exit 1
fi

GDB=
SERIAL=

for arg; do
    case "$arg" in
        gdb) 
            GDB='-S -gdb tcp::1234'
            ;;
        serial) 
            SERIAL='-serial tcp::7777,server,nodelay'
            ;;
        *.ELF|*.elf)
            echo "setting image to $1"
            IMAGE="$1"
            ;;
        *)
            echo "Unrecognised command argument: $arg" >&2
            echo "$USAGE" >&2
            exit 1
            ;;
    esac
    shift
done

# supply default image

: ${IMAGE:=build/debug/$TARGET}

QEMU=~/xpack-qemu-arm-2.8.0-9/bin/qemu-system-gnuarmeclipse

set -x
$QEMU --verbose --board Feabhas-WMS -d unimp,guest_errors --semihosting-config enable=on,target=native $GDB $SERIAL --image $IMAGE

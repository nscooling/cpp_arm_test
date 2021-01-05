
set -o nounset 
set -o errexit

# configurable variables

MAKE_BUILD="make clean; make"
MESON_BUILD="meson compile --clean"
SCONS_BUILD="scons -c; scons"

# command line options for RTOS

SCONS_RTOS="--rtos"
# TODO: implement build support for rtos in make and meson
MAKE_RTOS=""
MESON_RTOS=""

BUILD=
BUILD_RTOS=

SRC=src
SRCBAK=src.bak

BUILD_FILES="$SRC/SConstruct $SRC/SConscript $SRC/meson.build" 

function err_exit {
  [[ -n ${1:-} ]] && echo "$1" >&2
  echo "Build script for exercise solutions" >&2
  echo "Usage: $0 [-q|--quiet] [--scons|--make|--meson] [-A|--all]  [ NN ]" >&2
  echo "  where NN is the chapter number, or" >&2
  echo "  -A --all      to build all solutions" >&2
  echo "  -v --verbose  show diagnostic trace messages" >&2
  echo "  -q --quiet    to suppress output messages, a build failure is always reported" >&2
  echo "  --scons --meson --make" >&2
  echo "               to override default build system" >&2
  exit 1
}

function trace {
  [[ -z $QUIET ]] && echo $1
  : # required due to bug in handling &&
}

function get_solutions {
  SOLDIR=
  for dir in ~ ~/course_material .. ../.. ../../../course_material ; do
    [[ -d $dir/solutions ]] && SOLDIR="$dir/solutions" && break
  done
  [[ -z $SOLDIR ]] && err_exit "Cannot find solutions folder"
  : # required due to bug in handling &&
}

function get_all_exercises {
  EXERCISES=
  EXDIR=$(ls "$1" | egrep '^[0-9]|[0-9][A-Z]?$') 
  for ex in $EXDIR; do
    n=$(echo $ex | sed -rn 's/^(0?[1-9][0-9]?[^0-9])|(0?[1-9][0-9]?[A-Z]?)$/\1\2/p')
    EXERCISES="$EXERCISES $n"
  done
}

VERBOSE=
EXERCISES=
SOLDIR=
REDIR=/dev/stdout
QUIET=

while getopts ":vqhA-:" opt; do
  [[ $opt == - ]] && opt="--$OPTARG"
  [[ $opt == \? && $OPTARG == \? ]] && opt="--help"
  [[ $opt == ? ]] && opt="-$opt"
  case $opt in
    -A|--all) 
      get_solutions 
      get_all_exercises $SOLDIR
      ;;
    -v|--verbose) 
      VERBOSE=y
      ;;
    -q|--quiet) 
      REDIR=/dev/null
      QUIET=y
      ;;
    --scons) 
      [[ -n $BUILD ]] && err_exit "Cannot specify more than one build system"
      [[ ! -f SConstruct ]] && err_exit "Missing SConstruct file required by scons"
      BUILD="$SCONS_BUILD"
      BUILD_RTOS="$SCONS_BUILD $SCONS_RTOS"
      ;;
    --make) 
      [[ -n $BUILD ]] && err_exit "Cannot specify more than one build system"
      [[ ! -f Makefile ]] && err_exit "Missing Makefile file required by make"
      BUILD="$MAKE_BUILD"
      BUILD_RTOS="$SCONS_BUILD $MAKE_RTOS"
      ;;
    --meson) 
      [[ -n $BUILD ]] && err_exit "Cannot specify more than one build system"
      [[ ! -f meson.build ]] && err_exit "Missing meson.build file required by meson"
      BUILD="$MESON_BUILD"
      BUILD_RTOS="$SCONS_BUILD $MESON_RTOS"
      ;;
    --help) 
      err_exit 
      ;;
    *) 
      err_exit "Unknown option '$OPTARG'"
      ;;
  esac
done
shift $(( OPTIND - 1 ))

[[ -z $SOLDIR ]] && get_solutions 
[[ -z $SOLDIR ]] && err_exit "Cannot find solutions folder"


[[ -z $EXERCISES && $# == 0 ]] && err_exit "No solution number specified"
[[ -z $EXERCISES && $# != 1 ]] && err_exit "Cannot specify multiple solution numbers"
[[ -n $EXERCISES && $# != 0 ]] && err_exit "Cannot specify -A (-all) and a solution number"

if [[ -z $EXERCISES ]]; then
  case $1 in
    [1-9]|[1-9][A-Z]) 
      EXERCISES="0$1"
      ;;
    [0-9][0-9]|[0-9][0-9][A-Z]) 
      EXERCISES="$1"
      ;;
    *)
      if [[ ! -d "$SOLDIR/$1" ]]; then
        err_exit "Invalid solution number: $1"
      fi
      EXERCISES="$1"
      ;;
  esac
fi

[[ ! -d $SRC ]] && err_exit "please run this script inside the root of your workspace"

for EX in $EXERCISES; do
  if [[ -d "$SOLDIR/$EX" ]]; then
    EXDIR=$EX
  else
    EXDIR=$(ls "$SOLDIR" | egrep "^0?${EX}[^0-9]|0?${EX}$") || true
    [[ -z $EXDIR ]] && err_exit "No solution provided for exercise $EX"
    (cd $SOLDIR; ls -d $EXDIR); 
    N=$(cd $SOLDIR; ls -d $EXDIR | wc -l)
    (( N > 1 )) && err_exit "Configuration error - multiple solutions for exercise $EX"
  fi
  
  if [[ -d $SRCBAK ]]; then
    rm -rf "$SRCBAK"/* 2>/dev/null
  else
    trace "Making source backup folder $SRCBAK"
    mkdir "$SRCBAK"
  fi

  trace "Moving source files to $SRCBAK"
  mv "$SRC"/*.[ch] "$SRC"/*.cpp "$SRCBAK" 2>/dev/null || true
  trace "Copying build files to $SRCBAK"
  cp "$BUILD_FILES" "$SRCBAK" 2>/dev/null || true

  if [[ -d "$SOLDIR/$EXDIR/src" ]]; then
  	EXDIR="$EXDIR/src"
  fi
  trace "Copying solution files from $SOLDIR/$EXDIR"
  cp -f "$SOLDIR/$EXDIR"/* $SRC 2>/dev/null || true

  RTOS=
  if grep -q '[Ff]eabhOS' $SRC/*.c $SRC/*.cpp 2>/dev/null; then
    RTOS=1
  fi
  
  if [[ -n $VERBOSE ]]; then
    echo "checking $EX" >&2
  fi
  
  set +o errexit

  if [[ -n $BUILD ]]; then
    [[ -n $RTOS ]] && BUILD="$BUILD_RTOS"
    trace "Building solutions using $BUILD"
    (eval $(echo "$BUILD")) >$REDIR 2>&1
  elif [[ -f SConstruct ]]; then
    [[ -n $RTOS ]] && SCONS_BUILD="$SCONS_BUILD $SCONS_RTOS"
    trace "Running scons build: $SCONS_BUILD"
    (eval $(echo "$SCONS_BUILD")) >$REDIR 2>&1
  elif [[ -f Makefile ]]; then
    [[ -n $RTOS ]] && MAKE_BUILD="$MAKE_BUILD $MAKE_RTOS"
    trace "Running make build: $MAKE_BUILD"
    (eval $(echo "$MAKE_BUILD")) >$REDIR 2>&1
  elif [[ -f meson.build ]]; then
    [[ -n $RTOS ]] && MESON_BUILD="$MESON_BUILD $MESON_RTOS"
    trace "Running meson build: $MESON_BUILD"
    (eval $(echo "$MESON_BUILD")) >$REDIR 2>&1
  else
    err_exit "Cannot file suitable build configuration file"
  fi
  status=$?
  if [[ $status != 0 ]]; then
    echo "Build failed for solution $EX"
    exit $status
  fi
done


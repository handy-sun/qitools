#!/bin/sh
app="qitools"
ver="0.13.1"
# Display usage
echo_usage() {
    cat <<EOF
Usage: $0 [options]
Options: [defaults in brackets after descriptions]
  --help            print this message
  --version         print cmake installer version
  --prefix=dir      directory in which to install
EOF
    exit 1
}

echo_err_exit() {
    echo $1
    exit 1
}

# Display version
echo_version() {
    echo "${app} version: ${ver}"
}

# Helper function to fix windows paths.
fix_slashes() {
    echo "$1" | sed 's/\\/\//g'
}

include_subdir=""
for a in "$@"; do
    if echo $a | grep "^--prefix=" >/dev/null 2>/dev/null; then
        prefix_dir=$(echo $a | sed "s/^--prefix=//")
        prefix_dir=$(fix_slashes "${prefix_dir}")
    fi
    if echo $a | grep "^--help" >/dev/null 2>/dev/null; then
        echo_usage
    fi
    if echo $a | grep "^--version" >/dev/null 2>/dev/null; then
        echo_version
        exit 2
    fi
done

echo_version
echo "This is a self-extracting archive."
toplevel="$(pwd)"
if [ "x${prefix_dir}x" != "xx" ]; then
    toplevel="${prefix_dir}"
fi

echo "The archive will be extracted to: ${toplevel}"

# if [ "x${qitl_include_subdir}x" = "xTRUEx" ]
# then
#   toplevel="${toplevel}/${app}-${ver}"
#   mkdir -p "${toplevel}"
# fi
echo
echo "Using target directory: ${toplevel}"
echo "Extracting, please wait..."
echo ""

# take the archive portion of this file and pipe it to tar
# the NUMERIC parameter in this command should be one more
# than the number of lines in this header file
# there are tails which don't understand the "-n" argument, e.g. on SunOS
# OTOH there are tails which complain when not using the "-n" argument (e.g. GNU)
# so at first try to tail some file to see if tail fails if used with "-n"
# if so, don't use "-n"
use_new_tail_syntax="-n"
tail $use_new_tail_syntax +1 "$0" >/dev/null 2>/dev/null || use_new_tail_syntax=""

extractor="pax -r"
command -v pax >/dev/null 2>/dev/null || extractor="tar --no-same-owner -xf -"

tail $use_new_tail_syntax +85 "$0" | gunzip | (cd "${toplevel}" && ${extractor}) || echo_err_exit "Problem unpacking the package"

echo "Unpacking finished successfully"
exit 0
#-----------------------------------------------------------
#      Start of TAR.GZ file
#-----------------------------------------------------------;

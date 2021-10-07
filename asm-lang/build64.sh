# Build script? todo makefile: e.g. https://www.csee.umbc.edu/portal/help/nasm/Makefile_64

#!/bin/sh

autorun=false
project=''

assembler=nasm
linker=ld
#linker=ld.lld

usage() {
	echo "Usage: $0 -f PROJECT [-r]"
	exit 1
}

options=':f:r'
while getopts $options option
do
	case "$option" in
		f ) project=$OPTARG;;
		r ) autorun=true;;
		: ) echo "Missing arg for -$OPTARG" >&2; exit 1;;
		* ) echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
	esac
done

if [ $OPTIND -eq 1 ]; then
	echo "No options specified"
	usage;
fi

shift $(( OPTIND - 1 ))

echo -e "Building ${project} with:"
echo -e "\tProject: ${project} (${project}.asm)"
echo -e "\tAutorun: ${autorun}"
echo -e "\tAssembler: ${assembler}"
echo -e "\tLinker: ${linker}"

# check input file exists
if [ ! -f ${project}.asm ]; then
	echo "Could not find ${project}.asm"
	exit 1
fi

# assemble
#ASSEMBLE_CMD="${assembler} -f elf64 -g dwarf2 ${project}.asm"
ASSEMBLE_CMD="${assembler} -g -F dwarf -f elf64 ${project}.asm"
echo "Running '${ASSEMBLE_CMD}'"
`${ASSEMBLE_CMD}`
s=$?

if [ ! ${s} -eq 0 ]; then
	echo "Aborting due to assembler error"
	exit ${s}
fi

# link
# -s
LINK_CMD="${linker} -o ${project} ${project}.o"
echo "Running '${LINK_CMD}'"
`${LINK_CMD}`

# show files...
ls -hnug | grep ${project}

if [ ${autorun} = true ]; then
	echo "Running ${project}"
	./${project}
fi




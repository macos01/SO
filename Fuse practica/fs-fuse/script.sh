#!/bin/bash

MPOINT="./mount-point"
VIRTUAKDISK="virtual-disk"
let BLOCK_SIZE_BYTES=4096

echo ""
echo "Script test"
if [ -s "fs-fuse" ] && [ -x "fs-fuse" ]; then
echo "Fuse files found"
else 
echo "Necessary files not found"
exit 1
fi

if [ -s "my-fsck-static-64" ] && [ -x "my-fsck-static-64" ]; then
echo "my-fsck-static-64 found"
else 
echo "my-fsck-static-64 not found"
exit 1
fi


echo "Cleaning workspace"
read -p "Press enter..."

fusermount -u $MPOINT 2> /dev/null
if [ -d ./temp ]; then 
rm -r temp
fi

if [ -d $MPOINT ]; then 
rm -r mount-point
fi

if [ -s "$VIRTUAKDISK" ]; then 
rm virtual-disk
fi

echo "Executing fuse executing fuse on other terminal"
read -p "Press enter..."


mkdir mount-point
x-terminal-emulator -T fuse -e "bash -c \"./fs-fuse -t 2097152 -a virtual-disk -f '-d -s mount-point'; exec bash\""
# ./fs-fuse -t 2097152 -a virtual-disk -f '-d -s mount-point'

echo "Wait to fuse boot and press enter"
read -p "Press enter..."

echo "Using test files"
echo "Copying files to tmp and to mount-point"
read -p "Press enter..."
mkdir temp
cp ./src/fuseLib.c ./temp
cp ./src/myFS.h ./temp
cp ./src/fuseLib.c $MPOINT
cp ./src/myFS.h $MPOINT


echo "Use my-fsck-static-64.."
read -p "Press enter..."

./my-fsck-static-64 $VIRTUAKDISK

echo "Check diff..."
read -p "Press enter..."
diff ./temp/fuseLib.c $MPOINT/fuseLib.c
diff1=$?
diff ./temp/myFS.h $MPOINT/myFS.h
diff2=$?


if [ $diff1 -eq 0 ] && [ $diff2 -eq 0 ]; then
echo "CORRECT"
else 
echo "ERROR"
exit 1
fi

echo "Check file 1 diff with truncate -1 BLOCK"
truncate -s -$BLOCK_SIZE_BYTES ./temp/fuseLib.c 
truncate -s -$BLOCK_SIZE_BYTES $MPOINT/fuseLib.c 

echo "Use my-fsck-static-64.."
read -p "Press enter..."
./my-fsck-static-64 $VIRTUAKDISK

echo "Check diff..."
read -p "Press enter..."
diff ./temp/fuseLib.c $MPOINT/fuseLib.c
diff3=$?
diff ./temp/myFS.h $MPOINT/myFS.h
diff4=$?

if [ $diff3 -eq 0 ] && [ $diff4 -eq 0 ]; then
echo "CORRECT"
else 
echo "ERROR"
exit 1
fi

echo "Copy another file to fuse and check diff.."
seq 0 1 2500 > ./temp/fileseq
cp ./temp/fileseq $MPOINT

echo "Use my-fsck-static-64.."
read -p "Press enter..."
./my-fsck-static-64 $VIRTUAKDISK

echo "Check diff..."
read -p "Press enter..."
diff ./temp/fileseq $MPOINT/fileseq
diff5=$?

if [ $diff5 -eq 0 ]; then
echo "CORRECT"
else 
echo "ERROR"
exit 1
fi

echo "Check file 2 diff with truncate +3 BLOCKS"

let SIZE=$BLOCK_SIZE_BYTES*3

truncate -s +$SIZE ./temp/myFS.h 
truncate -s +$SIZE $MPOINT/myFS.h 

echo "Use my-fsck-static-64.."
read -p "Press enter..."
./my-fsck-static-64 $VIRTUAKDISK

echo "Check diff..."
read -p "Press enter..."
diff ./temp/myFS.h $MPOINT/myFS.h
diff6=$?

if [ $diff6 -eq 0 ]; then
echo "CORRECT"
else 
echo "ERROR"
exit 1
fi

echo "OK"
read -p "Press enter..."
exit 1








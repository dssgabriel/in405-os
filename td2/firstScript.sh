cd /tmp
mkdir --parents ./project/doc ./project/include ./project/src
echo "DOS SANTOS Gabriel" > ./project/README.md
touch ./project/include/func.h ./project/src/func.c ./project/src/main.c
ls --recursive ./project/ > ./project/contents.txt
cp --recursive ./project/ ./projectV2
rm --force --recursive ./project/
tar cvf pv2.tar projectV2/
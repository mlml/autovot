#!/bin/bash

##
## script to make lists of textgrids and wavs for (v1 of) the CAS data, and copy them to experiments/config
##
## To run:
## > chmod +x makeConfigFiles.sh
## > ./makeConfigFiles.sh
##
## NB: If there are 'new' files in the voiced and voiceless subdirs
## somewhere, have to delete them first.  (These are generated when
## running auto_vot_decode.py)
##
## MS, 1/2014
##

trainD="`pwd`/data/tutorialExample/train"
testD="`pwd`/data/tutorialExample/test"

configD="`pwd`/config"

vlTrTgF="${configD}/voicelessTrainTgList.txt"
vlTrWavF="${configD}/voicelessTrainWavList.txt"

vdTrTgF="${configD}/voicedTrainTgList.txt"
vdTrWavF="${configD}/voicedTrainWavList.txt"

vlTeTgF="${configD}/voicelessTestTgList.txt"
vlTeWavF="${configD}/voicelessTestWavList.txt"

vdTeTgF="${configD}/voicedTestTgList.txt"
vdTeWavF="${configD}/voicedTestWavList.txt"


## create voiceless & voiced training textgrid and wav lists

if [ -f $vlTrTgF ]
then
    rm $vlTrTgF
fi

for i in `find ${trainD}/voiceless/*.TextGrid`
do
    echo "`pwd`/$i" >> $vlTrTgF
done

if [ -f $vlTrWavF ]
then
    rm $vlTrWavF
fi

for i in `find ${trainD}/voiceless/*.wav`
do
    echo "`pwd`/$i" >> $vlTrWavF
done


if [ -f $vdTrTgF ]
then
    rm $vdTrTgF
fi

for i in `find ${trainD}/voiced/*.TextGrid`
do
    echo "`pwd`/$i" >> $vdTrTgF
done

if [ -f $vdTrWavF ]
then
    rm $vdTrWavF
fi

for i in `find ${trainD}/voiced/*.wav`
do
    echo "`pwd`/$i" >> $vdTrWavF
done

## create voiceless & voiced test textgrid and wav lists

if [ -f $vlTeTgF ]
then
    rm $vlTeTgF
fi

for i in `find ${testD}/voiceless/*.TextGrid`
do
    echo "`pwd`/$i" >> $vlTeTgF
#echo "`pwd`/test/wav/`basename ${i/.TextGrid/.wav}`" >> $vlTeWavF
done

if [ -f $vlTeWavF ]
then
rm $vlTeWavF
fi

for i in `find ${testD}/voiceless/*.wav`
do
    echo "`pwd`/$i" >> $vlTeWavF
done



if [ -f $vdTeTgF ]
then
    rm $vdTeTgF
fi

for i in `find ${testD}/voiced/*.TextGrid`
do
    echo "`pwd`/$i" >> $vdTeTgF
#echo "`pwd`/test/wav/`basename ${i/.TextGrid/.wav}`" >> $vdTeWavF
done

if [ -f $vdTeWavF ]
then
    rm $vdTeWavF
fi

for i in `find ${testD}/voiced/*.wav`
do
    echo "`pwd`/$i" >> $vdTeWavF
done

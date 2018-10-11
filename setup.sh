#/usr/bin/env bash -e

make

if [ -e "./action-app_respeaker" ]
then
    echo "compile done"
else
    echo "compile failed"
fi

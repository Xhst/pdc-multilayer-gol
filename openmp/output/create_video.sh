#!/bin/bash

FRAMERATE=20;
NAME="_output"

while getopts ":f:n:" opt; do
  case $opt in
    f)
      echo "Creating video with $OPTARG images per second"
      FRAMERATE=$OPTARG
      ;;
    n)
      NAME=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
    :)
      ;;
  esac
done

cd ../output/combined
ffmpeg -y -framerate $FRAMERATE -i combined%04d.png -c:v libx264 -pix_fmt yuv420p $NAME.mp4

cd ../dependent
ffmpeg -y -framerate $FRAMERATE -i dependent%04d.png -c:v libx264 -pix_fmt yuv420p $NAME.mp4

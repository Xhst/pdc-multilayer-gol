#!/bin/bash

FRAMERATE=20;

while getopts ":f:" opt; do
  case $opt in
    f)
      echo "Creating video with $OPTARG images per second"
      FRAMERATE=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
    :)
      ;;
  esac
done

cd ../output/combined
ffmpeg -y -framerate $FRAMERATE -i combined%04d.png -c:v libx264 -pix_fmt yuv420p _output_combined.mp4

cd ../dependent
ffmpeg -y -framerate $FRAMERATE -i dependent%04d.png -c:v libx264 -pix_fmt yuv420p _output_dependent.mp4

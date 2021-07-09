#!/bin/sh
PROJECT_DIR=$(dirname "$0")

if [ "$#" -ne 1 ]; then
  echo "Requires at least one file as asrgument!"
  exit 1
fi

echo "$(date +"%T"): Building '$1'..."

# We use a busy wait because it works on all systems,
# however the correct solution here would be fswatch/inotify etc.

# Busy wait for file change adapted from: https://superuser.com/a/634313
LTIME=`stat -c %Z $1`

./$PROJECT_DIR/emcc.sh $@

echo "Watching '$1' for changes..."

while true
do
  ATIME=`stat -c %Z $1`

  if [[ "$ATIME" != "$LTIME" ]]
  then
    echo "Rebuilding..."
    # echo "$(date +"%T"): Rebuilding..."

    ./$PROJECT_DIR/emcc.sh $@

    # echo "$(date +"%T"): Done"
    echo "Done"

    LTIME=$ATIME
  else
    sleep 1
  fi
done

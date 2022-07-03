#!/bin/bash

# Running this script will output the total number of lines tracked 
# by the repository. Just a fun thing to keep track of

git ls-files | xargs wc -l | tail -1
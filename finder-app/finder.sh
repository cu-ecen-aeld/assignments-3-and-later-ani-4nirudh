#!/usr/bin/sh

###### Finder App ######
# finder-app/finder.sh

filesdir=$1
searchstr=$2

# Exit if the number of enter arguments in less than 2
if [[ $# -lt 2 ]]; then
  echo -e "\nPlease enter 2 arguments! Exiting ...\n"
  exit 1
fi

# Checking if directory exists at path
if [[ ! -d ${filesdir} ]]; then
  echo -e "\nDirectory does not exist at the following path. Exiting ...\n"
  exit 1
fi

filecount=$(find ${filesdir} -type f | wc -l)
matchcount=$(grep -irn "${searchstr}" "${filesdir}" | wc -l)

echo -e "The number of files are ${filecount} and the number of matching lines are ${matchcount}"

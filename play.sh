#!/bin/bash
if [ 1 -eq 0 ] ; then
  echo Number of verses that includes 'love'
  grep -i love bible.txt | wc -l
  echo Number of verses that includes 'mercy'
  grep -i mercy bible.txt | wc -l
  echo Number of verses that includes both 'mercy' and 'love'
  grep -i mercy bible.txt | grep -i love | wc -l
fi

# Count word frequency http://stackoverflow.com/a/10552948
#cat bible.txt | (tr ' ' '\n' | sort | uniq -c | awk '{print $2"@"$1}')
# Remove first two columns http://stackoverflow.com/a/13446273
#cat bible.txt | ( cut -d " " -f 3- | tr ' ' '\n'  | tr -d '[:punct:]' | tr -d '\t' | tr -d  '[0-9]' | sort | uniq -c  | sort -k 1 -g | awk '{printf "-%s-%s-\n",$1,$2}')
# 1. remove first and second columns
# 2. Replace space with newline
# 3. Remove punctuations
# 4. remove tabs
# 5. remove numbers
# 6. sort
# 7. count
# 8. sort by frequency, and treat number string as number
cat bible.txt | ( cut -d " " -f 3- | tr ' [:punct:]\t[:digit:]\r' '\n' | grep -v '^$' |  sort | uniq -c  | sort -k 1 -g | awk '{printf "%s %s-\n",$1,$2}')
# 1. remove first and second columns
# 2. Replace space, punc, tabs, digits, CR with newline
# 3. remove empty lines
# 4. sort
# 5. count
# 6. sort by frequency, and treat number string as number

# Find words that are in /usr/share/dict/cracklib-small
cat bible.txt | ( cut -d " " -f 3- | tr ' [:punct:]\t[:digit:]\r' '\n' | grep -v '^$' |  sort | uniq | grep -f /usr/share/dict/cracklib-small)


# Use :.w !bash to run this line in bash

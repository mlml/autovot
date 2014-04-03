# This script changes a preset interval to a given string of text
# Use to quickly rename a specific interval on a specific tier for a large number of files
# Written by Eric Doty
# McGill University, November 2011

# Set the directory
sound_directory$ = "/Users/mcgillLing/3_MLML/autovot/autovot/data/test/voiceless/"

clearinfo

# Load the filenames of TextGrids in the directory
Create Strings as file list...  list 'sound_directory$'*.TextGrid
num_files = Get number of strings

# Load each TextGrid one by one
for i from 1 to num_files
	select Strings list
	object_name$ = Get string... 'i'
	Read from file... 'sound_directory$''object_name$'
	object_name$ = selected$("TextGrid")

# Edit this line to choose interval and text. The order is: tier, interval, text
# Changes text of given interval
	Set interval text... 3 2 vot
	Write to text file... 'sound_directory$''object_name$'.TextGrid
	
endfor

printline done

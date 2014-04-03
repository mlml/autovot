#########################################
## This script adds a new tier to an existing TextGrid
## and saves the new textgrids in an output folder
#########################################

form Make Selection
	comment Script must be in same folder as soundfiles and textgrids
	#comment Enter directory of TextGrids and directory for new TextGrids
	#sentence Directory /Users/speechlab/Dropbox/CAS/CAS_contrast-repeat/cas7/2_data/9_H1H2_Hand annotated/4_annotated_rest/TODO first3speakers copy/
	comment Enter the tier you'd like to delete
	positive DeleteTier
	comment Make new directory for original textgrids?
	boolean makeDirectory 1
endform

clearinfo

directory$ = ""

if makeDirectory
	createDirectory ("1_orig")
endif

# get the sound and textgrid files from the directory to work on

	Create Strings as file list...  list 'directory$'*.TextGrid
	number_files = Get number of strings

# iterate through the file list

	for j from 1 to number_files
		select Strings list
		current_token$ = Get string... 'j'
		Read from file... 'directory$''current_token$'
		object_name$ = selected$ ("TextGrid")
		printline 'object_name$'

		#Remove tier... deleteTier
		Set tier name... 3 vot

# Write and save
		select TextGrid 'object_name$'
		
		#system cp "'directory$''object_name'.TextGrid"  0_oldTextgrids/'object_name$'.TextGrid
				system cp "'directory$''object_name$'.TextGrid"  1_orig/'object_name$'.TextGrid
				Save as text file... 'object_name$'.TextGrid
				#system cp "'directory$''object_name$'.lab"  1_orig/'object_name$'.lab
				#system cp "'directory$''object_name$'.wav"  1_orig/'object_name$'.wav

		select all
     		minus Strings list
     		Remove

	endfor

select Strings list
Remove
print all done 

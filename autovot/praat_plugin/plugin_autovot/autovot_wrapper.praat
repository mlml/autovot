# Call AutoVOT Praat plugin to autopopulate a batch of TextGrids
#
# Input:
#	- TextGrid containing three tiers:
#		1. force-aligned word
#		2. force-aligned phone
#		3. labelled stop boundaries of segments of interest
#	- Corresponding .wav files
#		- .wav files and .TextGrid files have the same names
#	- Assumptions:
#		- there is one tier with labeled stop intervals and nothing else
#		- this is the tier that will be fed to autovot
#
# Output:
#	- TextGrids with an additional tier containing the AutoVOT predictions
#	- Optional additional tier for annotation (TODO)
#	- Optional embellishing of VOT labels. This option includes the following modifications:
#		- Relabels predictions as, e.g., B_vot, where the suffix corresponds to the original label
#		- Allows for option to copy original stop onset to AutoVOT tier
#		- Adds tier even if no AutoVOT predictions (to ensure TextGrids are consistent for later post-processing)
#		- Optional copying of the original stop onset boundary to the AutoVOT tier
#			- If the original stop onset occurs at the same time or later than the AutoVOT prediction,
#			  closure will be placed 1ms prior to VOT onset instead.
#
# Thea Knowles
# theaknow@buffalo.edu
# May 2020

form Run AutoVOT on all files in a directory
	comment This script calls AutoVOT on a TextGrid with force-aligned extracted stops.
	comment Assumptions:
	comment At least one tier contains stop boundary windows
	comment .wav and .TextGrid files have the same name
	comment Confirm the directories:
	sentence textgrid_directory test_data_in/
	sentence audio_directory test_data_in/
	sentence output_directory test_data_out/
	comment Check here if you would like to manually select a directory containing the files.
	boolean Manually_choose 0
	comment Would you like to embellish the output (e.g., relabel, add stop closure, etc)
	comment If so, the next screen will prompt you for more options
	boolean Embellish 1
	comment Check if you like the output of the algorithm printed to the Praat info window
	boolean Verbose 0
	comment Check if you would like to test this script on the first five files.
	boolean Test 1
endform

clearinfo


if manually_choose
	directory$ = chooseDirectory$ ("Choose the directory containing the input .wav and .TextGrid files")
	dir_in_tg$ = directory$
	dir_in_wav$ = directory$
	dir_out$ = "'directory$'/data_out"

	if directory$ == ""
		exit Script exited. You did not select folders.
	else
		dir_in_tg$ = dir_in_tg$ + "/"
		dir_in_wav$ = dir_in_wav$ + "/"
		dir_out$ = dir_out$ + "/"
	endif
else
	dir_in_tg$ = textgrid_directory$
	dir_in_wav$ = audio_directory$
	dir_out$ = output_directory$
endif

createDirectory ("'dir_out$'")


printline Running AutoVOT ver. 0.94.1
printline Input TextGrids located in:
printline 'tab$''dir_in_tg$'
printline Input Sounds located in:
printline 'tab$''dir_in_wav$'
printline Output TextGrids saved in:
printline 'tab$''dir_out$'
printline

if embellish
	beginPause: "Embellish output"
		comment: "Provide the tier number containing the segment info"
		positive: "segment_tier",1
		comment: "Would you like to relabel the autoVOT intervals based on their original text?"
		comment: "AutoVOT prediction labels will be appended with _vot by default (e.g., B --> B_vot)"
		boolean: "relabel",1
		sentence: "vot_suffix","_vot"
		comment: "Would you like to include the original stop closure boundary (e.g., force-aligned boundary)?"
		comment: "Closure intervals will be appended with _clo by default (e.g., B --> B_clo)"
		boolean: "include_onset",1
		sentence: "onset_suffix","_clo"
	endPause: "Next: AutoVOT parameters",1
else
	relabel = 0
	vot_suffix$ = "_vot"
	include_onset = 0
	onset_suffix$ = ""
endif


# AutoVOT Parameters
selected_channel = 1
min_vot_length = 5
max_vot_length = 500
vot_classifier_model$ = "models/vot_predictor.amanda.max_num_instances_1000.model"

beginPause: "AutoVOT"
	comment: "Enter the interval tier number with the stop boundaries:"
	positive: "Tier_number", 3
	comment: "Enter the interval mark (* = any string)"
	sentence: "Interval_mark", "*"

	optionMenu: "Channel:", selected_channel
	option: "mono"
	option: "left"
	option: "right"
	comment: "Algorithm parameters:"
	natural: "min vot length", 5
	natural: "max vot length", 500
	comment: "The script directory is:"
	comment: defaultDirectory$
	comment: "Training model files (relative to the above directory):"
	text: "vot classifier model", vot_classifier_model$
endPause: "Run",1

# Rename tier_number variable for readability later on
soi_tier = tier_number

Create Strings as file list... list 'dir_in_tg$'*.TextGrid

if test
	n_files = 5
else
	n_files = Get number of strings
endif

for file from 1 to n_files
	select Strings list
	current_token$ = Get string... 'file'
	Read from file... 'dir_in_tg$''current_token$'
	name$ = selected$ ("TextGrid")
	Read from file... 'dir_in_wav$''name$'.wav
	select TextGrid 'name$'
	n_tiers_original = Get number of tiers
	interval_tier$ = Get tier name... 'tier_number'


	@autovot: name$

	if embellish
		select TextGrid 'name$'_vad
		n_tiers = Get number of tiers
		skip = 0

		# First, add AutoVOT tier even when stops are not present (if no tier was added)
		if n_tiers == n_tiers_original
			skip = 1
			if verbose
				printline NO STOPS FOUND for 'name$'. AutoVOT tier will be empty.
			endif
			Insert interval tier... n_tiers+1 AutoVOT
		endif

		# Duplicate AutoVOT tier for adjustments
		select TextGrid 'name$'_vad
		n_tiers = Get number of tiers
		autovot_tier = n_tiers
		edit_tier = n_tiers + 1
		Duplicate tier... autovot_tier edit_tier AutoVOT-edit

		# Iterate over autoVOT predictions
		n_pred = Get number of intervals... 'autovot_tier'
		for pred_int from 1 to n_pred
			select TextGrid 'name$'_vad
			start = Get start time of interval... 'autovot_tier' 'pred_int'
			current_label$ = Get label of interval... 'autovot_tier' 'pred_int'

			# Relabel VOT predictions
			if relabel and skip==0

				# If labeled interval found on AutoVOT tier
				# segment_tier was defined by the user; autovot_tier was the last tier (before duplication); edit_tier is the current last tier
				if current_label$ <> ""
					soi_int = Get interval at time... 'segment_tier' 'start'+0.001
					soi_label$ = Get label of interval... 'segment_tier' 'soi_int'

					# Relabel VOT
					current_edit_int = Get interval at time... 'edit_tier' 'start'+0.001
					Set interval text... 'edit_tier' 'current_edit_int' 'soi_label$''vot_suffix$'
				endif
			endif

			if include_onset and skip==0
				if current_label$ <> ""
					select TextGrid 'name$'_vad
					soi_int = Get interval at time... 'segment_tier' 'start'+0.001
					soi_label$ = Get label of interval... 'segment_tier' 'soi_int'
					soi_start = Get start time of interval... 'segment_tier' 'soi_int'
					# If soi onset is at the same time or later than VOT, just set it one ms before VOT
					if soi_start >= start
						soi_start = start - 0.001
					endif

					if soi_start > 0
						Insert boundary... 'edit_tier' 'soi_start'
						onset_int = Get interval at time... 'edit_tier' 'soi_start'
						Set interval text... 'edit_tier' 'onset_int' 'soi_label$''onset_suffix$'
					else
						if verbose
							printline Stop closure boundary couldn't be added on 'name$' at interval 'edit_int'
						endif
					endif
				endif
			endif
		endfor
		# Remove AutoVOT tier because the AutoVOT-edit tier replaces it now.
		Remove tier... 'autovot_tier'
	endif

	@save: name$

endfor

date$ = date$ ()
month$ = mid$ (date$,5,3)
year$ = right$ (date$,4)
month_year$ = "'month$', 'year$'"


printline 'newline$'Files saved in 'dir_out$''newline$'
printline Citing AutoVOT:'newline$'
printline If possible to cite a program, the following format is recommended
printline (adjusting retrieval dates and versions as necessary):'newline$'
printline Keshet, J., Sonderegger, M., Knowles, T. (2014). 
printline AutoVOT: A tool for automatic measurement of voice onset time using discriminative 
printline structured prediction [Computer program]. 
printline Version 0.94.1, retrieved 'month_year$' from https://github.com/mlml/autovot/.
printline
printline If you are unable to cite the program itself, please cite the following paper:'newline$'
printline Sonderegger, M., & Keshet, J. (2012).
printline Automatic measurement of voice onset time using discriminative structured predictions. 
printline The Journal of the Acoustical Society of America, 132(6), 3965-3979.
printline
printline Latest release available at: https://github.com/mlml/autovot/.

#appendInfoLine: "Please cite:"
#appendInfoLine: "Sonderegger, M., & Keshet, J. (2012). Automatic measurement of "
#appendInfoLine: "voice onset time using discriminative structured predictions. The"
#appendInfoLine: "Journal of the Acoustical Society of America, 132(6), 3965-3979."
#appendInfoLine: " "
appendInfoLine: "Send comments to keshet@biu.ac.il"

select all
Remove

#system open 'dir_out$'



################################################################################
# PROCEDURES
################################################################################
procedure autovot: .name$
	# default behavior executes autovot on specified tier.
	# outputs new TextGrid to Object Window with suffix _vad
	select TextGrid '.name$'
	plus Sound '.name$'
include autovot_batch.praat
endproc


procedure save: .name$
	if verbose
		printline Saving '.name$'.TextGrid to: 'dir_out$'
		printline
		printline
	endif

	select TextGrid '.name$'_vad
	Write to text file... 'dir_out$''.name$'.TextGrid
	select all
	minus Strings list
	Remove
endproc

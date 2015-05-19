# auotovot predict VOTs by selecting a Sound object and a TextGrid object. 
# Then specify a tier in the TextGrid with intervals of the arround the stop consonants.
# 
# Written by Joseph Keshet (17 July 2014)
# joseph.keshet@biu.ac.il
#
# This script is free software: you can redistribute it and/or modify 
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# A copy of the GNU General Public License is available at
# <http://www.gnu.org/licenses/>.

clearinfo

writeInfoLine: "AutoVOT ver. 0.92"

# check OS X version
tmp_filename$ = temporaryDirectory$  + "/tmp.txt"
cmd_line$ = "uname -r > " + tmp_filename$
system 'cmd_line$'
os_version = readFile(tmp_filename$)
if os_version < 12.0
	appendInfoLine: "os_version=", os_version, " < 12.0"
	exitScript: "AutoVOT can run on OS X 10.8 or later."
endif

# save files to a temporary directory
if numberOfSelected ("Sound") <> 1 or numberOfSelected ("TextGrid") <> 1
    exitScript: Please select a Sound and a TextGrid first.
endif

# keep the selected object safe
sound = selected ("Sound")
textgrid = selected ("TextGrid")

# get tier names
selectObject: textgrid
num_tiers = Get number of tiers
for i from 1 to 'num_tiers'
	tier_name$[i] = Get tier name... 'i'
endfor

# get number of channels
selectObject: sound
nummebr_of_channels = Get number of channels

selectObject: sound, textgrid
selected_tier_name = 1
selected_channel = 1
min_vot_length = 5
max_vot_length = 500
vot_classifier_model$ = "models/vot_predictor.amanda.max_num_instances_1000.model"
beginPause: "AutoVOT"
	comment: "This is a script for automatic measurement of voice onset time (VOT)"
	comment: "Sound & TextGrid parameters:"
	optionMenu: "Interval tier", selected_tier_name
	for i from 1 to 'num_tiers'
		option: tier_name$[i]
	endfor
    word: "Interval mark", "* (= any string)"
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
clicked = endPause: "Cancel", "Next", 2, 1
if clicked <> 2
	exit
endif
#appendInfoLine: "selected_tier_name=", interval_tier$
#appendInfoLine: "min_vot_length=", min_vot_length
#appendInfoLine: "max_vot_length=", max_vot_length
#appendInfoLine: "model=", vot_classifier_model$
#appendInfoLine: "interval_mark=", interval_mark$
#appendInfoLine: "num_of_channels=", nummebr_of_channels
#appendInfoLine: "selected_channel=", channel$

selectObject: sound
sound_name$ = selected$( "Sound")
sound_filename$ = temporaryDirectory$ + "/" + sound_name$ + ".wav"
#appendInfoLine: "Saving ", name$, " as ", sound_filename$
if channel$ = "mono"
	converted_sound = Convert to mono
else
	converted_sound = Extract one channel... 'channel$'
endif
current_rate = Get sample rate
if current_rate <> 16000
	appendInfoLine: "Resampling Sound object to 16000 Hz."
	Resample... 16000 50
	Save as WAV file: sound_filename$
	Remove
else
	Save as WAV file: sound_filename$
endif
removeObject: 'converted_sound'

selectObject: textgrid
textgrid_name$ = selected$( "TextGrid")
textgrid_filename$ = temporaryDirectory$  + "/" + textgrid_name$ + ".TextGrid"
new_textgrid_filename$ = temporaryDirectory$ + "/" + textgrid_name$ + "_vad.TextGrid"
#appendInfoLine: "Saving ", name$, " as ", textgrid_filename$
Save as text file: textgrid_filename$
selectObject: sound, textgrid

# call vot prediction
log_filename$ = temporaryDirectory$  + "/cmd_line.log"
exec_name$ = "export PATH=$PATH:.;  auto_vot_decode.py "
exec_params$ = "--min_vot_length " +  string$(min_vot_length) 
exec_params$ = exec_params$ + " --max_vot_length " +  string$(max_vot_length) 
exec_params$ = exec_params$ + " --window_tier " + "'" + interval_tier$ + "'"
exec_params$ = exec_params$ + " --window_mark " + "'" + interval_mark$ + "'"
cmd_line$ = exec_name$ + exec_params$
cmd_line$ = cmd_line$ + " " + sound_filename$ + " " + textgrid_filename$ 
cmd_line$ = cmd_line$ + " " + vot_classifier_model$
cmd_line$ = cmd_line$ + " > " + log_filename$ + " 2>&1"
appendInfoLine: "Executing in the shell the following command:"
appendInfoLine: cmd_line$
system 'cmd_line$'
appendInfoLine: "Output:"
log_text$ = readFile$ (log_filename$)
appendInfoLine: log_text$
appendInfoLine: "Done."
appendInfoLine: " "
appendInfoLine: "Please cite:"
appendInfoLine: "Sonderegger, M., & Keshet, J. (2012). Automatic measurement of "
appendInfoLine: "voice onset time using discriminative structured predictions. The"
appendInfoLine: "Journal of the Acoustical Society of America, 132(6), 3965-3979."
appendInfoLine: " "
appendInfoLine: " send comments to keshet@biu.ac.il"

# read new TextGrid
system cp 'textgrid_filename$' 'new_textgrid_filename$'
Read from file... 'new_textgrid_filename$'
textgrid_obj_name$ = "TextGrid " + textgrid_name$ + "_vad"
selectObject: sound, textgrid_obj_name$

# remove unecessary files
deleteFile: sound_filename$
deleteFile: textgrid_filename$
deleteFile: new_textgrid_filename$

View & Edit
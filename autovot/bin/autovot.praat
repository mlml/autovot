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


# save files to a temporary directory
if numberOfSelected ("Sound") <> 1 or numberOfSelected ("TextGrid") <> 1
    exit Please select a Sound and a TextGrid first.
endif
form AutoVOT - automatic prediction of VOTs
    text Window_tier 'Processing Window'
endform
sound = selected ("Sound")
textgrid = selected ("TextGrid")
selectObject: sound
name$ = selected$( "Sound")
#appendInfoLine: "name=", name$
sound_filename$ = temporaryDirectory$ + name$ + ".wav"
#appendInfoLine: "Saving ", name$, " as ", sound_filename$
Save as WAV file: sound_filename$
selectObject: textgrid
name$ = selected$( "TextGrid")
#appendInfoLine: "name=", name$
textgrid_filename$ = temporaryDirectory$  + name$ + ".TextGrid"
new_textgrid_filename$ = temporaryDirectory$ + name$ + "_vad.TextGrid"
#appendInfoLine: "Saving ", name$, " as ", textgrid_filename$
Save as text file: textgrid_filename$
selectObject: sound, textgrid

# call vot prediction

exec_name$ = "export PATH=$PATH:.;  auto_vot_decode.py "
vot_classifier_model$ = "../models/vot_predictor.amanda.max_num_instances_1000.model"
exec_params$ = "--min_vot_length 5 --max_vot_length 500 --window_tier " + window_tier$
cmd_line$ = exec_name$ + exec_params$ + " " + sound_filename$ + " " + textgrid_filename$ + " " + vot_classifier_model$
writeInfoLine: "Predicting VOTs..."
system 'cmd_line$'
appendInfoLine: "Done."

# read new TextGrid
system cp 'textgrid_filename$' 'new_textgrid_filename$'
Read from file... 'new_textgrid_filename$'
sound_obj_name$ = "Sound " + name$
textgrid_obj_name$ = "TextGrid " + name$ + "_vad"
selectObject: sound_obj_name$, textgrid_obj_name$

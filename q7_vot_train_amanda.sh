 
# Before executing please fix path:
export PATH=$PATH:/Users/yossiadi/Projects/MLSPL/AutoVOT/autovot/bin

# stop if error
set -e


if [ ! $(which auto_vot_extract_features.py) ]; then
	echo "Error: Could not find scripts. Please add the auto_vot/bin directory to path. E.g.:"
	echo "   export PATH=\$PATH:/Users/jkeshet/Projects/vot_measurement/published_code/autovot/bin"
	exit
fi

if [ ! $(which VotTrain) ]; then
	echo "Could not find binaries. Please compile code. "
	exit
fi


if [ 1 = 1 ] ; then
	for i in 1 2 3 4 5 6 7 8 9 
	do
		auto_vot_extract_features.py --max_num_instances 1000 --vot_tier VOT \
			experiments/config/amanda_textgrids_only_speaker_$i.list \
			experiments/config/amanda_wavs_only_speaker_$i.list \
			experiments/config/amanda_fe_input_$i.txt \
			experiments/config/amanda_fe_features_$i.txt \
			experiments/config/amanda_fe_labels_$i.txt \
			/Users/yossiadi/Datasets/vot/amanda/features/
	done
fi

# if [ 1 = 1 ] ; then
# 	/bin/rm -f config/amanda_fe_features.txt config/amanda_fe_labels.txt 
# 	for i in 1 2 3 4 5 6 7 8 9 
# 	do
# 		auto_vot_append_files.py config/amanda_fe_features_$i.txt config/amanda_fe_labels_$i.txt \
# 			config/amanda_fe_features.txt config/amanda_fe_labels.txt 
# 	done
# fi


# if [ 1 = 1 ] ; then
# 	auto_vot_train_after_fe.py --log=INFO config/amanda_fe_features.txt config/amanda_fe_labels.txt  \
# 		models/vot_predictor.amanda.max_num_instances_1000.model
# fi


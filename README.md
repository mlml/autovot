AutoVOT, v. 0.9
=======
#### Note: for a quick-start first compile the code, then scroll down to the tutorial section.


## How to Cite AutoVOT in my articles?
***If possible to cite a program, the following format is recommended (adjusting retrieval dates and versions as necessary):***

* Keshet, J., Sonderegger, M., Knowles, T. (2014). AutoVOT: A tool for automatic measurement of voice onset time using discriminative structured prediction [Computer program]. Version 0.9, retrieved April 2014 from https://github.com/mlml/autovot/.

***If you are unable to cite the program itself, please cite the following paper:***

* Sonderegger, M., & Keshet, J. (2012). Automatic measurement of voice onset time using discriminative structured predictions. *The Journal of the Acoustical Society of America*, 132(6), 3965-3979.


## Acknowledgements
*To be filled in*
#### FFTReal
FFTReal, Version 1.02, 2001/03/27

Fourier transformation (FFT, IFFT) library specialised for real dataPortable ISO C++

Copyright (c) by Laurent de Soras <laurent.de.soras@club-internet.fr>

Object Pascal port (c) Frederic Vanmol <frederic@fruityloops.com>

##### get_f0, sigproc
get_f0.c estimates F0 using normalized cross correlation and dynamic programming.

sigproc.c is a collection of pretty generic signal-processing routines.

Copyright (c) 1990-1996 Entropic Research Laboratory, Inc. All rights reserved

This software has been licensed to the Centre of Speech Technology, KTH by Microsoft Corp. with the terms in the accompanying file BSD.txt, which is a BSD style license.

Written and revised by: Derek Lin and David Talkin



##### Meghan Clayards & Michael Wagner
Example data provided jointly by McGill University Speech Learning Lab
 and McGill University Prosody Lab.

## Out of the box: 

**Files included in this version:**

* **AutoVOT scripts:** `auto_vot/` contains all scripts necessary for user to extract features, train, and decode VOT measurements.
* **Tutorial examples:** 
	* `data/` contains the .wav and .TextGrid files used for training and testing, as well as `makeConfigFiles.sh`, a helper script used to generate file lists.
		* **Note:** This data contains short utterances with one VOT window per file. Future versions will contain examples with longer files and more instances of VOT per file.
	* `experiments/` is currently empty and will be used to store file lists, feature files, and classifers generated in the tutorial.
* **Example classifiers:** `examples/models/` contains three pre-trained classifiers that the user may use if they do not wish to provide their own training data. All example classifiers were used in Sonderegger&Keshet(2012) (above) and correspond to the Big Brother and Big Brother and Paterson/Goldrick Words datasets:
	* *Big Brother:* `bb_jasa.classifier`'s are trained on conversational British speech.  Word-initial voiceless stops were included in training. This classifier is best to use if working with *conversational speech*
	* *Paterson/Goldrick Words:* `nattalia_jasa.classifier` is trained on single-word productions from lab speech: L1 American English and L2 English/L1 Portuguese bilinguals. Word-initial voiceless stops were included in training. This classifier is best to use if working with *lab speech.*
	* **Note:** For best performance the authors recommend hand-labeling a small subset of VOTs (~100 tokens) from your own data and training new classifiers (see information on training below). 


## User provided files and directories
***Important:*** Input TextGrids will be overwritten. If you wish to access your original files, be sure to back them up elsewhere.

#### Sound file format
* Wav files sampled at 16kHz mono

#### TextGrid file format
* Saved as text files with .TextGrid extension
* TextGrids for training must contain a tier with hand measured vot intervals. These intervals must have a common text label, such as "vot."
* TextGrids for testing must contain a tier with window intervals indicating the position the algorithm should begin looking for VOT onset. These intervals must have a common label. For best performance the window intervals should:
	* contain no more than one stop consonant 
	* contain about 50 msec before the beginning of the burst *or*
	* if only force-aligned segments are available, contain about 30 msec before the beginning of the burst.

#### Directory format
User provided directories should be in the autovot master directory.

*See example data & experiment folders.*

* `data/train/`: Each embedded folder should contain both TextGrids and corresponding wav files
* `data/test/`: Each embedded voiced/voiceless folder should contain only TextGrids.
	* Wav files should be in separate folder.
* `experiments/config/`: Lists of filenames. See below for more info.
* `experiments/models/`: Empty. This is where classifiers will be stored.
* `experiments/tmp_dir/`: Empty. This is where feature extraction will be stored in Mode 2.
    
# Getting Started

#### Note: For a quick-start, skip to the tutorial section below after compiling.

## Compiling

Navigate to auto_vot/code/ and run:

        $ make clean

If successful, the final line of the output should be:

        [make] Cleaning completed

Then, run:

        $ make

Final line of the utput should be:

        [make] Compiling completed
    

Finally, set the path:

	$ export PATH=$PATH:"$(dirname `pwd`)/bin"
        
        
TODO: Run:
        $ make install

# Usage
***Tutorial to follow***

### AutoVOT allows for two modes of feature extraction:

* **Mode 1 - Covert feature extraction:** The handling of feature extraction is hidden. The training can get cross-validation set or automatically select 20% of the training data.
* **Mode 2 - Features extracted to a known directory:** Training and decoding is done after feature extraction. Features are extracted to a known directory once after which training and decoding may be done as needed. Mode 2 is recommended if you have a large quantity of data.

## Feature extraction and training

#### Mode 1:
##### *Training of AutoVOT*
###### Usage: auto\_vot\_train.py [OPTIONS] wav\_list textgrid\_list model_filename 

    Positional Arguments:                   Description:
    
    wav_list                                list of WAV files
    textgrid_list                           list of manually labeled TextGrid files
    model_filename                          output model file name

    Optional arguments:                     Description
    
    -h, --help                              show this help message and exit
    --vot_tier VOT_TIER                     name of the tier to extract VOTs from
    --vot_mark VOT_MARK                     VOT mark value (e.g., "pos", "neg") or "*" for any
                                            string
    --window_min WINDOW_MIN                 window left boundary (in msec) relative to the VOT
                                            right boundary (usually should be negative, that is,
                                            before the VOT right boundary.)
    --window_max WINDOW_MAX                 window right boundary (in msec) relative to the VOT
                                            right boundary ( usually should be positive, that is,
                                            after the VOT left boundary.)
    --cv_auto                               use 20% of the training set for cross validation
    --cv_textgrid_list CV_TEXTGRID_LIST     list of manually labeled TextGrid files for cross-
                                            validation
    --cv_wav_list CV_WAV_LIST               list of WAV files for cross-validation
    --max_num_instances MAX_NUM_INSTANCES   max number of instances per file to use (default is to
                                            use everything)
    --logging_level LOGGING_LEVEL           print out level (DEBUG, INFO, WARNING or ERROR)
                

#### Mode 2: 
##### *Extract acoustic features of AutoVOT*
###### Usage: auto\_vot\_extract\_features.py [OPTIONS] textgrid\_list wav\_list input\_filename features\_filename labels\_filename features_dir
    

    Positional Arguments:                       Description:
    
    textgrid_list                               list of manually labeled TextGrid files (input)
    wav_list                                    list of WAV files (input)
    input_filename                              AutoVOT front end input file name (output)
    features_filename                           AutoVOT front end features file name (output)
    labels_filename                             AutoVOT front end labels file name (output)
    features_dir                                directory to put the feature files (output)

    Optional Arguments:                         Description:
    
    -h, --help                                  show this help message and exit
    --decoding                                  Extract features for decoding based on window_tier
                                                (vot_tier is ignored), otherwise extract features for
                                                training based on manual labeling given in the
                                                vot_tier
    --vot_tier VOT_TIER                         name of the tier to extract VOTs from
    --vot_mark VOT_MARK                         VOT mark value (e.g., "pos", "neg") or "*" for any
                                                string
    --window_tier WINDOW_TIER                   used this window as a search window for training. If
                                                not given, a constant window with parameters
                                                [window_min, window_max] around the manually labeled
                                                VOT will be used
    --window_mark WINDOW_MARK                   window mark value or "*" for any string
    --window_min WINDOW_MIN                     window left boundary (in msec) relative to the VOT
                                                right boundary (usually should be negative, that is,
                                                before the VOT right boundary.)
    --window_max WINDOW_MAX                     window right boundary (in msec) relative to the VOT
                                                right boundary (usually should be positive, that is,
                                                after the VOT left boundary.)
    --max_num_instances MAX_NUM_INSTANCES       max number of instances per file to use (default is to
                                                use everything)
    --logging_level LOGGING_LEVEL               print out level (DEBUG, INFO, WARNING or ERROR)
                  

##### *Train AutoVOT after features extraction*
###### Usage: auto\_vot\_train\_after\_fe.py [OPTIONS] features\_filename labels\_filename model\_filename

    Positional Arguments:                       Description:
    
    features_filename                           AutoVOT front end features file name (training)
    labels_filename                             AutoVOT front end labels file name (training)
    model_filename                              output model file name

    Optional arguments:                         Description:
    -h, --help                                  show this help message and exit
    --logging_level LOGGING_LEVEL               print out level (DEBUG, INFO, WARNING or ERROR)


## VOT DECODING

#### Mode 1
##### *Decode AutoVOT*
###### Usage: auto\_vot\_decode.py [OPTIONS] wav\_filename textgrid\_filename model\_filename

    Positional arguments:                       Description:
    
    wav_filenames                               a list of WAV files.
    textgrid_filenames                          a list of TextGrid files.
    model_filename                              output model file name

    Optional arguments:                         Description:
    -h, --help                                  show this help message and exit
    --vot_tier VOT_TIER                         name of the tier to extract VOTs from
    --vot_mark VOT_MARK                         VOT mark value (e.g., "pos", "neg") or "*" for any
                                                string
    --window_tier WINDOW_TIER                   used this window as a search window for training. If
                                                not given, a constant window with parameters
                                                [window_min, window_max] around the manually labeled
                                                VOT will be used
    --window_mark WINDOW_MARK                   window mark value or "*" for any string
    --window_min WINDOW_MIN                     window left boundary (in msec) relative to the VOT
                                                right boundary (usually should be negative, that is,
                                                before the VOT right boundary.)
    --window_max WINDOW_MAX                     window right boundary (in msec) relative to the VOT
                                                right boundary (usually should be positive, that is,
                                                after the VOT left boundary.)
    --min_vot_length MIN_VOT_LENGTH             minimum allowed length of predicted VOT (in msec) in decoding
    --max_vot_length MAX_VOT_LENGTH             maximum allowed length of predicted VOT (in msec) in
                                                decoding
    --max_num_instances MAX_NUM_INSTANCES       max number of instances per file to use (default is to
                                                use everything)
    --ignore_existing_tiers                     add a new AutoVOT tier to output textgrids, even if
                                                one already exists
    --logging_level LOGGING_LEVEL               print out level (DEBUG, INFO, WARNING or ERROR)
    

#### Mode 2
##### *Decode AutoVOT after features extraction*
###### Usage: auto_vot_decode_after_fe.py [OPTIONS] features_filename labels_filename model_filename

    Positional arguments:                       Description:
    features_filename                           AutoVOT front end features file name (training)
    labels_filename                             AutoVOT front end labels file name (training)
    model_filename                              output model file name

    Optional arguments:                         Description:
    -h, --help                                  show this help message and exit
    --logging_level LOGGING_LEVEL               print out level (DEBUG, INFO, WARNING or ERROR)


## Check Performance
##### *Compute the performance between a set of predicted textgrid files and a set of
labeled textgrid files.*
###### Usage: auto_vot_performance.py [OPTIONS] labeled_textgrid_list predicted_textgrid_list labeled_vot_tier predicted_vot_tier

    Positional arguments:                       Description:
    
    labeled_textgrid_list                       list of manually labeled TextGrid files
    predicted_textgrid_list                     list of predicted TextGrid files (can be the same as
                                                labeled_textgrid_list when the same TextGrid is used
                                                with two different tiers to compare)
    labeled_vot_tier                            name of the VOT tier of the manual labeled TextGrids
    predicted_vot_tier                          name of the VOT tier to predicted TetGrids

    Optional arguments:                         Description:
    
    -h, --help                                  show this help message and exit
    --debug                                     verbose printing
    
# Tutorial
### Arguments used in this example:
* **TextGrid labels** are all `vot`. This includes tier names and window labels.
* **File lists** will be generated in the first step of the tutorial and will be located in `experiments/config/`.
* **Classifier files** will be generated during training and will be located in `experiments/models/`.
* **Feature file lists** will be generated during feature extraction in Mode 2 and will be located in `experiments/config/`. These include AutoVOT Front End input, feature, and label files.
* **Feature files** will be generated during feature extraction in Mode 2 and will be located in `experiments/tmp_dir/`.


## Getting started
### Make sure directories are properly set up
* `experiments/` should contain three subfolders, all of which are currently empty: `config`, `models`, `tmp_dir`
* `data/` should contain two subfolders: `test` and `train`
	* `train/` should contain two subfolders: `voiced` and `voiceless`, each with wav files and corresponding TextGrids
		* The TextGrids contain 3 tiers, one of which will be used by autovot. The tiers are `phones`, `words`, and `vot`. The `vot` tier contains manually aligned VOT intervals that are labeled "vot"
	* `test/` should contain three subfolders: `voiced`, `voiceless`, each containing TextGrids, and `wav`, which should contain all wav files for testing (both voiced and voiceless)
	* All wav files are sampled at 16kHz.
	* All textgrids have been saved as text files.

### Generate file lists
The user may also provide their own file lists if they prefer.

* Navigate to data and run: 

	`$ ./makeConfigFiles.sh`

* `experiments/config` should now contain 8 files containing lists of the testing/training files, as listed above.


## Mode 1
### Training
**Note** that for Mode 1, feature extraction is hidden to the user as a component of auto_vot_train.py.

Navigate to `experiments/` and run the following:

*Note: `\` indicate line breaks that should not be included in the actual command line prompt.*

For voiceless data:

	auto_vot_train.py --vot_tier vot --vot_mark vot --max_num_instances 1 \
	config/voicelessTrainWavList.txt config/voicelessTrainTgList.txt \
	models/VoicelessModel.classifier
	
For voiced data:

	auto_vot_train.py --vot_tier vot --vot_mark vot --max_num_instances 1 \
	config/voicedTrainWavList.txt config/voicedTrainTgList.txt \
	models/VoicedModel.classifier

If sucessful, you'll see which files have been processed in the command line output. The final output should indicate that all steps have been completed:

`[VotFrontEnd2] INFO: Processing 75 files.`
`[VotFrontEnd2] INFO: Features extraction completed.`
`[InitialVotTrain] INFO: Training completed.`
`[auto_vot_train.py] INFO: All done.`
	
You'll also see that classifier files have been generated in the `models` folder (2 for voiceless and 2 for voiced).

### Decoding
**Note for voiced data:** When predicting VOT, the default minimum length is 15ms. For English voiced stops this is too high, and must be adjusted in the optional parameter settings during this step.

Still from within `experiments/`, run the following:

For voiceless:

	auto_vot_decode.py --vot_tier vot  --vot_mark vot --max_num_instances 1 \
	config/voicelessTestWavList.txt config/voicelessTestTgList.txt \
	models/VoicelessModel.classifier
	
For voiced:

	auto_vot_decode.py --vot_tier vot  --vot_mark vot --min_vot_length 5 \
	--max_vot_length 100 --max_num_instances 1 config/voicelessTestWavList.txt \
	config/voicelessTestTgList.txt models/VoicelessModel.classifier

If successful, you'll see information printed out about how many examples in each file were successfully decoded. After all files have been processed, you'll see the message:

`[auto_vot_decode.py] INFO: All done.`

If there were any problematic files that couldn't be processed, these will appear at the end, with the message:

`[auto_vot_train.py] WARNING: Look for lines beginning with WARNING or ERROR in the program's output to see what went wrong.`

You can then look at your textgrids, which will have a new tier AutoVOT with predicted VOT intervals. These intervals are labeled with the algorithm's confidence in the predictions. A higher number indicates a more confident prediction.

## Mode 2
**Recommended for large datasets**

### Extracting acoustic features
Navigate to `experiments/` and run:

For voiceless:

	auto_vot_extract_features.py --log=INFO --vot_tier vot --vot_mark vot \
	config/voicelessTrainTgList.txt config/voicelessTrainWavList.txt \
	config/VoicelessFeInput.txt config/VoicelessFeFeatures.txt \
	config/VoicelessFeLabels.txt tmp_dir
	
For voiced: 

	auto_vot_extract_features.py --log=INFO --vot_tier vot --vot_mark vot \
	config/voicedTrainTgList.txt config/voicedTrainWavList.txt \
	config/VoicedFeInput.txt config/VoicedFeFeatures.txt \
	config/VoicedFeLabels.txt tmp_dir

If successful, you'll be able to see how many files are being processed and whether extraction was completed:

`[VotFrontEnd2] INFO: Processing 75 files.`
`[VotFrontEnd2] INFO: Features extraction completed.`

Feature matrix files will appear in the given directory (`tmp_dir` in this example) and can be used in future training/decoding sessions without having to be reextracted. This is the recommended mode of operation if you have a large quantity of data. Feature extraction can be time consuming, and only needs to be done once. Training and decoding are faster and allow for the user to tune parameters. External feature extraction allows you to tune these parameters as necessary without recomputing features.

### Training
From within `experiments/` run the following:

For voiceless:

	auto_vot_train_after_fe.py --log=INFO config/VoicelessFeFeatures.txt \
	config/VoicelessFeLabels.txt models/VoicelessModel_ver2.classifier
	
For voiced:

	auto_vot_train_after_fe.py --log=INFO config/VoicedFeFeatures.txt \
	config/VoicedFeLabels.txt models/VoicedModel_ver2.classifier

If training is successful classifier files will be generated in `experiments/models/` and you will see the following output command line message upon completion:

`[InitialVotTrain] INFO: Training completed.`

### Decoding
**Note:** If you wish to produce predictions in the TextGrids, you may perform Mode 1 decoding using the classifiers produced from Mode 2 training.

For voiceless:

	auto_vot_decode_after_fe.py --log=INFO config/VoicelessFeFeatures.txt \
	config/VoicelessFeLabels.txt models/VoicelessModel_ver2.classifier
	
For voiced:

	auto_vot_train_after_fe.py --log=INFO --min_vot_length 5 \
	--max_vot_length 100 config/VoicedFeFeatures.txt \
	config/VoicedFeLabels.txt models/VoicedModel_ver2.classifier

If decoding is successful you'll see the following output message: 

`[InitialVotDecode] INFO: Decoding completed.`

As with Mode 1, if there were any problematic files that couldn't be processed, these will appear at the end, with the message:

`[auto_vot_train.py] WARNING: Look for lines beginning with WARNING or ERROR in the program's output to see what went wrong.`



## Possible errors and warnings
### After running auto_vot_train.py:

##### Short VOT in training data
If you have shorter instances of VOT in your training data, you may get the following error:

`[InitialVotTrain] WARNING: Hinge loss is less than zero. This is due a short VOT in training data.`

You can ignore this, but be aware that a VOT in the training data was skipped.

##### Missing files
If you do not have a corresponding wav file for a TextGrid:
`[auto_vot_extract_features.py] ERROR: Number of TextGrid files should match the number of WAVs`

### After running auto_vot_decode.py:

##### AutoVOT tier already exists
`[auto_vot_decode.py] WARNING: File *filename*.TextGrid already contains a tier with the name "AutoVOT"`

`[auto_vot_decode.py] WARNING: New "AutoVOT" tier is NOT being written to the file.`

`[auto_vot_decode.py] WARNING: (use the --ignore_existing_tiers flag if you'd like to do so)`


If you've used --ignore_existing_tiers flag, you'll be reminded that an AutoVOT tier exists already:
`[auto_vot_decode.py] WARNING: Writing a new AutoVOT tier (in addition to existing one(s))`


### Other

##### Wrong file format
If one of your files does not have the right format, the following error will appear:

`[auto_vot_extract_features.py] ERROR: *filename*.wav is not a valid WAV.`
`[auto_vot_extract_features.py] ERROR: *filename*.TextGrid is not a valid TextGrid.`




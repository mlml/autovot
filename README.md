AutoVOT
=======

Joseph Keshet (joseph.keshet@biu.ac.il)             
Morgan Sonderegger (morgan.sonderegger@mcgill.ca)    
Thea Knowles (thea.knowles@gmail.com)

Autovot is a software package for automatic measurement of voice onset time (VOT), using an algorithm which is trained to mimic VOT measurement by human annotators. It works as follows:
* The user provides wav files containing a number of stop consonants, and corresponding Praat TextGrids containing some information about roughly where each stop consonant is located.
* A classifier is used to find the VOT for each stop consonant, and add a new tier to each TextGrid containing these measurements.
* The user can either use a pre-existing classifier, or (recommended) train a new one using a small number (~100) of manually-labeled VOTs from their own data.

This is a beta version of Autovot. Any reports of bugs, comments on how to improve the software or documentation, or questions are greatly appreciated, and should be sent to the authors at the addresses given above.

---

**For a quick-start,** first [download and compile the code](#settingup) then go to the [tutorial](#tutorial) section to begin.

<a name="toc"/>

##### Table of Contents

**1.** [Setting up](#settingup)

**1.1** [Praat plugin installation](#praatsetup)

**1.2** [Command line installation](#commandlinesetup)

**2.** [Usage](#usage)

**3.** [Tutorial](#tutorial)

**3.1** [Praat plugin tutorial](#praattutorial)

**3.2** [Command line tutorial](#cltutorial)

**4.** [Citing AutoVOT](#citing)

**5.** [Acknowledgements](#acknowledgements)


<a name="settingup"/>

# Setting Up

## Dependencies
In order to use AutoVOT you'll need the following installed in addition to the source code provided here:
* [GCC, the GNU Compiler Collection](http://gcc.gnu.org/install/download.html)
* [Python 2.7 or 3)](https://www.python.org/downloads/)
* To install python dependences, please run the command `pip -r "requirements.txt"` from the main directory of the repository
* If you're using Mac OS X you'll need to download GCC, as it isn't installed by default.  You can either:
	* Install [Xcode](http://itunes.apple.com/us/app/xcode/id497799835?ls=1&mt=12), then install Command Line Tools using the Components tab of the Downloads preferences panel.
	* Download the [Command Line Tools for Xcode](http://developer.apple.com/downloads) as a stand-alone package.

  You will need a registered Apple ID to download either package.

## Downloading and Installing
*[What is included in the download?](#outofthebox)*

<a name="praatsetup"/>

### Praat plugin installation

Download the latest Praat plugin installer from the [releases page](https://github.com/mlml/autovot/releases)

Double click on the installer icon and follow the directions.

**Quick-start:** *Bring me to the* ***[tutorial](#praattutorial)***

*Back to [top](#toc)*

<a name="commandlinesetup"/>

### Command line installation

#### Please note: 
* For a quick-start, skip to the [tutorial section](#tutorial) below *after [compiling](#compiling)*.
* All commands in this readme should be executed from the command line on a Unix-style system (OS X or Linux).
* All commands for AutoVOT Version 0.91 have been tested on OS X Mavericks only.
* Any feedback is greatly appreciated!

***AutoVOT is available to be cloned from Github, which allows you to easily have access to any future updates.***

The code to clone AutoVOT is: 

	$ git clone https://github.com/mlml/autovot.git

When updates become available, you may navigate to the directory and run:
	
	$ git pull origin master

If you are new to Github, check out the following site for helpful tutorials and tips for getting set up:

https://help.github.com/articles/set-up-git

***Alternatively, you can download the current version of AutoVOT as a [zip file](https://github.com/mlml/autovot/archive/master.zip), in which case you will not have access to future updates without re-downloading the updated version.***

### Compiling
**Note:** While you only have to clean and compile once, you will have to add the path to `code` to your `experiments` path *every time you open a new terminal window.*

Clean and compile from the `code` directory:
	
	$ cd autovot/autovot/code
        $ make clean

If successful, the final line of the output should be:

        [make] Cleaning completed

Then, run:

        $ make

Final line of the output should be:

        [make] Compiling completed
    



Finally, add the path to `code` to your `experiments` path:

If not working out of the given `experiments` directory, you must add the path to your intended working directory.

***IMPORTANT: YOU MUST ADD THE PATH EVERY TIME YOU OPEN A NEW TERMINAL WINDOW***


	$ cd ../../experiments
	$ export PATH=$PATH:/[YOUR PATH HERE]/autovot/autovot/bin
	
	For example:
	$ export PATH=$PATH:/Users/mcgillLing/3_MLML/autovot/autovot/bin

**Quick-start:** *Bring me to the* ***[tutorial](#cltutorial)***

*Back to [top](#toc)*

<a name="outofthebox"/>

## Out of the box:

**Files included in this version:**

* **AutoVOT scripts:** `autovot/` contains all scripts necessary for user to extract features, train, and decode VOT measurements.
* **Tutorial example data:** 
	* `experiments/data/tutorialExample/` contains the .wav and .TextGrid files used for training and testing, as well as `makeConfigFiles.sh`, a helper script used to generate file lists.
		* **Note:** This data contains short utterances with one VOT window per file. Future versions will contain examples with longer files and more instances of VOT per file.
		* The TextGrids contain 3 tiers, one of which will be used by autovot. The tiers are `phones`, `words`, and `vot`. The `vot` tier contains manually aligned VOT intervals that are labeled "vot"

* **Example classifiers:** `experiments/models/` contains three pre-trained classifiers that the user may use if they do not wish to provide their own training data. All example classifiers were used in [Sonderegger & Keshet (2012)](#howtocite) and correspond to the Big Brother and PGWords datasets in that paper:
	* *Big Brother:* `bb_jasa.classifier`'s are trained on conversational British speech.  Word-initial voiceless stops were included in training. This classifier is best to use if working with *conversational speech*
	* *PGWords:* `nattalia_jasa.classifier` is trained on single-word productions from lab speech: L1 American English and L2 English/L1 Portuguese bilinguals. Word-initial voiceless stops were included in training. This classifier is best to use if working with *lab speech.*
	* **Note:** For best performance the authors recommend hand-labeling a small subset of VOTs (~100 tokens) from your own data and training new classifiers (see information on training below). Experiments suggesting this works better than using a classifier pre-trained on another dataset are given in [Sonderegger & Keshet (2012)](#howtocite).


## User provided files and directories
***Important:*** Input TextGrids will be overwritten. If you wish to access your original files, be sure to back them up elsewhere.

#### Sound file format
* Wav files sampled at 16kHz mono
	* You can convert wav files using a utility such as [SoX](http://sox.sourceforge.net/), as follows:

# 
			$ sox input.wav  -c 1 -r 16000 output.wav
        

#### TextGrid file format
* Saved as text files with .TextGrid extension
* TextGrids for training must contain a tier with hand measured vot intervals. These intervals must have a common text label, such as "vot".
* TextGrids for testing must contain a tier with window intervals indicating the range of times where the algorithm should look for the VOT onset. These intervals must also have a common label, such as "window". For best performance the window intervals should:
	* contain no more than one stop consonant 
	* contain about 50 msec before the beginning of the burst *or*
	* if only force-aligned segments are available (each corresponding to an entire stop), contain about 30 msec before the beginning of the segment.

#### Directory format
The `experiments` folder contains subdirectories that will be used to store files generated by the scripts, in addition to data to be used during the working [tutorial](#tutorial). 

(*See example data & experiment folders.*)

* `experiments/config/`: Currently empty: This is where lists of file names will be stored.
* `experiments/models/`: Currently contains example classifiers. This is also where your own classifiers will eventually be stored.
* `experiments/tmp_dir/`: Currently empty. This is where feature extraction will be stored in Mode 2.
* `experiments/data/tutorialExample/`: This contains TextGrids and wav files for training and testing during the [tutorial](#tutorial).

*Back to [top](#toc)*

<a name="usage"/>

# Usage

*Back to [top](#toc)*

***Tutorial to follow***

### AutoVOT allows for two modes of feature extraction:

* **Mode 1 - Covert feature extraction:** The handling of feature extraction is hidden. When training a classifier sing these features, a cross-validation set can be specified, or a random 20% of the training data will be used. The output consists of modified TextGrids with a tier containing VOT prediction intervals.
* **Mode 2 - Features extracted to a known directory:** Training and decoding is done after feature extraction. Features are extracted to a known directory once after which training and decoding may be done as needed. The output consists of the prediction performance summary. Mode 2 is recommended if you have a large quantity of data. 


***Note: All help text may also be viewed from the command line for each .py program using the flag -h***

	For example:
	auto_vot_performance.py -h


## Feature extraction and training

#### Mode 1:
##### *Train a classifier to automatically measure VOT, using manually annotated VOTs in a set of textgrids and corresponding wav files. See [tutorial](#tutorial) for usage examples.*
###### Usage: auto\_vot\_train.py [OPTIONS] wav\_list textgrid\_list model_filename 

#

	Positional arguments:		Function:
	wav_list              Text file listing WAV files
	textgrid_list         Text file listing corresponding manually labeled
                        TextGrid files
	model_filename        Name of classifiers (output)

	Optional arguments:		Function:
	-h, --help            show this help message and exit
  	--vot_tier VOT_TIER   Name of the tier to extract VOTs from (default: vot)
  	--vot_mark VOT_MARK   Only intervals on the vot_tier with this mark value
                        (e.g. "vot", "pos", "neg") are used for training, or
                        "*" for any string (this is the default)
  	--window_min WINDOW_MIN
                        Left boundary of the window (in msec) relative to the
                        VOT interval's right boundary. Usually should be
                        negative, that is, before the VOT interval's left
                        boundary. (default: -50)
  	--window_max WINDOW_MAX
                        Right boundary of the window (in msec) relative to the
                        VOT interval's right boundary. Usually should be
                        positive, that is, after the VOT interval's right
                        boundary. (default: 800)
  	--cv_auto             Use 20% of the training set for cross-validation
                        (default: don't do this)
  	--cv_wav_list CV_WAV_LIST
                        Text file listing WAV files for cross-validation
                        (default: none)
  	--cv_textgrid_list CV_TEXTGRID_LIST
                        Text file listing corresponding manually labeled
                        TextGrid files for cross-validation (default: none)
  	--max_num_instances MAX_NUM_INSTANCES
                        Maximum number of instances per file to use (default:
                        use everything)
  	--logging_level LOGGING_LEVEL
                        Level of verbosity of information printed out by this
                        program (DEBUG, INFO, WARNING or ERROR), in order of
                        increasing verbosity. See
                        http://docs.python.org/2/howto/logging for
                        definitions. (default: INFO)
                

#### Mode 2: 
##### *Extract acoustic features for AutoVOT. To be used before auto_vot_train_after_fe.py or auto_vot_decode_after_fe.py*
###### Usage: auto\_vot\_extract\_features.py [OPTIONS] textgrid\_list wav\_list input\_filename features\_filename labels\_filename features_dir

	Positional arguments:		Function:
	textgrid_list         File listing TextGrid files containing stops to
                        extract features for (input)
	wav_list              File listing corresponding WAV files (input)
	input_filename        Name of AutoVOT front end input file (output)
	features_filename     Name of AutoVOT front end features file (output)
	labels_filename       Name of AutoVOT front end labels file (output)
	features_dir          Name of AutoVOT directory for output front end feature
                        files

	Optional arguments:		Function:
	-h, --help            show this help message and exit
	--decoding            Extract features for decoding based on window_tier
                        (vot_tier is ignored), otherwise extract features for
                        training based on manual labeling given in the
                        vot_tier
	--vot_tier VOT_TIER   Name of the tier containing manually labeled VOTs to
                        compare automatic measurements to (optional. default
                        is none)
	--vot_mark VOT_MARK   On vot_tier, only intervals with this mark value (e.g.
                        "vot", "pos", "neg") are used, or "*" for any string
                        (this is the default)
	--window_tier WINDOW_TIER
                        Name of the tier containing windows to be searched as
                        possible starts of the predicted VOT (default: none).
                        If not given *and* vot_tier given, a window with
                        boundaries window_min and window_max to the left and
                        right of the manually labeled VOT will be used . NOTE:
                        either window_tier or vot_tier must be specified. If
                        both are specified, window_tier is used, and
                        window_min and window_max are ignored.
	--window_mark WINDOW_MARK
                        VOT is only predicted for intervals on the window tier
                        with this mark value (e.g. "vot", "pos", "neg"), or
                        "*" for any string (this is the default)
	--window_min WINDOW_MIN
                        window left boundary (in msec) relative to the VOT
                        right boundary (usually should be negative, that is,
                        before the VOT right boundary.)
	--window_max WINDOW_MAX
                        Right boundary of the window (in msec) relative to the
                        VOT interval's right boundary. Usually should be
                        positive, that is, after the VOT interval's right
                        boundary. (default: 800)
	--max_num_instances MAX_NUM_INSTANCES
                        Maximum number of instances per file to use (default:
                        use everything)
	--logging_level LOGGING_LEVEL
                        Level of verbosity of information printed out by this
                        program (DEBUG, INFO, WARNING or ERROR), in order of
                        increasing verbosity. See
                        http://docs.python.org/2/howto/logging for
                        definitions. (default: INFO)
                  

##### *Train a classifier to automatically measure VOT, using manually annotated VOTs for which features have already been extracted using auto_vot_extract_features.py, resulting in a set of feature files and labels.*
###### Usage: auto\_vot\_train\_after\_fe.py [OPTIONS] features\_filename labels\_filename model\_filename

	Positional arguments:		Function:
  	features_filename     AutoVOT front end features filename (training)
  	labels_filename       AutoVOT front end labels filename (training)
  	model_filename        Name of classifiers (output)

	Optional arguments:		Function:
	-h, --help            show this help message and exit
	--logging_level LOGGING_LEVEL
                        Level of verbosity of information printed out by this
                        program (DEBUG, INFO, WARNING or ERROR), in order of
                        increasing verbosity. See
                        http://docs.python.org/2/howto/logging for
                        definitions. (default: INFO)


## VOT DECODING

#### Mode 1
##### *Use an existing classifier to measure VOT for stops in a set of textgrids and corresponding wav files.*
###### Usage: auto\_vot\_decode.py [OPTIONS] wav\_filename textgrid\_filename model\_filename

	Positional arguments:		Function:
	wav_filenames         Text file listing WAV files
	textgrid_filenames    Text file listing corresponding manually labeled
                        TextGrid files containing stops VOT is to be measured
                        for
	model_filename        Name of classifier to be used to measure VOT

	Optional arguments:		Function:
	-h, --help            show this help message and exit
	--vot_tier VOT_TIER   Name of the tier containing manually labeled VOTs to
                        compare automatic measurements to (optional. default
                        is none)
	--vot_mark VOT_MARK   On vot_tier, only intervals with this mark value (e.g.
                        "vot", "pos", "neg") are used, or "*" for any string
                        (this is the default)
	--window_tier WINDOW_TIER
                        Name of the tier containing windows to be searched as
                        possible startsof the predicted VOT (default: none).
                        If not given *and* vot_tier given, a window with
                        boundaries window_min and window_max to the left and
                        right of the manually labeled VOT will be used . NOTE:
                        either window_tier or vot_tier must be specified. If
                        both are specified, window_tier is used, and
                        window_min and window_max are ignored.
	--window_mark WINDOW_MARK
                        VOT is only predicted for intervals on the window tier
                        with this mark value (e.g. "vot", "pos", "neg"), or
                        "*" for any string (this is the default)
	--window_min WINDOW_MIN
                        Left boundary of the window (in msec) relative to the
                        VOT interval's left boundary.
	--window_max WINDOW_MAX
                        Right boundary of the window (in msec) relative to the
                        VOT interval's right boundary. Usually should be
                        positive, that is, after the VOT interval's right
                        boundary. (default: 800)
	--min_vot_length MIN_VOT_LENGTH
                        Minimum allowed length of predicted VOT (in msec)
                        (default: 15)
	--max_vot_length MAX_VOT_LENGTH
                        Maximum allowed length of predicted VOT (in msec)
                        (default: 250)
	--max_num_instances MAX_NUM_INSTANCES
                        Maximum number of instances per file to use (default:
                        use everything)
	--ignore_existing_tiers
                        add a new AutoVOT tier to output textgrids, even if
                        one already exists (default: don't do so)
	--csv_file CSV_FILE   Write a CSV file with this name with one row per
                        predicited VOT, with columns for the prediction and
                        the confidence of the prediction (default: don't do
                        this)
	--logging_level LOGGING_LEVEL
                        Level of verbosity of information printed out by this
                        program (DEBUG, INFO, WARNING or ERROR), in order of
                        increasing verbosity. See
                        http://docs.python.org/2/howto/logging for
                        definitions. (default: INFO)
                        

#### Mode 2
##### *Decoding when features have already been extracted*
###### Usage: auto_vot_decode_after_fe.py [OPTIONS] features_filename labels_filename model_filename

	Positional arguments:		Function:
	features_filename     AutoVOT front end features filename (training)
	labels_filename       AutoVOT front end labels filename (training)
	model_filename        Name of classifier to be used to measure VOT

	Optional arguments:		Function:
	-h, --help            show this help message and exit
	--logging_level LOGGING_LEVEL
                        Level of verbosity of information printed out by this
                        program (DEBUG, INFO, WARNING or ERROR), in order of
                        increasing verbosity. See
                        http://docs.python.org/2/howto/logging for
                        definitions. (default: INFO)


## Check Performance
##### *Compute various measures of performance given a set of labeled VOTs and predicted VOTs for the same stops, optionally writing information for each stop to a CSV file.*
###### Usage: auto_vot_performance.py [OPTIONS] labeled_textgrid_list predicted_textgrid_list labeled_vot_tier predicted_vot_tier [OPTIONS]


	Positional arguments:		Function:
	labeled_textgrid_list
                        textfile listing TextGrid files containing manually
                        labeled VOTs (one file per line)
	predicted_textgrid_list
                        textfile listing TextGrid files containing predicted
                        VOTs (one file per line). This can be the same as
                        labeled_textgrid_list, provided two different tiers
                        are given for labeled_vot_tier and predicted_vot_tier.
	labeled_vot_tier      name of the tier containing manually labeled VOTs in
                        the TextGrids in labeled_textgrid_list (default: vot)
	predicted_vot_tier    name of the tier containing automatically labeled VOTs
                        in the TextGrids in predicted_textgrid_list (default:
                        AutoVOT)

	Optional arguments:		Function:
	-h, --help            show this help message and exit
	--csv_file CSV_FILE   csv file to dump labeled and predicted VOT info to
                        (default: none)
    
    
<a name="tutorial"/>

# Tutorial

<a name="praattutorial"/>

## Praat plugin tutorial

*Bring me to the [command line tutorial](#cltutorial)*

**Note:** 
* This plugin does not train a new classifier. You have the option of using one of the classifiers provided with this installation. If you'd like to train your own, please follow the [command line tutorial](#cltutorial).
* The Praat plugin allows you to work with one file at a time. If you would like to execute this function over all files in a directory and save them, you may write a Praat script to do so. A script will be made available in a future version of AutoVOT.


---------
* Open the soundfile and accompanying TextGrid in Praat.
	* For this tutorial you may use files from the `test` experiment directory, e.g:
	* `autovot-0.93/experiments/data/tutorialExample/test/voiced/cas7D_1054_24_3.wav`
	* `autovot-0.93/experiments/data/tutorialExample/test/voiced/cas7D_1054_24_3.TextGrid`

* Select both and click AutoVOT in the Praat Objects window.

* Adjust the parameters as necessary:
	**Sound and TextGrid parameters**
	* Interval tier: name of the tier containing the intervals where you would like to predict the VOT

		`vot`

	* Interval mark: text used to mark your VOT intervals

		`\*default = any string` or `vot`

	* Channel: Choose mono for a single track recording or left/right if you are using a multitrack recording.

		`Mono`
	
	**Algorithm Parameters**
	* min vot length: The minimum duration (in ms) that the algorithm should predict. **Note** that the default is 15ms. For English voiced stops this is too high. Since this tutorial uses voiced data, we will change this.		

		`5`

	* max vot length: The maximum duration (in ms) that the algorithm should predict.

		`100`

	* Training model (path): Indicate which training model file you would like to use (default = amanda)
		
		`models/vot_predictor.amanda.max_num_instances_1000.model`

* Click "Next"

If successful, the Praat info window will display the output of auto\_vot_decode.py and the new TextGrid with the AuotVOT prediction tier will open with the sound in the Praat editor. You must save the TextGrid manually.

*Back to [top](#toc)*

<a name="cltutorial"/>

# From the command line

*Bring me to the [Praat plugin tutorial](#praattutorial)*

*Back to [top](#toc)*

#### Information for command-line arguments to be used in this example:
* **TextGrid labels** are all `vot`. This includes tier names and window labels.
* **File lists** will be generated in the first step of the tutorial and will be located in `experiments/config/`.
* **Classifier files** will be generated during training and will be located in `experiments/models/`.
* **Feature file lists** will be generated during feature extraction in Mode 2 and will be located in `experiments/config/`. These include AutoVOT Front End input, feature, and label files.
* **Feature files** will be generated during feature extraction in Mode 2 and will be located in `experiments/tmp_dir/`.


## Getting started

### Generate file lists
The user may also provide their own file lists in the `config` directory if they prefer.

* From within `experiments` run: 

	`$ data/tutorialExample/makeConfigFiles.sh`

* `experiments/config` should now contain 8 new files containing lists of the testing/training files, as listed above.


## Mode 1
### Training
**Note** that for Mode 1, feature extraction is hidden to the user as a component of auto_vot_train.py.

Navigate to `experiments/` and run the following:

*Note: `\` indicate line breaks that should not be included in the actual command line prompt.*

For voiceless data:

	auto_vot_train.py --vot_tier vot --vot_mark vot \
	config/voicelessTrainWavList.txt config/voicelessTrainTgList.txt \
	models/VoicelessModel.classifier
	
For voiced data:

	auto_vot_train.py --vot_tier vot --vot_mark vot \
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

	auto_vot_decode.py --vot_tier vot  --vot_mark vot \
	config/voicelessTestWavList.txt config/voicelessTestTgList.txt \
	models/VoicelessModel.classifier
	
For voiced:

	auto_vot_decode.py --vot_tier vot  --vot_mark vot --min_vot_length 5 \
	--max_vot_length 100 config/voicedTestWavList.txt \
	config/voicedTestTgList.txt models/VoicedModel.classifier

If successful, you'll see information printed out about how many instances in each file were successfully decoded. After all files have been processed, you'll see the message:

`[auto_vot_decode.py] INFO: All done.`

If there were any problematic files that couldn't be processed, these will appear at the end, with the message:

`[auto_vot_train.py] WARNING: Look for lines beginning with WARNING or ERROR in the program's output to see what went wrong.`

Your TextGrids will be overwritten with a new tier called AutoVOT containing VOT predictions. These intervals will be labeled "pred".

## Mode 2
**Recommended for large datasets**

### Extracting acoustic features
Navigate to `experiments/` and run:

For voiceless:

	auto_vot_extract_features.py --vot_tier vot --vot_mark vot \
	config/voicelessTrainTgList.txt config/voicelessTrainWavList.txt \
	config/VoicelessFeInput.txt config/VoicelessFeFeatures.txt \
	config/VoicelessFeLabels.txt tmp_dir
	
For voiced: 

	auto_vot_extract_features.py --vot_tier vot --vot_mark vot \
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

	auto_vot_train_after_fe.py config/VoicelessFeFeatures.txt \
	config/VoicelessFeLabels.txt models/VoicelessModel_ver2.classifier
	
For voiced:

	auto_vot_train_after_fe.py config/VoicedFeFeatures.txt \
	config/VoicedFeLabels.txt models/VoicedModel_ver2.classifier

If training is successful classifier files will be generated in `experiments/models/` and you will see the following output command line message upon completion:

`[InitialVotTrain] INFO: Training completed.`

### Decoding
**Note:** Mode 2 decoding outputs a summary of the predictions' performance, **not** modified TextGrids containing predictions. If you wish to produce predictions in the TextGrids, you may perform Mode 1 decoding using the classifiers produced from Mode 2 training.

For voiceless:

	auto_vot_decode_after_fe.py config/VoicelessFeFeatures.txt \
	config/VoicelessFeLabels.txt models/VoicelessModel_ver2.classifier
	
For voiced:

	auto_vot_decode_after_fe.py config/VoicedFeFeatures.txt \
	config/VoicedFeLabels.txt models/VoicedModel_ver2.classifier

If decoding is successful you will see a summary of the average performance of the VOT predictions based on the test set. You can use this summary output to tweak training parameters to fine-tune your output without having to re-extract features.

Example output: 

	[VotDecode] INFO: Total num misclassified = 0
	[VotDecode] INFO: Num pos misclassified as neg = 0
	[VotDecode] INFO: Num neg misclassified as pos = nan
	[VotDecode] INFO: Cumulative VOT loss on correctly classified data = 2.10667
	[VotDecode] INFO: % corr VOT error (t <= 2ms) = 77.3333
	[VotDecode] INFO: % corr VOT error (t <= 5ms) = 92
	[VotDecode] INFO: % corr VOT error (t <= 10ms) = 97.3333
	[VotDecode] INFO: % corr VOT error (t <= 15ms) = 100
	[VotDecode] INFO: % corr VOT error (t <= 20ms) = 100
	[VotDecode] INFO: % corr VOT error (t <= 25ms) = 100
	[VotDecode] INFO: % corr VOT error (t <= 50ms) = 100
	[VotDecode] INFO: RMS onset loss: 8.66025
	[VotDecode] INFO: Decoding completed.


## Check Performance (after either Mode 1 or Mode 2)
It is possible to compare the performance of the algorithm to a development set of manually measured VOT. If you would like to do this, generate a list of the TextGrids included in your test data that also have manual measurements. You may choose to either keep these TextGrids separate, in a distinct directory in which case you will need two lists of TextGrids. You may also simply include TextGrids in your test data that have an additional tier containing the manual annotation (just be sure that the tier name is distinct from your window tier and AutoVOT tier. In this case, you would need to only reference the single TextGrid list. The script optionally outputs a CSV file with information about the VOT start time and duration in the manual and automatic measurement tiers. 

**Note:** This tutorial does not include this component, but the following section will provide an example of how it can be used with your data. Example arguments include:

* `checkPerformanceTgList.txt:` the list of TextGrids that would contain an additional tier with manual VOT measurements. For example, if you were testing over voiced stops, it would be `config/voicedTestTgList.txt`.
* `ManualVot:` the name of the tier containing manual VOT measurements
* `checkPerformance.csv:` the output CSV file to be generated

Still from within `experiments/` run the following:

	auto_vot_performance.py config/checkPerformanceTgList.txt \
	config/checkPerformanceTgList.txt ManualVot AutoVOT --csv_file checkPerformance.csv

If successful, the command line output will generate Pearson correlations, means, and standard deviations for the VOT measurements, as well as the percentage of tokens within given durational threshold differences. The output CSV file will contain VOT start times and durations for the data subset.

**Note:** It is **not recommended** to simply include the data used for training as the subset used to compute performance. The reason for this is that performance measurements are likely to be inflated in such a situation, as the algorithm is generating predictions for the same tokens on which it was trained. It is best to have an additional subset of manually aligned VOT if you would like to take advantage of this comparison. 

## Possible errors and warnings
*Back to [top](#toc)*

### General Errors
##### Missing files
If you do not have a corresponding wav file for a TextGrid:

`ERROR: Number of TextGrid files should match the number of WAVs`

##### Wrong file format
If one of your files does not have the right format, the following error will appear:

`ERROR: *filename*.wav is not a valid WAV.`
`ERROR: *filename*.TextGrid is not a valid TextGrid.`

### Warnings during Training:

##### Short VOT in training data
If you have shorter instances of VOT in your training data, you may get the following error:

`[InitialVotTrain] WARNING: Hinge loss is less than zero. This is due a short VOT in training data.`

You can ignore this, but be aware that a VOT in the training data was skipped.

### Warnings during Decoding:

##### AutoVOT tier already exists
`WARNING: File *filename*.TextGrid already contains a tier with the name "AutoVOT"`

`WARNING: New "AutoVOT" tier is NOT being written to the file.`

`WARNING: (use the --ignore_existing_tiers flag if you'd like to do so)`


If you've used --ignore_existing_tiers flag, you'll be reminded that an AutoVOT tier exists already:

`[auto_vot_decode.py] WARNING: Writing a new AutoVOT tier (in addition to existing one(s))`



<a name="citing"/>

## How do I Cite AutoVOT in my articles?

***If possible to cite a program, the following format is recommended (adjusting retrieval dates and versions as necessary):***

* Keshet, J., Sonderegger, M., Knowles, T. (2014). AutoVOT: A tool for automatic measurement of voice onset time using discriminative structured prediction [Computer program]. Version 0.91, retrieved August 2014 from https://github.com/mlml/autovot/.

***If you are unable to cite the program itself, please cite the following paper:***

* Sonderegger, M., & Keshet, J. (2012). Automatic measurement of voice onset time using discriminative structured predictions. *The Journal of the Acoustical Society of America*, 132(6), 3965-3979.

<a name="acknowledgements"/>

## Acknowledgements

### Code
This software incorporates code from several open-source projects:

#### FFTReal
FFTReal, Version 1.02, 2001/03/27

Fourier transformation (FFT, IFFT) library specialised for real data.

Copyright (c) by Laurent de Soras <laurent.de.soras@club-internet.fr>

Object Pascal port (c) Frederic Vanmol <frederic@fruityloops.com>

#### get_f0, sigproc
get_f0.c estimates F0 using normalized cross correlation and dynamic programming. 
sigproc.c is a collection of pretty generic signal-processing routines.

Written and revised by: Derek Lin and David Talkin

Copyright (c) 1990-1996 Entropic Research Laboratory, Inc. All rights reserved

This software has been licensed to the Centre of Speech Technology, KTH by Microsoft Corp. with the terms in the accompanying file BSD.txt, which is a BSD style license.

#### textgrid.py
Python classes for Praat TextGrid and TextTier files (and HTK .mlf files)

[http://github.com/kylebgorman/textgrid/](http://github.com/kylebgorman/textgrid/)

Copyright (c) 2011-2013 Kyle Gorman, Max Bane, Morgan Sonderegger

### Example data

Example data was provided jointly by **Meghan Clayards,** [McGill University Speech Learning Lab](http://people.linguistics.mcgill.ca/~meghan.clayards/Research.html) and **Michael Wagner,** [McGill University Prosody Lab](http://prosodylab.org/). Data collection was funded by:

* SSHRC #410-2011-1062 
* Canada Research Chair #217849
* FQRSC-NC #145433

### Feedback

We thank Eivind Torgersen for feedback on the code.

*Back to [top](#toc)*

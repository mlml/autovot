autovot
=======
    
## Out of the box directories and files

    Path                        Description/contents
    autovot/                    Master directory
    auto_vot/                   
        bin/                    
        code/                   
            audiofile-0.3.4/    
            learning_tools/     
            Makefile            
            vot_predictor/
    makeConfigFiles.sh          Helper file to generate file lists
            
**There are two modes for training and decoding:**
* **Mode1:** The handling of feature extraction is hidden. The train can get cross-validation set or automatically select 20% of the training data.
* **Mode2:** Training and decoding is done after feature extraction. Features are extracted to a known directory once after which training and decoding may be done as needed. Mode 2 is ideal if you have a large quantity of data.

## User provided files and directories
***Important:*** Input files will be overwritten. If you wish to access your original files, be sure to back them up elsewhere.

#### Sound file format
* Wav files sampled at 16kHz

#### TextGrid file format
* Saved as text files with .TextGrid extension
* TextGrids for training must contain a tier with hand measured vot intervals. These intervals must have a common text label, such as "vot."
* TextGrids for testing must contain a tier with window intervals indicating the position the algorithm should begin looking for VOT onset. These intervals must have a common text label, such as "pos."

#### Directory format
User provided directories should be in the autovot master directory.

See example data & experiment folders.
    
    Path                        Description/Contents

    data/train/                 Each embedded folder should contain both TextGrids and corresponding wav files
        voiced/
        voiceless/
    data/test/                  Each embedded voiced/voiceless folder should contain only TextGrids. 
                                Wav files should be in separate folder.
        voiced/
        voiceless/
        wav/
    
    experiments/config/:        Lists of filenames. See below for more info.
    experiments/models/:        Empty. This is where classifiers will be stored.
    experiments/tmp_dir/:      Empty. This is where feature extraction will be stored in Mode 2.

#### Helper files for generating user's file lists
Move makeConfigFiles.sh to data/. Navigate to this folder and run:

    $ ./makeConfigFiles.sh
    
If successful, a .txt file will be generated for all voiced and voiceless TextGrids and wav files in each train and test directory
    
# Getting Started
## Compiling (TODO: fix layout)
### Compile Audiofile Library

    Navigate to auto_vot/code/audiofile-0.3.4/ and run:
        $ make
    If successful, the command line will print out a lot of text. The last line should be:
        make[2]: Nothing to be done for 'all-am'.

### Compile Code

    Navigate to auto_vot/code/ and run:
        $ make clean
    If successful, the final line of the output should be:
        [make] Cleaning completed
    Then, run:
        $ make
    Final line of the utput should be:
        [make] Compiling completed
    

    Then navigate to experiments/ and run:
        $ export PATH=$PATH:*full path here*/autovot/auto_vot/bin
        
        
    TODO: Run:
        $ make install

# Usage
Tutorial to follow

## Training and feature extraction

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

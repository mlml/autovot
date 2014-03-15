autovot
=======

## Assumptions

- The user has backed up their data; input files will be overwritten.
- TextGrids have been saved as text files with .TextGrid extension
- Wav files are sampled at 16kHz
    
## Out of the box

    autovot/
    auto_vot/
        bin/
        code/
            audiofile-0.3.4/
            learning_tools/
            Makefile
            vot_predictor/
            
**There are two modes for training and decoding:**
* **Mode1:** The handling of feature extraction is hidden. The train can get cross-validation set or automatically select 20% of the training data. 
* **Mode2:** Training and decoding is done after feature extraction. Features are extracted to a known directory once after which training and decoding may be done as needed. Mode 2 is ideal if you have a large quantity of data.

## User provided files and directories
#### Sound file format
* Wav files sampled at 16kHz

#### TextGrid file format
* Saved as text files with .TextGrid extension
* TextGrids for training must contain a tier with hand measured vot intervals. These intervals must have a common text label, such as "vot."
* TextGrids for testing must contain a tier with window intervals indicating the position the algorithm should begin looking for VOT onset. These intervals must have a common text label, such as "pos."

#### Directory format
User provided directories should be in the autovot master directory.

See example data & experiment folders.

    data/train/: Each folder should contain both TextGrids and corresponding wav files
        voiced/
        voiceless/
    data/test/: Each folder should contain only TextGrids. Wav files should be in separate folder.
        voiced/
        voiceless/
        wav/
    
    experiments/config/: This should contain lists of filenames. See below for more info.
    experiments/models/: This is where classifiers will be stored.
    experiments/temp_dir/: This is where feature extraction will be stored in Mode 2.

#### Helper files for generating user's file lists
Move makeConfigFiles.sh to data/. Navigate to this folder and run:

    $ ./makeConfigFiles.sh
    
If successful, a .txt file will be generated for all voiced and voiceless TextGrids and wav files in each train and test directory
    
# Getting Started
## Compiling
### Compile Audiofile Library

    Navigate to auto_vot/code/audiofile-0.3.4/
    Run:
        $ make
    If successful, the command line will print out a lot of text. The last line should be:
        make[2]: Nothing to be done for 'all-am'.

### Compile Code

*Navigate to auto_vot/code/*
*Run:*

    $ make clean

*Output:*

    [make] Cleaning completed


*Run:*

    $ make

*Output:*

    [make] Compiling completed

*Navigate to experiments/*

*Run:*

    $ export PATH=$PATH:*full path here*/autovot/auto_vot/bin

***TODO: Run:***

    $ make install

# Script Usage
Tutorial to follow




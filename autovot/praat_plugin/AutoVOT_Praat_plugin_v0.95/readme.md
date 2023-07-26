# AutoVOT Praat Plugin

## Option 1: Single file
See current instructions on [AutoVOT readme](https://github.com/mlml/autovot/#praattutorial)

## Option 2: Batch process
This allows the user to run AutoVOT over all files in a given directory. It also allows for the option to "embellish" the output which includes several options such as relabelling the autoVOT prediction interval.

To run this option:

- From the Praat Object window, click New >> Batch Process AutoVOT...
- This will bring up the first form, detailed below.

### Prompt 1: Enter directories, choose options
- textgrid directory: Contains TextGrids that are ready to be processed by AutoVOT
- audio directory: Contains corresponding .wav files with the same name as the TextGrids
  - May or may not be the same directory as the textgrid directory
- output directory: Where you want the output TextGrids to be saved. Your original TextGrids do NOT get overwritten.

- **manually choose**: if selected, the user will be prompted to select each directory (in order of textgrids, audio, output)
  - The directory paths will print to the screen before AutoVOT launches

- **embellish**: if selected, the next prompt will allow the user to select various options to further modify the output TextGrids
  - TODO: Determine whether this is general use enough

- **verbose**: if selected, AutoVOT output (and other messages) will be printed to the Praat info window
  - TODO: potentially save this output to an external .txt file

- **test**: if selected, AutoVOT will only run on the first 5 files

### Prompt 2: Embellish
This prompt only appears if "embellish" was selected at the opening prompt.
This creates a tier called AutoVOT-edit in place of the original AutoVOT tier.
The user is prompted to enter the following information:

- **segment_tier**: The tier number containing the stop labels.
  - This could be the same as the tier with the stop windows to feed to AutoVOT, but not necessarily.
  - In the AutoVOT Tutorial example, the stop windows are on tier 3 (and always labelled "vot"), whereas the stop LABELS are the force-aligned segments on tier 3.
  - Critically, the script expects the VOT onset to occur within the boundaries of the stop label.
- **relabel**: if selected, the AutoVOT labels ("pred") will be relabelled as a combination of the label on the **segment_tier** plus the suffix provided below.
- **vot_suffix**: suffix to append to the stop label (default is \_vot, so the AutoVOT label becomes, e.g., P_vot)
- **include_onset**: if selected, the onset of the stop interval on the **segment_tier** will be included on the final AutoVOT-edit tier.
- **closure_suffix**: suffix to append to the closure label (default is \_clo)

**Note:** If any files did not contain stop intervals, the "embellish" option adds an empty AutoVOT-edit tier. The regular AutoVOT output would not add any tiers for these files.

### Prompt 3: AutoVOT parameters
This prompt takes the same arguments as the original AutoVOT Praat plugin and calls autovot_batch.praat, which then runs AutoVOT as per the original plugin.

### Running:
- If **verbose** was selected, the AutoVOT information will be printed to the screen, along with warnings if files without any stop intervals are detected.
- Upon completion, the output directory will be automatically opened.

## Outstanding TODOs
- option to save Praat info to external text file
- prompt citation to the program (Keshet et al., 2014) instead of or in addition to the paper (Sonderegger & Keshet, 2012)
- identify other options for TextGrid embellishment
- lower priority: include additional plugin scripts to help users prepare TextGrids for AutoVOT based on force-aligned TextGrids

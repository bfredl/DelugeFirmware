# DX7 synth

## Description:

Synth type fully compatible with dx7 patches with full editing support

Implemented as a special case of the subtractive enginge, where OSC1 is changed to a new DX7 oscilator type.
It is possible to use the filters on a DX7 patch (though this increase CPU usage and decrease polyphony).

## Usage:

optional: place DX7 compatible patch banks in .syx format in a "DX7/" folder in the sdcard.

Enable the "DX SHORTUCTS" community setting for full access to the relavant UI behaviors.

To create a new synth in dx7 mode, use the new shortcut "CUSTOM 1" + "SYNTH".
This will show up a menu. The first option in this menu lets you load patches.
First you browse for .syx files in the folder (nested subdirs are allowed).
After selecting a file, you will get a list with the 32 patches in the back.
Make sure to enable "KEYBOARD", to be able to audition different patches while browsing through.

If you want to create a patch from scrattch, you can use the second "DX7 PARAMETERS" option,
or just exit the menu to use the shortcuts for editing.

The DX7 sidebar is very useful when editing and gives you shortcuts to edit any operator.
With the community setting active, this column will automatically appear as the right column,
unless it was manually set to something else.

The first 6 rows shows the operators as green for a carrier and blue for a modulator, respectively
taping a single pad in isolation lets you switch an operator on or off. Shift+colored pad
opens the editor for that operator. First, the level parameter is selected, and shift+pad again
gives you the COARSE tuning parameter.
Likewise, shift+press the seventh pad will open up the editor for global parameters,
starting with algorithm and feedbak.

Inside the editor view (when the sidebar is flashing), the LEFT-RIGHT encoder can be used
to browse between parameters. Some parameters have a shortcut in a layer which is only
active when editing any DX7 parameter (otherwise the regular synth shortcuts apply)

For the first 6 rows these are

TABELL

The seventh row contains some of the global parameters

TABELL

If OLED screen is available a group of related parameters is displayed at a time, like all the envelope levels and rates
for a specific envelope. With 7SEG the parameter name will be shown briefly before the value


## Missing features:

- [ ] midi implementation
- [ ] modulating/automating individual operators

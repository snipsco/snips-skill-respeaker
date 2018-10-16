## Status

:sleeping: ***``` On idle: random led breathe in green```***

:smiley: ***```On listen: all leds start to blink in blue```***

:loudspeaker: ***```On speak: all leds start to breathe in purple```***

:ok_hand: ***```On success: all leds reverse blink in green```***

:exclamation: ***```On error: all leds reverse blink in red```***

:speak_no_evil: ***```To mute: all leds breathe once in red```***

:smile_cat: ***```To unmute: all leds breathe once in blue```***

:see_no_evil: ***```On off: all leds turn off```***

## Installation

#### By using `sam`:

```bash
sam install actions -g https://github.com/snipsco/snips-skill-respeaker-c.git
```

#### Manually:

```bash
git clone  https://github.com/snipsco/snips-skill-respeaker-c.git
cp -r snips-skill-respeaker-c /var/lib/snips/skills/
cd /var/lib/snips/skills/snips-skill-respeaker-c
./setup.sh
```

## On/Off mode

Mode change follows hermes procotol.

#### Turn on
```
hermes/feedback/sound/toggleOn
```
#### Turn off
```
hermes/feedback/sound/toggleOff
```

## Brightness

You can adjust the max brightness in the `config.ini` file, keep in mind that the range is between 0-255(Most bright). 

## Animation

You can enable or disable a certain animation. By doing this, modify the `config.ini` file. 

`1` stands for turn on and `0` stands for turn off.

By default, the `on_success` and `on_error` animation is disabled.

## Nightmode

***To be able to use this function, please properly set your timezone***

***Timezone setting: `sudo raspi-config` -> `Localisation Options` -> `Change Timezone` ***

`nightmode`: enable/disable night mode.

`go_sleep` : set the time when led turn itself off. 

`go_weak` : set the time when the led turn itself on.

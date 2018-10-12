## Status

***```"On idle: random led breathe in green"```***

***```"On listen: all leds start to flush in blue"```***

***```"On speak: all leds start to breathe in purple"```***

***```"On success: all leds reverse flush in green"```***

***```"On error: all leds reverse flush in red"```***

***```"To mute: all leds breathe once in red"```***

***```"To unmute: all leds breathe once in blue"```***

***```"On off: all leds turn off"```***

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

## ON/OFF mode

Mode change follows hermes procotol

#### Turn on
```
hermes/feedback/sound/toggleOn
```
#### Turn off
```
hermes/feedback/sound/toggleOff
```
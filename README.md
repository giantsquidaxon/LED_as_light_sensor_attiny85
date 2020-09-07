# LED as Light Sensor

I'd heard rumours that LEDs could be used as light sensors, but actual code for doing it seemed a little bit scattershot, so here's my tuppence worth. This cute old article https://www.merl.com/publications/docs/TR2003-35.pdf lays out the basics of using a LED for two-way communication, albeit in slightly delerious terms. Oh, to be as enthusiastic about technology as a man from 2003! Anyway, time to totally rip it off!

## The circuit
In order to use an LED bidirectionally we need to sling it between two GPIO pins, ideally with a 100R resistor on the cathode. We're messing with capacitances here to pull this off, so long lengths of wire, solderless breadboards, your oscilloscope, and the phase of the moon can throw this off. Be careful! Try stuffing the LED right into the dupont connectors on your arduino and hoping for the best. It's not that bad, really. It *will* stop working when you connect your oscilloscope, though.

To make a reading, first we reverse bias the LED in order to *charge* up the tiny capacitance across the diode junction. Then we put the pin to which the cathode is connected into High Z and *take a reading*, after first ensuring that the internal pull-up resistor is disabled. Readings are taken by sitting in a loop waiting for the digital value read from the cathode pin to drop to logic 0, and counting the loop iterations. Photons will knock charge carriers free in the depletion region of the diode, making the capacitor leak and discharge faster. This means higher light levels lead to shorter discharge times, and lower light levels have longer discharge times. In fact, in dim light we might spend longer than a second waiting for full discharge, so it's prudent to add some sort of a timeout (`LED_TIMEOUT`).

When we've detected our event -- in this case, an increase in discharge time of 20% due to the LED being in shadow -- we can get really fruity and use the LED to *actually emit light* by forward-biasing it. I wanted to make it fade in and out nicely, so I cobbled together some software 12 bit PWM because the attiny85 only has 8 bit pwm in hardware, which looks janky AF in the tail of smooth fades. Should get around 1$\mu$s resolution with the attiny85 clocked at 8MHz with the internal clock. Two LEDs are supported, specify their pins (logical pins, not physical pins) in the defines at the start of the source.

As readings can get a little bit noisy, I found I got more reliable results (at the cost of more latency) by averaging `SAMPLES_TO_AVERAGE` samples together then comparing those.

To summarise:

| State        | Anode pin | Cathode pin |
| ---          | ---       | ---         |
| Charging     | Logic 0   | Logic 1     |
| Reading      | Logic 0   | High Z      |
| Illuminating | Logic 1   | Logic 0     |

## Debugging
There's some serial logging stuff in there if you want to run it on arduino, or if you're using software serial on the serialless attiny85. Useful for producing nice line graphs. Define `LOGGING` to get this. Running this on an arduino will need you to rename the `PORTB` and `DDRB` registers as they vary between the chips. TODO: fix this, it's shameful.

If you want some slightly more light-touch debugging, you can define a `MONITOR` pin (still logical, not physical) that will go high for the duration of the LEDs discharge and let you observe what's going on on your scope.

## Choosing a LED
Different colours of LED have different sensitivity and different spectral responses, he said, redundantly. Less obviously, I found much bigger differences between similarly specced and coloured LEDs from different manufacturers, so always test and never assume.

## Flashing it onto an attiny85
I used an arduino with the ISP sketch on it hooked up like this:
| Arduino    | attiny85 |
| ---        | ---      |
| GND        | Pin 4    |
| 5V         | Pin 8    |
| Digital 13 | Pin 7    |
| Digital 12 | Pin 6    |
| Digital 11 | Pin 5    |
| Digital 10 | Pin 1    |

You're going to have to burn the bootloader to the attiny85, then set it to use the 8 MHz internal clock.

Once programmed, the circuit seems to work fine powered at 3V--5V.

## This seems unnecessarily involved. Couldn't I just use a photodiode or an LDR or something?
Yes.

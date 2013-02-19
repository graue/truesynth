# Truesynth

Some audio effects and an oscillator.

Can be used on the command line as a set of filters, e.g.

    cat audiosource.f32 | synth amp -dB +6 >louder.f32

Here `amp` is the effect to use. To find out its parameters, do:

    $ synth amp -help
    amp is an effect:
     -dB -120.00..120.00 [0.00]

This shows the effect has one parameter, `-dB`, and the minimum,
maximum, and default values of that parameter.

For a list of all generators and effects use `synth -help`.


## The CLI only handles raw 32-bit floats

*Fair warning*: Truesynth is somewhat unfriendly in what format
it handles. While most audio on your computer is probably stored
in a format like mp3 or wav, Truesynth only operates on raw 32-bit
float samples.

(Also, since no file headers are used, it just assumes
44100 Hz is the sampling rate - you can override this by setting a
`RATE` environment variable.)

You can at least convert between 16-bit integers and 32-bit floats
using `fmt` and `unfmt` from my older
[synth package](https://github.com/graue/synth).

My personal convention when saving raw, 32-bit float files is to
give them the extension `.f32`.


## These are not just CLI programs!

Truesynth can be used as a library and the effects can be embedded
in other programs. Define `SYNTH_NO_SHELL` to avoid compiling the
command-line interface, and see synth.h for the API.

In this sense, Truesynth is an audio framework. It basically does
the same thing as VST or [LV2](http://lv2plug.in) while being
much simpler and more primitive.

It's a refinement of my older [synth package](https://github.com/graue/synth),
offering two big improvements:

* Can be embedded in non-CLI host programs
* Parameters can change at runtime. For example, you can initialize
  an amp effect with a gain of +6 dB, but then change it back to 0 dB
  after a while... or write a GUI program where a knob controls the gain,
  or whatever.

In 2009 I wrote a host program that embedded Truesynth effects,
created a feedback loop, and allowed the effect parameters to be controlled
with a joystick. I used this to play a few live experimental-music shows.
That host program was pretty messy and isn't open sourced yet,
but that is the whole point of using Truesynth instead of Synth.

However, Synth has way more effects that I never ported over.

I'm trying the Truesynth concept again, but with all effects coded in Lua,
with [Luasynth](https://github.com/graue/luasynth).


## License

MIT license.

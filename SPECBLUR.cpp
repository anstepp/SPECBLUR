/* SPECBLUR - an instrument to blur a spectrum in the frequency and time
domain

   p0 = output start time
   p1 = input start time
   p2 = input duration
   p3 = amplitude multiplier
   p4 = input channel [optional, default is 0]
   p5 = pan (in percent-to-left format) [optional, default is .5]

   p3 (amp) and p5 (pan) can receive updates from a table or real-time
   control source.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ugens.h>
#include "SPECBLUR.h"          // declarations for this instrument class
#include <rt.h>
#include <rtdefs.h>


SPECBLUR::SPECBLUR()
	: _in(NULL), _branch(0)
{
}

SPECBLUR::~SPECBLUR()
{
	delete [] _in;
}

int SPECBLUR::init(double p[], int n_args)
{
	_nargs = n_args;

	const float outskip = p[0];
	const float inskip = p[1];
	const float dur = p[2];

	_inchan = (n_args > 4) ? int(p[4]) : 0;

	if (rtsetoutput(outskip, dur, this) == -1)
		return DONT_SCHEDULE;

	if (outputChannels() > 2)
		return die("SPECBLUR", "Use mono or stereo output only.");

	if (rtsetinput(inskip, this) == -1)
		return DONT_SCHEDULE;

	if (_inchan >= inputChannels())
		return die("SPECBLUR", "You asked for channel %d of a %d-channel input.",
		                                             _inchan, inputChannels());

	return nSamps();
}

int SPECBLUR::configure()
{

	_in = new float [RTBUFSAMPS * inputChannels()];

	return _in ? 0 : -1;
}

void SPECBLUR::doupdate()
{

	double p[6];
	update(p, 6);

	_amp = p[3];

	_pan = (_nargs > 5) ? p[5] : 0.5;
}

int SPECBLUR::run()
{

	const int samps = framesToRun() * inputChannels();

	rtgetin(_in, this, samps);

	for (int i = 0; i < samps; i += inputChannels()) {

		if (--_branch <= 0) {
			doupdate();
			_branch = getSkip();
		}

		float insig = _in[i + _inchan] * _amp;

		float out[2];

		out[0] = insig;

		if (outputChannels() == 2) {
			out[1] = out[0] * (1.0f - _pan);
			out[0] *= _pan;
		}

		rtaddout(out);

		increment();
	}

	return framesToRun();
}

Instrument *makeSPECBLUR()
{
	SPECBLUR *inst = new SPECBLUR();
	inst->set_bus_config("SPECBLUR");

	return inst;
}

void rtprofile()
{
	RT_INTRO("SPECBLUR", makeSPECBLUR);
}



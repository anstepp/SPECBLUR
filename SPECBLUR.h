#include <Instrument.h>      // the base class for this instrument

class SPECBLUR : public Instrument {

public:
	SPECBLUR();
	virtual ~SPECBLUR();
	virtual int init(double *, int);
	virtual int configure();
	virtual int run();

private:
	void doupdate();

	float *_in;
	int _nargs, _inchan, _branch;
	float _amp, _pan;
};


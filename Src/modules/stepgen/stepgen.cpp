
#include "stepgen.h"
#include "remora.h"
//#include "../boardconfig.h"


/***********************************************************************
                MODULE CONFIGURATION AND CREATION FROM JSON     
************************************************************************/

void createStepgen()
{
    const char* comment = module["Comment"];
    printf("\n%s\n",comment);

    int joint = module["Joint Number"];
	const char* enable = module["Enable Pin"];
    const char* step = module["Step Pin"];
    const char* dir = module["Direction Pin"];

    // configure pointers to data source and feedback location
    //ptrJointFreqCmd[joint] = &rxData.jointFreqCmd[joint];
    //ptrJointFeedback[joint] = &txData.jointFeedback[joint];
    //ptrJointEnable = &rxData.jointEnable;

    // create the step generator, register it in the thread
    Module* stepgen = new Stepgen(base_freq, joint, enable, step, dir, STEPBIT);
    baseThread->registerModule(stepgen);
    baseThread->registerModulePost(stepgen);
}


/***********************************************************************
    MODULE CONFIGURATION AND CREATION FROM STATIC CONFIG - boardconfi.h   
************************************************************************/

void loadStaticStepgen()
{

}

/***********************************************************************
                METHOD DEFINITIONS
************************************************************************/

Stepgen::Stepgen(int32_t threadFreq, int jointNumber,std::string enable, std::string step, std::string direction, int stepBit) :
	jointNumber(jointNumber),
	enable(enable),
	step(step),
	direction(direction),
	stepBit(stepBit)
{
	this->enablePin = new Pin(this->enable, OUTPUT);			// create Pins
	this->stepPin = new Pin(this->step, OUTPUT);
	this->directionPin = new Pin(this->direction, OUTPUT);
	this->DDSaccumulator = 0;
	this->rawCount = 0;
	this->frequencyScale = (float)(1 << this->stepBit) / (float)threadFreq;
	this->mask = 1 << this->jointNumber;
	this->isEnabled = false;
	this->isForward = false;
	this->lastDir = !this->isForward;
}


void Stepgen::update()
{
	// Use the standard Module interface to run makePulses()
	this->makePulses();
}

void Stepgen::updatePost()
{
	//to-do need to ensure that minimum pulse timing is met.
	this->stopPulses();
}

void Stepgen::slowUpdate()
{
	return;
}

void Stepgen::makePulses()
{
	int32_t stepNow = 0;

	rxData_t* rxData = getCurrentRxBuffer(&rxPingPongBuffer);
	txData_t* txData = getCurrentTxBuffer(&txPingPongBuffer);

	this->isEnabled = ((rxData->jointEnable & this->mask) != 0);
	
	if (this->isEnabled == true)  											// this Step generator is enables so make the pulses
	{
		this->enablePin->set(false);                                			// Enable the driver - CHANGE THIS TO MAKE THE OUTPUT VALUE CONFIGURABLE???
		this->frequencyCommand = rxData->jointFreqCmd[jointNumber];             // Get the latest frequency command via pointer to the data source
		this->DDSaddValue = this->frequencyCommand * this->frequencyScale;		// Scale the frequency command to get the DDS add value
		stepNow = this->DDSaccumulator;                           				// Save the current DDS accumulator value
		this->DDSaccumulator += this->DDSaddValue;           	  				// Update the DDS accumulator with the new add value
		stepNow ^= this->DDSaccumulator;                          				// Test for changes in the low half of the DDS accumulator
		stepNow &= (1L << this->stepBit);                         				// Check for the step bit

		if (this->DDSaddValue > 0)												// The sign of the DDS add value indicates the desired direction
		{
			this->isForward = true;
		}
		else //if (this->DDSaddValue < 0)
		{
			this->isForward = false;
		}

        if (this->lastDir != this->isForward)
        {
            //Direction has changed, flip dir pin and do not step this iteration to give some setup time. At a 160kHz base thread freq, this should be about 6.25us, at 120kHz 8.33us (1 period). JMC servos requre 6us. TODO - make hold time configurable.
            this->lastDir = this->isForward;
            this->directionPin->set(this->isForward);             		// Set direction pin
        }else if (stepNow)
		{
			this->stepPin->set(true);										// Raise step pin - A4988 / DRV8825 stepper drivers only need 200ns setup time
            if (this->isForward)
            {
                this->rawCount++;
            }
            else
            {
                this->rawCount--;
            }
            txData->jointFeedback[jointNumber] = this->rawCount;							// Update position feedback via pointer to the data receiver
			this->isStepping = true;
		}
	}
	else
	{
		this->enablePin->set(true);
	}

}


void Stepgen::stopPulses()
{
	this->stepPin->set(false);	// Reset step pin
	this->isStepping = false;
}


void Stepgen::setEnabled(bool state)
{
	this->isEnabled = state;
}

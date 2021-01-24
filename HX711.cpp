#include "HX711.h"

HX711::HX711(PinName pin_dout, PinName pin_slk) :_pin_dout(pin_dout), _pin_slk(pin_slk)
{
    _pin_slk = 1;
    wait_ms(100);
    _pin_slk = 0;

    averageValue();
    this->setOffset(averageValue());
    this->setScale(1);
}

HX711::~HX711() 
{

}

long HX711::averageValue(unsigned char times)
{
    long sum = 0;
    for (unsigned char i = 0; i < times; i++)
    {
        sum += getValue();
    }

    return sum / times;
}

long HX711::getValue()
{
    long valeur = 0;

    while (_pin_dout)
        ;

    for (int i=0;i<24;i++)
    { 
        _pin_slk = 1;
        valeur=valeur<<1; 
        _pin_slk = 0;
        if(_pin_dout) valeur++; 
    } 

    _pin_slk = 1;
    _pin_slk = 0;

    valeur=valeur^0x800000;

    return valeur;
}

long HX711::getSignedValue()
{
    return (getValue()-0x7FFFFF);
}

void HX711::setOffset(long offset)
{
    _offset = offset;
}

void HX711::setScale(float scale)
{
    _scale = scale;
}

int HX711::getGram()
{
    long val = (getValue() - _offset);
    return (int)((val/_scale)*1000);
}
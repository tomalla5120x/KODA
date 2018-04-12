#ifndef CODEC_DISPATCH_H
#define CODEC_DISPATCH_H

#include <iostream>

#include "configuration.h"

bool dispatch(std::istream& in, std::ostream& out, Configuration& config);

#endif
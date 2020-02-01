#include "vessel.h"

bool Crane::operator < (const Crane &rhs) const {
	return init_pos < rhs.init_pos;
}

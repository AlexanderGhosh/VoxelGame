#include "BasicContainer.h"

using namespace GUI;
using namespace Utils;

DrawData BasicContainer::getDrawData() const
{
	return _children.front()->getDrawData();
}

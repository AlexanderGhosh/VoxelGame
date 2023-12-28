#include "TextBox.h"
#include "../Utils/DrawData.h"

using namespace GUI;
using namespace Utils;

DrawData TextBox::getDrawData() const
{
	return IGUI_Element::getDrawData();
}

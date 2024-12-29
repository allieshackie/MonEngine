#include "EventListener.h"

const EventFunc& EventSubscription::GetHandlerFunc() const
{
	return mHandlerFunc;
}

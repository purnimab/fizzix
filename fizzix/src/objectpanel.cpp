#ifndef OBJECTPANEL_CPP
#define OBJECTPANEL_CPP
#include "objectpanel.h"

ObjectPanel::ObjectPanel():QWidget()
{
	QGridLayout * layout = new QGridLayout();
	layout->addWidget(new QLabel ("Objects"),0,0);
	this->setLayout(layout);
}

#endif
#ifndef DATABACKEND_CPP
#define DATABACKEND_CPP
#include "databackend.h"

DataBackend::DataBackend():QObject()
{
	objects=new MapKeyListModel <DrawableObject *> ();
	forces=new MapKeyListModel <FizForce *> ();
	macros=new MapKeyListModel <FizFormula *> ();
	constants=new MapKeyListModel <fizdatum> ();
	dataChanges=new QUndoStack();
	newFromDefault();
	warnClear=true;
	QObject::connect(dataChanges,SIGNAL(indexChanged(int)),this,SLOT(dataUndone(int)));
}

bool DataBackend::isDataLocked()
{return dataLock;}

MapKeyListModel <DrawableObject *> * DataBackend::getObjectModel()
{return objects;}

MapKeyListModel <FizForce *> * DataBackend::getForceModel()
{return forces;}

MapKeyListModel <FizFormula *> * DataBackend::getMacroModel()
{return macros;}

MapKeyListModel <fizdatum> * DataBackend::getConstModel()
{return constants;}

QUndoStack * DataBackend::getUndoStack()
{return dataChanges;}

void DataBackend::setDataInserter(ChangeFactory * di)
{dataInserter=di;}

ChangeFactory * DataBackend::getDataInserter()
{return dataInserter;}

void DataBackend::toggleDataLock()
{
	dataLock=!dataLock;
	emit dataLocked(dataLock);
}

bool DataBackend::haveUnpredicatableChanges()
{
	return lastChangeUnpredictable;
}

void DataBackend::applyDataChange(QUndoCommand * c)
{
	if(!lastChangeUnpredictable)
	{
		dataChanges->push(c);
	}
	else
	{
		if(warnClear)
		{
			if(QMessageBox::question(NULL, "Clear Undo Stack?", 
			"Making changes to the input data will prevent you from resetting to the state before the engine ran. Make changes?",
			QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
			{
				dataChanges->clear(); //MASSIVE UTTERLY ENORMOUS MEMORY LEAK.
				c->redo();
				dataChanges->push(c);
				lastChangeUnpredictable=false;
				emit(gainedUnpredicatableChanges(lastChangeUnpredictable));
			}
		}
		else
		{
			dataChanges->clear(); //MASSIVE UTTERLY ENORMOUS MEMORY LEAK.
			c->redo();
			dataChanges->push(c);
			lastChangeUnpredictable=false;
			emit(gainedUnpredicatableChanges(lastChangeUnpredictable));
		}
	}
}

void DataBackend::applyUnpredictableDataChange(QUndoCommand * c, bool addToStack)
{
	if(addToStack)
	{
		dataChanges->push(c);
		if(!lastChangeUnpredictable)
			unpredictableChange=dataChanges->index();
	}
	else
	{
		c->redo();
	}
	if(!lastChangeUnpredictable)
		emit(gainedUnpredicatableChanges(true));
	lastChangeUnpredictable=true;
}


void DataBackend::dataUndone(int index)
{
	if(index<=unpredictableChange)
	{
		lastChangeUnpredictable=false;
		emit(gainedUnpredicatableChanges(lastChangeUnpredictable));
	}
}

fizstack DataBackend::parse(QString in)
{
	fizstack out;
	int i=0;
	try{
		out=Parser::parse(in,i);
	}
	catch(std::logic_error e)
	{
		QString errmsg("Parser error: ");
		errmsg.append(e.what());
		emit statusChanged(errmsg,2);
		return out;
	}
	catch(std::exception e)
	{
		QString errmsg("Parser error: ");
		errmsg.append(e.what());
		emit statusChanged(errmsg,2);
		return out;
	}
	return out;
}

void DataBackend::createObject(QDomNode d_)
{
	QDomElement d=d_.toElement();
	DrawableObject * f_=new DrawableObject();
	QString name = d.attribute("name");
	QDomNodeList constantL = d.elementsByTagName("fizzix_constant");
	FizObject * f=((FizObject *)(f_));
	for(int i=0; i<constantL.size(); i++)
	{
		QDomElement dp=(constantL.at(i)).toElement();
		f->setProperty(dp.attribute("name").toStdString(),Parser::parseFizdatum(dp.text(),((Type)(dp.attribute("type").toInt()))));
	}
	if(f->contains("SYSTEM_preset_geomtype"))
	{
		int c=(int)(f->getProperty("SYSTEM_preset_geomtype").scalar);
		switch(c)
		{
			case PRISM:
				f->setVertices(DrawableObject::makePrism(f->getProperty("SYSTEM_width").scalar,
									   f->getProperty("SYSTEM_length").scalar,f->getProperty("SYSTEM_height").scalar));
				break;
			case CYLINDER:
				f->setVertices(DrawableObject::makeCylinder(f->getProperty("SYSTEM_radius").scalar,
									   f->getProperty("SYSTEM_height").scalar));
				break;
			case SPHERE:
				f->setVertices(DrawableObject::makeSphere(f->getProperty("SYSTEM_radius").scalar));
				break;
		};
	}
	for(int i=0; i<constantL.size(); i++)
	{
		QDomElement dp=(constantL.at(i)).toElement();
		f->setProperty(dp.attribute("name").toStdString(),Parser::parseFizdatum(dp.text(),((Type)(dp.attribute("type").toInt()))));
	}
	objects->setElement(name, f_);
}

void DataBackend::createForce(QDomNode d_)
{
	QDomElement d=d_.toElement();
	QString name = d.attribute("name");
	int i=0;
	forces->setElement(name, new FizForce(FizFormula(Parser::parse(d.text(),i))));
}

void DataBackend::createMacro(QDomNode d_)
{
	if(d_.parentNode().nodeName()=="fizzix_force") return;
	QDomElement d=d_.toElement();
	QString name = d.attribute("name");
	int i=0;
	macros->setElement(name, new FizFormula(Parser::parse(d.text(),i)));
}

void DataBackend::createConstant(QDomNode d_)
{
	if(d_.parentNode().nodeName()=="fizzix_object") return;
	QDomElement d=d_.toElement();
	QString name = d.attribute("name");
	constants->setElement(name, Parser::parseFizdatum(d.text(),((Type)(d.attribute("type").toInt()))));
}

QDomNode DataBackend::createObjectNode(QString name, DrawableObject n, QDomDocument * d)
{
	QDomElement out=d->createElement("fizzix_object");
	out.setAttribute("name",name);
	std::vector<std::string> v=n.getSetProperties();
	for(unsigned int i=0; i<v.size(); i++)
		out.appendChild(createConstantNode(QString::fromStdString(v.at(i)),n[v.at(i)],d));
	return out;
}

QDomNode DataBackend::createConstantNode(QString name, fizdatum n, QDomDocument * d)
{
	QDomElement out=d->createElement("fizzix_constant");
	out.setAttribute("name",name);
	out.setAttribute("type",((int)n.type));
	out.appendChild(d->createTextNode(Parser::parseFizdatum(n)));
	return out;
}

QDomNode DataBackend::createForceNode(QString name, FizForce n, QDomDocument * d)
{
	QDomElement out=d->createElement("fizzix_force");
	out.setAttribute("name",name);
	out.appendChild(createFormulaNode("internal_formula",n.getFormula(),d));
	return out;
}

QDomNode DataBackend::createFormulaNode(QString name, FizFormula n, QDomDocument * d)
{
	QDomElement out=d->createElement("fizzix_formula");
	out.setAttribute("name",name);

	out.appendChild(d->createTextNode(QString::fromStdString(n.toString())));
	return out;
}

void DataBackend::saveDataAsXML(QString filename)
{
	dataChanges->setClean();
	QFile f (filename);
	if(f.exists())
		if(QMessageBox::question(NULL, "Overwrite file?", "This file already exists. Overwrite?",QMessageBox::Yes|QMessageBox::No)==QMessageBox::No)
			return;

	QDomDocument * d_=new QDomDocument("fizzix_document");
	QDomElement d=d_->createElement("fizzix_project");
	QList<QString> k=objects->getData()->keys();
	for(int i=0; i<k.size(); i++)
		d.appendChild(createObjectNode(k.at(i),*(objects->getData()->value(k.at(i))),d_));
	k=forces->getData()->keys();
	for(int i=0; i<k.size(); i++)
		d.appendChild(createForceNode(k.at(i),*(forces->getData()->value(k.at(i))),d_));
	k=macros->getData()->keys();
	for(int i=0; i<k.size(); i++)
		d.appendChild(createFormulaNode(k.at(i),*(macros->getData()->value(k.at(i))),d_));
	k=constants->getData()->keys();
	for(int i=0; i<k.size(); i++)
		d.appendChild(createConstantNode(k.at(i),constants->getData()->value(k.at(i)),d_));
	d_->appendChild(d);
	if(f.open(QIODevice::WriteOnly))
	{
		QTextStream ts (&f);
		ts<<d_->toString();
		f.close();
	}
	delete d_;
}

void DataBackend::loadDataFromXML(QString filename)
{
	QDomDocument doc("fizzix project");
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
	qDebug()<<"cannot open: "<<filename;
		return;
	}
	QString error;
	if (!doc.setContent(&file,false,&error))
	{
	qDebug()<<"cannot parse: "<<error;
		file.close();
		return;
	}
	file.close();
	QDomElement docElem = doc.documentElement();
	QDomNodeList objectL = docElem.elementsByTagName("fizzix_object");
	QDomNodeList forceL = docElem.elementsByTagName("fizzix_force");
	QDomNodeList macroL = docElem.elementsByTagName("fizzix_formula");
	QDomNodeList constantL = docElem.elementsByTagName("fizzix_constant");
	for(int i=0; i<objectL.size(); i++)
		createObject(objectL.at(i));
	for(int i=0; i<forceL.size(); i++)
		createForce(forceL.at(i));
	for(int i=0; i<macroL.size(); i++)
		createMacro(macroL.at(i));
	for(int i=0; i<constantL.size(); i++)
		createConstant(constantL.at(i));
}

void DataBackend::saveAs()
{
	QString filename = QFileDialog::getSaveFileName(NULL, "Save Project",QString(),"*.xml");
	if(filename.isEmpty()) return;
	else 
	{
		currentName=filename;
		saveDataAsXML(filename);
	}
}

void DataBackend::save()
{
	if(currentName.isEmpty())
		saveAs();
	else
		saveDataAsXML(currentName);
}

void DataBackend::load()
{
	if(!checkBeforeDataUnload()) return;
	emit(unloadData());
	dataLock=false;
	emit(dataLocked(false));
	currentName="";
	lastChangeUnpredictable=false;
	emit(gainedUnpredicatableChanges(lastChangeUnpredictable));
	unpredictableChange=0;
	purgeData();
	QString filename = QFileDialog::getOpenFileName(NULL, "Load Project",QString(),"*xml");
	if(filename.isEmpty()) return;
	else 
	{
		loadDataFromXML(filename);
	}
}

void DataBackend::newFromBlank()
{
	if(!checkBeforeDataUnload()) return;
	emit(unloadData());
	dataLock=false;
	emit(dataLocked(false));
	currentName="";
	lastChangeUnpredictable=false;
	emit(gainedUnpredicatableChanges(lastChangeUnpredictable));
	unpredictableChange=0;
	purgeData();
}

void DataBackend::newFromDefault()
{
	if(!checkBeforeDataUnload()) return;
	emit(unloadData());
	dataLock=false;
	emit(dataLocked(false));
	currentName="";
	lastChangeUnpredictable=false;
	emit(gainedUnpredicatableChanges(lastChangeUnpredictable));
	unpredictableChange=0;
	purgeData();
	loadDataFromXML(":data/default.xml");
}

void DataBackend::purgeData()
{
	QList<QString> k=objects->getData()->keys();
	for(int i=0; i<k.size(); i++)
		delete objects->getData()->value(k.at(i));
	k=forces->getData()->keys();
	for(int i=0; i<k.size(); i++)
		delete forces->getData()->value(k.at(i));
	k=macros->getData()->keys();
	for(int i=0; i<k.size(); i++)
		delete macros->getData()->value(k.at(i));
	objects->purge();
	forces->purge();
	macros->purge();
	constants->purge();
	dataChanges->clear();
}

void DataBackend::setWarning(bool b)
{warnClear=b;}

bool DataBackend::checkBeforeDataUnload()
{
	if(dataChanges->isClean()) return true;
	switch(QMessageBox::question(NULL, "Dicard changes?", 
			"You have made changes to the current project. Do you wish to save them?",
			QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
	{
		case QMessageBox::Yes:
			if(currentName.isEmpty())
			{
				QString filename = QFileDialog::getSaveFileName(NULL, "Save Project",QString(),tr("*xml"));
				if(filename.isEmpty()) return false;
				else saveDataAsXML(filename);
			}
			else save();
			return true;
		case QMessageBox::No:
			return true;
		default: return false;
	}
}

void DataBackend::quit()
{
	if(!checkBeforeDataUnload()) return;
	QCoreApplication::instance()->quit();
}
#endif

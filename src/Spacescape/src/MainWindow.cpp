/* 
This source file is part of Spacescape
For the latest info, see http://alexcpeterson.com/spacescape

"He determines the number of the stars and calls them each by name. "
Psalm 147:4

The MIT License

Copyright (c) 2010 Alex Peterson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <QSettings>
#include <QProgressDialog>
#include <QFileDialog>

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include "PropertyBrowser/QtFilePathProperty.h"


#include <Ogre.h>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AboutDialog.h"
#include "ExportFileDialog.h"

using namespace spacescape;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    mFilename(""),
    mRefreshing(false)
{
    setAttribute(Qt::WA_NativeWindow);

	// we store last settings used in registry/settings
	QSettings settings;

    mLastExportDir = settings.value("LastExportDir","../../../export/").toString();
	mLastOpenDir = settings.value("LastOpenDir","../../../save/").toString();
	mLastSaveDir = settings.value("LastSaveDir","../../../save/").toString();

    // setup the ui
    ui->setupUi(this);

    QFile stylesheetFile(":/spacescape/resources/stylesheet.qss");
    if (stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setStyleSheet(stylesheetFile.readAll());
    }

    // set the property manager to a variant type (changable/editable - not read only)
    mPropertyManager = new QtVariantPropertyManager();
    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory();
    ui->layerProperties->setFactoryForManager(mPropertyManager, variantFactory);
    ui->layerProperties->setPropertiesWithoutValueMarked(true);

    // we want the little arrows on root properties to indicate they're expandable
    ui->layerProperties->setRootIsDecorated(true);

    // set us as a progress listener
    ui->previewWindow->setProgressListener(this);

    // add a signal for when properties are changed
    connect(mPropertyManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(valueChanged(QtProperty *, const QVariant &)));

    // add a signal to detect when layer items change so we can update status bar
    // with a tip
    connect(ui->layerProperties, SIGNAL(currentItemChanged(QtBrowserItem*)),
            this,SLOT(currentItemChanged(QtBrowserItem*)));

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::createNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);


    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionExport, &QAction::triggered, this, &MainWindow::onExport);

    connect(ui->actionShowDebugBox, &QAction::triggered, this, &MainWindow::showDebugBox);
    connect(ui->actionEnableHDR, &QAction::triggered, this, &MainWindow::enableHDR);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    connect(ui->newLayerButton, &QAbstractButton::clicked, this, &MainWindow::createNewLayer);
    connect(ui->copyLayerButton, &QAbstractButton::clicked, this, &MainWindow::copySelectedLayer);
    connect(ui->deleteLayerButton, &QAbstractButton::clicked, this, &MainWindow::deleteSelectedLayer);
    connect(ui->moveLayerDownButton, &QAbstractButton::clicked, this, &MainWindow::moveSelectedLayerDown);
    connect(ui->moveLayerUpButton, &QAbstractButton::clicked, this, &MainWindow::moveSelectedLayerUp);

    mPropertyTitles["destBlendFactor"] = QString("Dest Blend Factor");
    mPropertyTitles["ditherAmount"] = QString("Dither Amount");
    mPropertyTitles["farColor"] = QString("Far Color");
    mPropertyTitles["gain"] = QString("Gain");
    mPropertyTitles["innerColor"] = QString("Inner Color");
    mPropertyTitles["lacunarity"] = QString("Lacunarity");
    // in non-hdr mode color fades linearly based on distance
    // but in hdr mode we may want other options
    mPropertyTitles["hdrPower"] = QString("HDR Power");
    
    // option multiplier to use in HDR mode
    mPropertyTitles["hdrMultiplier"] = QString("HDR Multiplier");
    mPropertyTitles["maskEnabled"] = QString("Mask Enabled");
    mPropertyTitles["maskGain"] = QString("Mask Gain");
    mPropertyTitles["maskInnerColor"] = QString("Mask Inner Color");
    mPropertyTitles["maskLacunarity"] = QString("Mask Lacunarity");
    mPropertyTitles["maskNoiseType"] = QString("Mask Noise Type");
    mPropertyTitles["maskOffset"] = QString("Mask Offset");
    mPropertyTitles["maskOctaves"] = QString("Mask Octaves");
    mPropertyTitles["maskOuterColor"] = QString("Mask Outer Color");
    mPropertyTitles["maskPower"] = QString("Mask Power");
    mPropertyTitles["maskScale"] = QString("Mask Noise Scale");
    mPropertyTitles["maskSeed"] = QString("Mask Random Seed");
    mPropertyTitles["maskThreshold"] = QString("Mask Threshold");
    mPropertyTitles["maxSize"] = QString("Near Billboard Size");
    mPropertyTitles["minSize"] = QString("Far Billboard Size");
    mPropertyTitles["name"] = QString("Layer Name");
    mPropertyTitles["nearColor"] = QString("Near Color");
    mPropertyTitles["noiseType"] = QString("Noise Type");
    mPropertyTitles["numBillboards"] = QString("Number of Billboards");
    mPropertyTitles["dataFile"] = QString("Data File");
    mPropertyTitles["numPoints"] = QString("Number of Points");
    mPropertyTitles["octaves"] = QString("Octaves");
    mPropertyTitles["offset"] = QString("Noise Offset");
    mPropertyTitles["outerColor"] = QString("Outer Color");
    mPropertyTitles["pointSize"] = QString("Point Size");
    mPropertyTitles["powerAmount"] = QString("Power");
    mPropertyTitles["previewTextureSize"] = QString("Preview Texture Size");
    mPropertyTitles["texture"] = QString("Billboard Texture");
    mPropertyTitles["type"] = QString("Layer Type");
    mPropertyTitles["scale"] = QString("Noise Scale");
    mPropertyTitles["seed"] = QString("Random Seed");
    mPropertyTitles["shelfAmount"] = QString("Threshold");
    mPropertyTitles["sourceBlendFactor"] = QString("Source Blend Factor");
    mPropertyTitles["visible"] = QString("Layer Visible");
    
    mDebugLayerLoaded = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    if(!mDebugLayerLoaded &&
       ui->previewWindow->pluginReady()) {
#ifdef _DEBUG
         Ogre::NameValuePairList params;
         ui->previewWindow->addLayer(1,params);
         
         // insert the new layer
         insertLayerProperties(ui->previewWindow->getLayers().back());
#endif
        mDebugLayerLoaded = true;
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QtVariantProperty* MainWindow::createProperty(const Ogre::String& key, const Ogre::String& value)
{
    QStringList noiseTypes, layerTypes, blendTypes, textureSizes, billboardTextures;
    noiseTypes << "fbm" << "ridged";
    layerTypes << "points" << "billboards" << "noise";
    blendTypes << "one" << "zero" << "dest_colour" << "src_colour" 
        << "one_minus_dest_colour" << "one_minus_src_colour" 
        << "dest_alpha" << "src_alpha" << "one_minus_dest_alpha" 
        << "one_minus_src_alpha";
    textureSizes << "256" << "512" << "1024" << "2048" << "4096";
    if(ui->previewWindow->isHDREnabled()) {
        billboardTextures << "hdr-flare-blue.exr" << "hdr-flare-red.exr" << "hdr-flare-yellow.exr" << "hdr-flare-purple.exr" << "hdr-flare-white.exr"  << "hdr-flare-white2.exr";
        billboardTextures << "hdr-flare-blue-2spires.exr" << "hdr-flare-blue-4spires.exr" << "hdr-flare-blue1.exr" << "hdr-flare-bluepurple-4spire.exr" << "hdr-flare-bluepurple-multispire.exr" << "hdr-flare-pink1.exr" << "hdr-flare-red-2spires.exr" << "hdr-flare-redpurple-multispire.exr";
    }
    else {
        billboardTextures << "default.png" << "flare-blue-purple1.png" << "flare-blue-purple2.png" << "flare-blue-purple3.png" << "flare-blue-spikey1.png" << "flare-green1.png" << "flare-inverted-blue-purple3.png" << "flare-red-yellow1.png" << "flare-red1.png" << "flare-white-small1.png" << "sun.png";
    }
    int propertyType = getPropertyType(key);
    QtVariantProperty* property;

    property = mPropertyManager->addProperty(propertyType, getPropertyTitle(key));
    property->setStatusTip(getPropertyStatusTip(key));
    property->setToolTip(getPropertyStatusTip(key));

    if(propertyType == QVariant::Int) {
        property->setValue(Ogre::StringConverter::parseInt(value));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("singleStep"), 1);
    }
    else if(propertyType == QVariant::Bool) {
        property->setValue(Ogre::StringConverter::parseBool(value));
    }
    else if(propertyType == QtVariantPropertyManager::enumTypeId()) {
        QStringList *enumList = NULL;
        if(key == "destBlendFactor" || key == "sourceBlendFactor") {
            enumList = &blendTypes;
        }
        else if(key == "type") {
            enumList = &layerTypes;
        }
        else if(key == "noiseType" || key == "maskNoiseType") {
            enumList = &noiseTypes;
        }
        else if(key == "previewTextureSize") {
            enumList = &textureSizes;    
        }
/*
#ifdef Q_WS_MAC
        else if(key == "texture") {
            enumList = &billboardTextures;
        }
#endif
*/
        property->setAttribute(QLatin1String("enumNames"), *enumList);

        int valueIndex = 0;

        // find the selected value
        for(int i = 0; i < enumList->size(); i++) {
            if((*enumList)[i] == QString(value.c_str())) {
                valueIndex = i;
                break;
            }
        }
        
        property->setValue(valueIndex);
    }
    else if(propertyType == QVariant::Double) {
		property->setValue(Ogre::StringConverter::parseReal(value));
        property->setAttribute(QLatin1String("singleStep"), 0.01);
        property->setAttribute(QLatin1String("decimals"), 3);
    }
    else if(propertyType == QVariant::Color) {
       property->setValue(getColor(value));
    }
    else {
        // assume string
        property->setValue(value.c_str());
    }

    return property;
}

void MainWindow::currentItemChanged(QtBrowserItem *item)
{
    // a hack to prevent invalid item checking when moving layers around
    if(mRefreshing) return;

    if(item && item->property() && !item->property()->statusTip().isNull()) {
        ui->statusBar->showMessage(item->property()->statusTip());
    }
}

int MainWindow::getBlendMode(const QString& param)
{
    if (param == "one")
        return 0;
    else if (param == "zero")
        return 1;
    else if (param == "dest_colour")
        return 2;
    else if (param == "src_colour")
        return 3;
    else if (param == "one_minus_dest_colour")
        return 4;
    else if (param == "one_minus_src_colour")
        return 5;
    else if (param == "dest_alpha")
        return 6;
    else if (param == "src_alpha")
        return 7;
    else if (param == "one_minus_dest_alpha")
        return 8;
    else if (param == "one_minus_src_alpha")
        return 9;
    else
        return 0;
}

QColor MainWindow::getColor(const Ogre::String& color)
{
    // first convert to a colourvalue
    Ogre::ColourValue c = Ogre::StringConverter::parseColourValue(color);
    return QColor(
        (int)floor(c.r * 255.0),
        (int)floor(c.g * 255.0),
        (int)floor(c.b * 255.0),
        (int)floor(c.a * 255.0)
    );
}

#include <string>
Ogre::ColourValue MainWindow::getColor(const QString& color)
{
	Ogre::StringVector vec = Ogre::StringUtil::split(color.toStdString());
    double scale = 1.0 / 255.0;

    return Ogre::ColourValue(
        floor(Ogre::StringConverter::parseReal(vec[0]) * scale),
        floor(Ogre::StringConverter::parseReal(vec[1]) * scale),
        floor(Ogre::StringConverter::parseReal(vec[2]) * scale),
        floor(Ogre::StringConverter::parseReal(vec[3]) * scale)
    );
}

Ogre::String MainWindow::getColor(QColor color)
{
    float scale = (float)1.0 / (float)255.0;
    return Ogre::StringConverter::toString(color.red() * scale) + " " +
        Ogre::StringConverter::toString(color.green() * scale) + " " +
        Ogre::StringConverter::toString(color.blue() * scale) + " " +
        Ogre::StringConverter::toString(color.alpha() * scale);
}

QLatin1String MainWindow::getPropertyStatusTip(const Ogre::String& prop)
{
    if(prop == "name") {
        return QLatin1String("Layer name. What? Spacescapelayer0 isn't descriptive enough for you?!");
    }
    else if(prop == "destBlendFactor") {
        return QLatin1String("Destination blend factor.");
    }
    else if(prop == "ditherAmount") {
        return QLatin1String("Add additional fine grain noise to help with banding in the noise gradients - most noticable in dark colors.");
    }
    else if(prop == "gain" || prop == "maskGain") {
        return QLatin1String("A multiplier that determines how quickly the amplitudes diminish for each successive octave.");
    }
    else if(prop == "lacunarity" || prop == "maskLacunarity") {
        return QLatin1String("A multiplier that determines how quickly the frequency increases for each successive octave.");
    }
    else if(prop == "hdrPower") {
        return QLatin1String("How distance affects the transition between near and far colours.");
    }
    else if(prop == "hdrMultiplier") {
        return QLatin1String("Multiply the final by this value.");
    }
    else if(prop == "dataFile") {
        return QLatin1String("A CSV file with x,y,z,distance (in parsecs), mag (magnitude), BV fields.");
    }
    else if(prop == "octaves" || prop == "maskOctaves") {
        return QLatin1String("Number of noise functions in a series of noise functions that are added together.");
    }
    else if(prop == "noiseType" || prop == "maskNoiseType") {
        return QLatin1String("Noise type can be either FBM noise (smooth) or Ridged FBM noise.");
    }
    else if(prop == "offset" || prop == "maskOffset") {
        return QLatin1String("I honestly don't know exactly how this effects ridged noise, but it is only used for ridged noise. Have fun!");
    }
    else if(prop == "scale" || prop == "maskScale") {
        return QLatin1String("Multiplier applied to initial noise coordinates.");
    }
    else if(prop == "power" || prop == "maskPower") {
        return QLatin1String("The final noise value is raised to this power.  Useful for changing the noise gradient slope.");
    }
    else if(prop == "seed") {
        return QLatin1String("This number is used as the basis for the random number generator.");
    }
    else if(prop == "maskEnabled") {
        return QLatin1String("Makes points or billboards appear in greater density where noise values are higher.");
    }
    else if(prop == "maskSeed") {
        return QLatin1String("This number is used as the basis for the random number generator for the noise mask.");
    }
    else if(prop == "sourceBlendFactor") {
        return QLatin1String("Source blend factor.");
    }
    else if(prop == "type") {
        return QLatin1String("Layer type: Point, Billboards or Noise.");
    }
    else if(prop == "visible") {
        return QLatin1String("Show or hide this layer.");
    }
    else {
		// made this a space because if you just return "" then the value
		// does not get updated because the default value is ""
        return QLatin1String(" ");
    }
}

QString MainWindow::getPropertyTitle(const Ogre::String& prop)
{
    if(mPropertyTitles[prop].isEmpty()) {
        return QString(prop.c_str());
    }
    else {
        return mPropertyTitles[prop];
    }
}

Ogre::String MainWindow::getProperty(const QString& prop)
{
    std::map<Ogre::String,QString>::iterator ii;
    for(ii = mPropertyTitles.begin(); ii != mPropertyTitles.end(); ii++) {
        if(ii->second == prop) {
            return ii->first;
        }
    }

    return Ogre::String(QString(prop).toStdString());
}

int MainWindow::getPropertyType(const Ogre::String& name)
{
    if(name == "seed" || 
        name == "octaves" ||
        name == "maskSeed" ||
        name == "maskOctaves" ||
        name == "numBillboards" ||
        name == "numPoints" ||
        name == "pointSize" ) {
            return QVariant::Int;
    }
    else if(name == "visible" ||
        name == "maskEnabled"
        ) {
            return QVariant::Bool;
    }
    else if(name == "type" ||
        name == "destBlendFactor" ||
        name == "sourceBlendFactor" ||
        name == "noiseType" ||
        name == "previewTextureSize" ||
//#ifdef Q_WS_MAC
//        name == "texture" ||
//#endif
        name == "maskNoiseType") {
            return QtVariantPropertyManager::enumTypeId();
    }
    else if(name == "name" ||
            name == "texture" ||
			name == "dataFile") {
        return QVariant::String;
    }
    else if(name == "innerColor" ||
        name == "outerColor" ||
        name == "farColor" ||
        name == "nearColor") {
            return QVariant::Color;
    }
    else {
        // assume double
        return QVariant::Double;
    }
}

int MainWindow::getSelectedLayerId()
{
    // get the current selected item
    QtBrowserItem* bi = ui->layerProperties->currentItem();
    if(!bi) {
        return -1;
    }

    // get the top level layer this item belongs to
    if(bi->parent()) {
        while(bi->parent()) {
            bi = bi->parent();
        }
    }

    int index = ui->layerProperties->topLevelItems().indexOf(bi);
    if(index > -1) {
        return ui->layerProperties->topLevelItems().size() - 1 - index;
    }

    return -1;
}

QtProperty* MainWindow::insertLayerProperties(Ogre::SpacescapeLayer* layer, QtProperty *insertAfter, bool minimize)
{
    // turn refreshing flag on so we don't process valueChanged events
    mRefreshing = true;

    // get layer params
    Ogre::NameValuePairList params = layer->Params();

    // create the layer properties object
    QtProperty *layerProperties = mPropertyManager->addProperty(
                QtVariantPropertyManager::groupTypeId(),
                QLatin1String(layer->getName().c_str())
    );

    // insert it into the property tree early so we can minize items inside
    ui->layerProperties->insertProperty(layerProperties, insertAfter);

    // minimize the layer - speeds things up!
    if(minimize) {
        ui->layerProperties->setExpanded(ui->layerProperties->topLevelItem(layerProperties), false);
    }

    // add the common layer params to the subproperties first
    layerProperties->addSubProperty(createProperty( "name", layer->getName()));
    layerProperties->addSubProperty(createProperty( "type", layer->LayerTypeName()));
    layerProperties->addSubProperty(createProperty( "visible", "true"));
    layerProperties->addSubProperty(createProperty( "seed", params["seed"]));

    // now add all the remaining layer params to the subproperties
    Ogre::NameValuePairList::iterator pl;
    for(pl = params.begin(); pl != params.end(); pl++) {
        // skip common params that come first
        if(pl->first == "name" || pl->first == "type" || 
            pl->first == "visible" || pl->first == "seed") {
            continue;
        }
        
        // skip hdr stuff if not enabled
        if(pl->first == "hdrMultiplier" || pl->first == "hdrPower") {
            if(!ui->previewWindow->isHDREnabled())
                continue;
        }

        if(pl->first == "dataFile") {
            QtFilePathManager *mgr = new QtFilePathManager;
            QtProperty *pathProperty = mgr->addProperty("Data File");
			mgr->setValue(pathProperty, QLatin1String(pl->second.c_str()));
            mgr->setFilter(pathProperty, "Source files (*.csv)");
            QtFileEditFactory *fact = new QtFileEditFactory;
            ui->layerProperties->setFactoryForManager(mgr, fact);
            layerProperties->addSubProperty(pathProperty);
            
            // add a signal for when properties are changed
            connect(mgr, SIGNAL(valueChanged(QtProperty *, const QString &)),
                    this, SLOT(valueChanged(QtProperty *, const QString &)));
        }
		else if (pl->first == "texture") {
			QtFilePathManager *mgr = new QtFilePathManager;
			QtProperty *pathProperty = mgr->addProperty("Billboard Texture");
			mgr->setValue(pathProperty, QLatin1String(pl->second.c_str()));
			mgr->setFilter(pathProperty, "Image files (*.png *.dds *.jpg *.tga *.exr)");
			QtFileEditFactory *fact = new QtFileEditFactory;
			ui->layerProperties->setFactoryForManager(mgr, fact);
			layerProperties->addSubProperty(pathProperty);

			// add a signal for when properties are changed
			connect(mgr, SIGNAL(valueChanged(QtProperty *, const QString &)),
				this, SLOT(valueChanged(QtProperty *, const QString &)));
		}
        else {
            // create the sub property
            QtVariantProperty* subProperty = createProperty(pl->first, pl->second);
            if(!subProperty) {
                continue;
            }

            // add this sub property parameter
            layerProperties->addSubProperty(subProperty);

            // special auto hide for color types
            if(getPropertyType(pl->first) == QVariant::Color) {
                QList<QtBrowserItem *> bi = ui->layerProperties->items(subProperty);
                ui->layerProperties->setExpanded(bi.first(),false);
            }
        }
    }

    // done adding properties
    mRefreshing = false;

    return layerProperties;
}

void MainWindow::createNew()
{
    // TODO: prompt to save first
    ui->previewWindow->clearLayers();
    refreshProperties();
}


void MainWindow::openFile()
{
    QString filename = QFileDialog::getOpenFileName(
            this,
            QLatin1String("Open Spacescape .xml file"),
            mLastOpenDir,
            QLatin1String("XML Files (*.xml)")
    );

    // disable ogre window till done opening to prevent crashes
    ui->previewWindow->setDisabled(true);

    if(!filename.isNull() && !filename.isEmpty()) {
        QFileInfo fi(filename);
        mLastOpenDir = fi.absolutePath();
        mLastSaveDir = mLastOpenDir;

        // save in the registry
        QSettings settings;
        settings.setValue("LastOpenDir",mLastOpenDir);
        settings.setValue("LastSaveDir",mLastSaveDir);

        ui->statusBar->showMessage("Opening " + filename + " ...");

        // open the progress dialog
        // ui->mProgressDialog->setValue(0);
        // ui->mProgressDialog->show();

        if(ui->previewWindow->open(filename)) {
            mFilename = filename;

            QString title = "Spacescape - " + fi.completeBaseName() + "." + fi.completeSuffix();
            setWindowTitle(QApplication::translate("MainWindow", title.toStdString().c_str(), 0));

            // file opened successfully so clear property list and start over
            refreshProperties();

            ui->statusBar->showMessage("Opened " + filename,3000);
        }
        else {
            ui->statusBar->showMessage("Failed to load " + filename,3000);
        }
    }

    ui->previewWindow->setDisabled(false);
}

void MainWindow::save()
{
    if(mFilename.isEmpty() || mFilename.isNull()) {
        mFilename = QFileDialog::getSaveFileName(
                this,
                "Save As...",
                mLastSaveDir,
                QLatin1String("XML Files(*.xml)")
        );
    }

    if(!mFilename.isNull()) {
        if(mFilename.isEmpty()) {
            // pop up an error - no empty filenames allowed
        }
        else {
            QFileInfo fi(mFilename);
            mLastSaveDir = fi.absolutePath();

            // save the last save dir in the registry
            QSettings settings;
            settings.setValue("LastSaveDir",mLastSaveDir);

            ui->statusBar->showMessage("Saving " + mFilename);

            // save this file!
            if(ui->previewWindow->save(mFilename)) {
                QString title = "Spacescape - " + fi.completeBaseName() + "." + fi.completeSuffix();
                setWindowTitle(QApplication::translate("MainWindow", title.toStdString().c_str(), 0));

                ui->statusBar->showMessage("Saved " + mFilename,3000);
            }
            else {
                ui->statusBar->showMessage("Failed to save " + mFilename,3000);
            }
        }
    }
}

void MainWindow::saveAs()
{
    mFilename = QFileDialog::getSaveFileName(
            this,
            "Save As...",
            mLastSaveDir,
            QLatin1String("XML Files(*.xml)")
    );

    if(!mFilename.isNull()) {
        if(mFilename.isEmpty()) {
            // pop up an error - no empty filenames allowed
        }
        else {
            QFileInfo fi(mFilename);
            mLastSaveDir = fi.absolutePath();

            // save the last save dir in the registry
            QSettings settings;
            settings.setValue("LastSaveDir",mLastSaveDir);

            // save this file!
            if(ui->previewWindow->save(mFilename)) {
                QString title = "Spacescape - " + fi.completeBaseName() + "." + fi.completeSuffix();
                setWindowTitle(QApplication::translate("MainWindow", title.toStdString().c_str(), 0));
            }
        }
    }
}

void MainWindow::showDebugBox()
{
    static bool debugVisible = false;
    debugVisible = !debugVisible;
    ui->previewWindow->setDebugBoxVisible(debugVisible);
    if(debugVisible) {
        ui->actionShowDebugBox->setText(tr("Hide Debug Box"));
    } else {
        ui->actionShowDebugBox->setText(tr("Show Debug Box"));
    }
}

void MainWindow::enableHDR()
{
    static bool hdrEnabled = false;
    hdrEnabled = !hdrEnabled;

    ui->previewWindow->setHDREnabled(hdrEnabled);
    if(hdrEnabled) {
        ui->actionEnableHDR->setText(tr("Disable HDR"));
    } else {
        ui->actionEnableHDR->setText(tr("Enable HDR"));
    }

    refreshProperties();
}

void MainWindow::showAboutDialog()
{
    AboutDialog d;
    d.exec();
}

void MainWindow::createNewLayer()
{
    ui->statusBar->showMessage("Creating new layer...");

    Ogre::NameValuePairList params;
    ui->previewWindow->addLayer(0,params);

    ui->statusBar->showMessage("Creating new layer properties list...");

    insertLayerProperties(ui->previewWindow->getLayers().back());

    ui->statusBar->showMessage("New layer created",3000);
}

void MainWindow::copySelectedLayer()
{
    // copy this layer
    int layerId = getSelectedLayerId();
    if(layerId != -1) {
        int newLayerId = ui->previewWindow->copyLayer(layerId);

        // insert the new layer
        QList<QtBrowserItem *> bl = ui->layerProperties->topLevelItems();
        if((bl.size() - layerId - 1) == 0) {
            // insert at beginning of properties list
            insertLayerProperties(ui->previewWindow->getLayers()[newLayerId]);
        } else {
            // insert before the copied property
            insertLayerProperties(ui->previewWindow->getLayers()[newLayerId], ui->layerProperties->properties()[bl.size() - layerId - 2]);
        }
    }
}

void MainWindow::deleteSelectedLayer()
{
    // get the selected layer
    int layerId = getSelectedLayerId();
    if(layerId != -1) {
        ui->previewWindow->deleteLayer(layerId);
        QList<QtBrowserItem*> bl = ui->layerProperties->topLevelItems();
        ui->layerProperties->removeProperty(bl[bl.size() - layerId - 1]->property());
    }
}

void MainWindow::moveSelectedLayerDown()
{
    // get the selected layer
    int layerId = getSelectedLayerId();
    if(layerId > -1) {
        if(ui->previewWindow->moveLayerDown(layerId)) {
            QList<QtBrowserItem *> bl = ui->layerProperties->topLevelItems();
            const int index = bl.size() - layerId - 1;
            QtProperty *p = bl[index]->property();

            // save expanded settings
            bool expanded = ui->layerProperties->isExpanded(bl[index]);

            // remove and re-insert at the new location
            ui->layerProperties->removeProperty(p);
            ui->layerProperties->insertProperty(p,bl[index + 1]->property());

            // re-apply property tree visiblity settings
            bl = ui->layerProperties->topLevelItems();
            ui->layerProperties->setExpanded(bl[index + 1],expanded);

            // un-expand the color items
            QList<QtProperty *> sl = p->subProperties();
            for(const auto& i : sl) {
                if(((QtVariantProperty*)i)->propertyType() == QVariant::Color) {
                    QList<QtBrowserItem *> bi = ui->layerProperties->items(i);
                    ui->layerProperties->setExpanded(bi.first(),false);
                }
            }

            // re-select the item
            ui->layerProperties->setFocus();
            bl = ui->layerProperties->topLevelItems();
            ui->layerProperties->setCurrentItem(bl[index + 1]);
        }
    }
}

void MainWindow::moveSelectedLayerUp()
{
    // get the selected layer
    int layerId = getSelectedLayerId();
    if(layerId > -1) {
        if(ui->previewWindow->moveLayerUp(layerId)) {
            QList<QtBrowserItem *> bl = ui->layerProperties->topLevelItems();
            const int index = bl.size() - layerId - 1;
            QtProperty *p = bl[index]->property();

            // save expanded settings
            bool expanded = ui->layerProperties->isExpanded(bl[index]);

            // remove and re-insert at the new location
            ui->layerProperties->removeProperty(p);
            if(index == 1) {
                ui->layerProperties->insertProperty(p, NULL);
            } else {
                ui->layerProperties->insertProperty(p,bl[index - 2]->property());
            }

            // re-apply property tree visiblity settings
            bl = ui->layerProperties->topLevelItems();
            ui->layerProperties->setExpanded(bl[index - 1],expanded);

            // un-expand the color items
            QList<QtProperty *> sl = p->subProperties();
            for(auto & i : sl) {
                if(((QtVariantProperty*)i)->propertyType() == QVariant::Color) {
                    QList<QtBrowserItem *> bi = ui->layerProperties->items(i);
                    ui->layerProperties->setExpanded(bi.first(),false);
                }
            }

            // re-select the item
            ui->layerProperties->setFocus();
            ui->layerProperties->setCurrentItem(bl[index - 1]);
        }
    }
}

void MainWindow::onExport()
{
    QString imageSize;
    QString selectedFilter;
    QString outputTypes;
    QSettings settings;
    QString orientation;
    
    if(ui->previewWindow->isHDREnabled()) {
        outputTypes = QLatin1String("6 EXR files(*.exr);;Single DDS Cube Map(*.dds)");
    } else {
        outputTypes = QLatin1String("6 PNG files(*.png);;6 JPG files(*.jpg);;6 TGA files(*.tga);;Single DDS Cube Map(*.dds)");
    }
    
    if(!settings.value("LastExportDir").isNull()) {
        mLastExportDir = settings.value("LastExportDir").toString();
    }
    if(!settings.value("selectedFilter").isNull()) {
        selectedFilter = settings.value("selectedFilter").toString();
    }
    if(!settings.value("imageSize").isNull()) {
        imageSize = settings.value("imageSize").toString();
    }
    if(!settings.value("orientation").isNull()) {
        orientation = settings.value("orientation").toString();
    }
    
	QString filename = ExportFileDialog::getExportFileName(
		this,
		"Export Skybox",
		mLastExportDir,
		outputTypes,
        &selectedFilter,
        0,
        &imageSize,
        &orientation
    );

    settings.setValue("selectedFilter", selectedFilter);
    settings.setValue("imageSize", imageSize);
    settings.setValue("orientation", orientation);
    
    // disable ogre window till done exporting to prevent crashes
    ui->previewWindow->setDisabled(true);

    if(!filename.isNull() && !filename.isEmpty()) {
        // make sure we have an extension on the filename
        QFileInfo fi(filename);
        if(fi.completeSuffix().isNull() || fi.completeSuffix().isEmpty()) {
            if(selectedFilter == "6 EXR files(*.exr)") {
                filename += ".exr";
            }
			else if (selectedFilter == "Single DDS Cube Map(*.dds)") {
				filename += ".dds";
            }
			else if (selectedFilter == "6 JPG files(*.jpg)") {
                filename += ".jpg";
            }
            else if (selectedFilter == "6 TGA files(*.tga)") {
                filename += ".tga";
            }
            else {
                filename += ".png";
            }
        }
        
        mLastExportDir = fi.absolutePath();

		// save the last export dir in the registry
		settings.setValue("LastExportDir",mLastExportDir);

        ui->statusBar->showMessage("Exporting skybox " + filename);

        bool cubeMap = selectedFilter == "Single DDS Cube Map(*.dds)" ;
        
        int skyboxOrientation = 0;
        if(orientation == "UNREAL") {
            skyboxOrientation = 1;
        }
        else if(orientation == "UNITY") {
            skyboxOrientation = 2;
        }
        else if(orientation == "SOURCE") {
            skyboxOrientation = 3;
        }
        
        // ogre can't export dds files doh!
		ui->previewWindow->exportSkybox(filename,
                                     imageSize.toUInt(),
                                     cubeMap,
                                     skyboxOrientation);

        ui->statusBar->showMessage("Exported skybox " + filename,3000);
    }

    ui->previewWindow->setDisabled(false);

}

void MainWindow::refreshProperties()
{
    // set refreshing properties flag so we don't process valueChanged events
    mRefreshing = true;

    // remove existing properties
    mPropertyManager->clear();

    // get all layers and add them to the property list in reverse order
    std::vector<Ogre::SpacescapeLayer *> layers = ui->previewWindow->getLayers();
    if(!layers.empty()) {
        QtProperty *layer = NULL;

        QString numLayers;
        numLayers.setNum(layers.size());
        for(int i = layers.size() - 1; i >= 0; i--) {
            QString layerNum;
            layerNum.setNum(layers.size() - i);
            //ui->statusBar->showMessage("Updating layer properties " + layerNum + " of " + numLayers);
            layer = insertLayerProperties(layers[i], layer);
        }
    }

    mRefreshing = false;
}

void MainWindow::updateProgressBar(unsigned int percentComplete, const Ogre::String& msg)
{
    // ui->mProgressDialog->setValue(percentComplete);
    // ui->mProgressDialog->setLabelText(QString(msg.c_str()));
    qApp->processEvents();
}

void MainWindow::valueChanged(QtProperty *property, const QVariant &value)
{
    // don't update if we're refreshing
    if(mRefreshing) return;

    QList<QtProperty *> l = ui->layerProperties->properties();

    int topLevelIndex = 0;
    int layerId = -1;

    // find the layer that contains this property
    for(; topLevelIndex < l.size(); topLevelIndex++) {
        if(l[topLevelIndex] == property) {
            layerId = l.size() - 1 - topLevelIndex;
            break;
        }

        bool found = false;

        QList<QtProperty *> sl = l[topLevelIndex]->subProperties();
        if(!sl.empty()) {
            for(int id = 0; id < sl.size(); id++) {
                if(sl[id] == property) {
                    layerId = l.size() - 1 - topLevelIndex;
                    found = true;
                    break;
                }
            }
        }

        if(found) break;
    }

    // did we find a valid layer id?
    std::vector<Ogre::SpacescapeLayer *> layers = ui->previewWindow->getLayers();
    if(layerId > -1 && layerId < (int)l.size() && layerId < (int)layers.size()) {
        Ogre::NameValuePairList params;
        Ogre::String propertyStr = getProperty(property->propertyName());
        bool refresh = false;

        if(property->propertyName().indexOf("color") != -1 ||
            property->propertyName().indexOf("Color") != -1) {
            params[propertyStr] = getColor(value.value<QColor>());
        }
        else if(propertyStr == "noiseType" || propertyStr == "maskNoiseType") {
            params[propertyStr] = value == "0" ? "fbm" : "ridged";
        }
/*
#ifdef Q_WS_MAC
        else if(propertyStr == "texture") {
            QStringList billboardTextures;
            if(ui->previewWindow->isHDREnabled()) {
                billboardTextures << "hdr-flare-blue.exr" << "hdr-flare-red.exr" << "hdr-flare-yellow.exr" << "hdr-flare-purple.exr" << "hdr-flare-white.exr" << "hdr-flare-white2.exr";
                billboardTextures << "hdr-flare-blue-2spires.exr" << "hdr-flare-blue-4spires.exr" << "hdr-flare-blue1.exr" << "hdr-flare-bluepurple-4spire.exr" << "hdr-flare-bluepurple-multispire.exr" << "hdr-flare-pink1.exr" << "hdr-flare-red-2spires.exr" << "hdr-flare-redpurple-multispire.exr";
            }
            else {
                billboardTextures << "default.png" << "flare-blue-purple1.png" << "flare-blue-purple2.png" << "flare-blue-purple3.png" << "flare-blue-spikey1.png" << "flare-green1.png" << "flare-inverted-blue-purple3.png" << "flare-red-yellow1.png" << "flare-red1.png" << "flare-white-small1.png" << "sun.png";
            }
            params[propertyStr] = Ogre::String(billboardTextures[value.toUInt()].toStdString());
        }
#endif
*/
        else if(propertyStr == "type") {
            QStringList layerTypes;
            layerTypes << "points" << "billboards" << "noise";
            params[propertyStr] = Ogre::String(layerTypes[value.toUInt()].toStdString());
            refresh = true;
        }
        else if(propertyStr == "previewTextureSize") {
            QStringList textureSizes;
            textureSizes << "64" << "128" << "256" << "512" << "1024" << "2048" << "4096" << "8192";
            params[propertyStr] = Ogre::String(textureSizes[value.toUInt()].toStdString());
        }
        else if(propertyStr == "name") {
            // set the layer name property
            ui->layerProperties->topLevelItems()[topLevelIndex]->property()->setPropertyName(value.toString());
            params[propertyStr] = Ogre::String(value.toString().toStdString());
        }
        else if(propertyStr == "destBlendFactor" || propertyStr == "sourceBlendFactor") {
            QStringList blendTypes;
            
            blendTypes << "one" << "zero" << "dest_colour" << "src_colour" 
            << "one_minus_dest_colour" << "one_minus_src_colour" 
            << "dest_alpha" << "src_alpha" << "one_minus_dest_alpha" 
            << "one_minus_src_alpha";

            params[propertyStr] = Ogre::String(blendTypes[value.toUInt()].toStdString());
        }
        else if(propertyStr == "visible") {
            ui->previewWindow->setLayerVisible(layerId, value.toBool());
        }
        else {
            params[propertyStr] = Ogre::String(value.toString().toStdString());
        }

        // update the layer with new parameter settings
        if(!params.empty()) {
            ui->previewWindow->updateLayer(layerId,params);
        }

        // refresh layer properties if necessary
        if(refresh) {
            // remove this layer's properties
            QList<QtBrowserItem *> bl = ui->layerProperties->topLevelItems();
            ui->layerProperties->removeProperty(bl[bl.size() - layerId - 1]->property());
            
            bl = ui->layerProperties->topLevelItems();

            // re-insert new properties
            if(!bl.empty() && layerId != bl.size()) {
                insertLayerProperties(ui->previewWindow->getLayers()[layerId],bl[bl.size() - layerId - 1]->property(),false);
            }
            else {
                insertLayerProperties(ui->previewWindow->getLayers()[layerId],0,false);
            }
        }
    }
}
    
void MainWindow::valueChanged(QtProperty *property, const QString &value)
{
    // don't update if we're refreshing
    if(mRefreshing) return;
    
    QList<QtProperty *> l = ui->layerProperties->properties();
    
    int topLevelIndex = 0;
    int layerId = -1;
    
    // find the layer that contains this property
    for(; topLevelIndex < l.size(); topLevelIndex++) {
        if(l[topLevelIndex] == property) {
            layerId = l.size() - 1 - topLevelIndex;
            break;
        }
        
        bool found = false;
        
        QList<QtProperty *> sl = l[topLevelIndex]->subProperties();
        if(!sl.empty()) {
            for(int id = 0; id < sl.size(); id++) {
                if(sl[id] == property) {
                    layerId = l.size() - 1 - topLevelIndex;
                    found = true;
                    break;
                }
            }
        }
        
        if(found) break;
    }
    
    // did we find a valid layer id?
    std::vector<Ogre::SpacescapeLayer *> layers = ui->previewWindow->getLayers();
    if(layerId > -1 && layerId < (int)l.size() && layerId < (int)layers.size()) {
        Ogre::NameValuePairList params;
        Ogre::String propertyStr = getProperty(property->propertyName());
        params[propertyStr] = Ogre::String(value.toStdString());
        ui->previewWindow->updateLayer(layerId,params);
    }
}

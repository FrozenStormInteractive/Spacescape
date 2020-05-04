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
#include "QtSpacescapeWidget.h"
#include <QMouseEvent>
//#include "OGRE/Ogre.h"
#include <Ogre.h>

#ifdef Q_WS_MAC
#include "macUtils.h"
#endif

using namespace Ogre;

const float QtSpacescapeWidget::mRADIUS = (float)0.8;

QtSpacescapeWidget::QtSpacescapeWidget(QWidget *parent) : QWidget(parent, Qt::WindowType::Widget),
    mProgressListener(nullptr),
    mOgreCtx("Spacescape")
{
	mSceneMgr = nullptr;
	mViewPort = nullptr;
	mMousePressed = false;

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
}

int QtSpacescapeWidget::addLayer(int type, const Ogre::NameValuePairList& params)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        int ret = plugin->addLayer(type, params);
        mOgreCtx.getRoot()->renderOneFrame();
        return ret;
    }

    return -1;
}

void QtSpacescapeWidget::clearLayers()
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        plugin->clear();
        mOgreCtx.getRoot()->renderOneFrame();
    }
}

int QtSpacescapeWidget::copyLayer(unsigned int layerID)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        int ret = plugin->duplicateLayer(layerID);
        mOgreCtx.getRoot()->renderOneFrame();
        return ret;
    }

    return -1;
}

bool QtSpacescapeWidget::deleteLayer(unsigned int layerID)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        bool ret = plugin->deleteLayer(layerID);
        mOgreCtx.getRoot()->renderOneFrame();
        return ret;
    }

    return false;
}

bool QtSpacescapeWidget::exportSkybox(const QString& filename, unsigned int imageSize, bool cubeMap, int orientation)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        plugin->writeToFile(
            Ogre::String(filename.toStdString()),
            imageSize,
            cubeMap ? Ogre::TEX_TYPE_CUBE_MAP : Ogre::TEX_TYPE_2D,
			(SpacescapePlugin::SpacescapeRTTOrientation)orientation
        );
        return true;
    }

    return false;
}

std::vector<Ogre::SpacescapeLayer *> QtSpacescapeWidget::getLayers()
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        return plugin->getLayers();
    }

    std::vector<Ogre::SpacescapeLayer *> layers;

    return layers;
}

Ogre::SpacescapePlugin* QtSpacescapeWidget::getPlugin()
{
    if(!mOgreCtx.getRoot()) {
        return nullptr;
    }

    static SpacescapePlugin plugin;
    return &plugin;
}

void QtSpacescapeWidget::paintEvent(QPaintEvent *e)
{
    if(!mOgreCtx.getRoot()) {
        mOgreCtx.injectMainWindow(windowHandle());
        mOgreCtx.useQtEventLoop(true);
        mOgreCtx.initApp();

        setupResources();
        setupScene();
        show();
    }

    mOgreCtx.getRoot()->renderOneFrame();
}

bool QtSpacescapeWidget::isHDREnabled()
{
    return pluginReady() ? getPlugin()->isHDREnabled() : false;
}

void QtSpacescapeWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (mMousePressed) {
        QPoint curPos = e->pos();
		
        double w = width();
        double h = height();
		
        double curX = (curPos.x() * 2. - w) / w;
        double curY = (h - curPos.y() * 2.) / h;
        double x0 = (mMousePressPos.x() * 2. - w) / w;
        double y0 = (h - mMousePressPos.y() * 2.) / h;
		
        Ogre::Vector3 v1(x0, y0, 0);
        Ogre::Vector3 v2(curX, curY, 0);
		
        double radiusSqr = mRADIUS * mRADIUS;
        double cutoff = radiusSqr * 0.5;
        double Rho = v1[0] * v1[0] + v1[1] * v1[1];
        v1[2] = (Rho < cutoff) ? sqrt(radiusSqr - Rho) : (cutoff / sqrt(Rho));
		
        Rho = v2[0] * v2[0] + v2[1] * v2[1];
        v2[2] = (Rho < cutoff) ? sqrt(radiusSqr - Rho) : (cutoff / sqrt(Rho));
		
        // v_cross is the normal of rotating plane
        Ogre::Vector3 cross = v2.crossProduct(v1);
        cross.normalise();

        // compute the angle
        v1.normalise();
        v2.normalise();
        double cosAngle = v1.dotProduct(v2);
        if (cosAngle < -1.0) {
            cosAngle = -1.0;
        } else if(cosAngle > 1.0) {
            cosAngle = 1.0;
        }
        double angle = acos(cosAngle);
		
        mCameraNode->rotate(cross, Ogre::Radian(angle));
		
        mMousePressPos = curPos;
        mLastCamOrientation = mCameraNode->getOrientation();

        mOgreCtx.getRoot()->renderOneFrame();
    }
}

void QtSpacescapeWidget::mousePressEvent(QMouseEvent *e)
{
    mMousePressPos = e->pos();
	if (mCameraNode) {
        mLastCamOrientation = mCameraNode->getOrientation();
	}

    mMousePressed = true;
}

void QtSpacescapeWidget::mouseReleaseEvent(QMouseEvent *)
{
    mMousePressed = false;
}

bool QtSpacescapeWidget::moveLayerDown(unsigned int layerID)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        bool ret = plugin->moveLayer(layerID,-1);
        mOgreCtx.getRoot()->renderOneFrame();
        return ret;
    }

    return false;
}

bool QtSpacescapeWidget::moveLayerUp(unsigned int layerID)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        bool ret = plugin->moveLayer(layerID,1);
        mOgreCtx.getRoot()->renderOneFrame();
        return ret;
    }

    return false;
}

bool QtSpacescapeWidget::open(const QString& filename)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        if(mProgressListener) {
            plugin->addProgressListener(mProgressListener);
        }
        return plugin->loadConfigFile(filename.toStdString());
    }

    return false;
}

bool QtSpacescapeWidget::pluginReady()
{
    return getPlugin() != nullptr;
}

void QtSpacescapeWidget::resizeEvent(QResizeEvent *e) {
    if(e->size() == e->oldSize()) {
        return;
    }

    if (mOgreCtx.getRoot()) {
        // Alter the camera aspect ratio to match the viewport
        mCamera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
    }

    //QTimer::singleShot(30, windowHandle(), SLOT(requestUpdate()));

    QWidget::resizeEvent(e);
}

bool QtSpacescapeWidget::save(const QString& filename)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        return plugin->saveConfigFile(filename.toStdString());
    }

    return false;
}

void QtSpacescapeWidget::setDebugBoxVisible(bool visible)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        plugin->setDebugBoxVisible(visible);
        mOgreCtx.getRoot()->renderOneFrame();
    }
}

void QtSpacescapeWidget::setHDREnabled(bool enabled)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        plugin->setHDREnabled(enabled);
        mOgreCtx.getRoot()->renderOneFrame();
    }
}

void QtSpacescapeWidget::setLayerVisible(unsigned int layerID, bool visible)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        plugin->setLayerVisible(layerID,visible);
        mOgreCtx.getRoot()->renderOneFrame();
    }
}

void QtSpacescapeWidget::setupResources()
{
#ifdef Q_OS_MAC
    Ogre::String resourcePath = Ogre::macBundlePath() + "/Contents/Resources/";
#else
    Ogre::String resourcePath = "../";
#endif
    
	// Load resource paths from config file
	Ogre::ConfigFile config;
    config.load(resourcePath + "resources.cfg");

    const Ogre::ConfigFile::SettingsBySection_& sections = config.getSettingsBySection();

    // Go through all sections & settings in the file
    for (const auto& section : sections) {

        Ogre::ConfigFile::SettingsMultiMap settings = section.second;

        for (const auto& setting : settings) {
            Ogre::String typeName = setting.first;
            Ogre::String archName = setting.second;
            Ogre::String secName;

#ifdef Q_OS_MAC
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            if (!Ogre::StringUtil::startsWith(archName, "/", false)) { // only adjust relative dirs
                archName = Ogre::String(resourcePath + archName);
            }
#endif

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }


	
	// Go through all sections & settings in the file

}

void QtSpacescapeWidget::setupScene()
{
	mSceneMgr = Ogre::Root::getSingleton().createSceneManager();

    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setNearClipDistance(0.1f);
    mCamera->setFarClipDistance(10000.0f);

    // Create one viewport, entire window
    mViewPort = mOgreCtx.getRenderWindow()->addViewport(mCamera);
    mViewPort->setBackgroundColour(Ogre::ColourValue(0,0,0));
    mViewPort->setClearEveryFrame(true);

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    // Populate scene
    {
        mSceneMgr->setAmbientLight(Ogre::ColourValue(0,0,0,0));
    }

    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(1);

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(mViewPort->getActualWidth()) / Ogre::Real(mViewPort->getActualHeight()));
    mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mCameraNode->attachObject(mCamera);
    
//    Ogre::NameValuePairList params;
//    addLayer(1, params);
}

bool QtSpacescapeWidget::updateLayer(unsigned int layerID, const Ogre::NameValuePairList& params)
{
    Ogre::SpacescapePlugin* plugin = getPlugin();
    if(plugin) {
        bool ret = plugin->updateLayer(layerID,params);
        mOgreCtx.getRoot()->renderOneFrame();
        return ret;
    }

    return false;
}

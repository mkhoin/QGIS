/***************************************************************************
    qgspythonutils.h - abstract interface for Python routines
    ---------------------
    begin                : October 2006
    copyright            : (C) 2006 by Martin Dobias
    email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSPYTHONUTILS_H
#define QGSPYTHONUTILS_H

// Needed for CMake variables defines
#include "qgsconfig.h"

#include "qgis_python.h"

#include <QString>
#include <QStringList>


class QgisInterface;
#ifdef  HAVE_SERVER_PYTHON_PLUGINS
class QgsServerInterface;
#endif


/**
 All calls to Python functions in QGIS come here.
 This class is a singleton.

 Default path for Python plugins is:
 - QgsApplication::qgisSettingsDirPath() + "/python/plugins"
 - QgsApplication::pkgDataPath() + "/python/plugins"

 */

class PYTHON_EXPORT QgsPythonUtils
{
  public:

    virtual ~QgsPythonUtils() {}

    //! returns true if Python support is ready to use (must be inited first)
    virtual bool isEnabled() = 0;

    //! initialize Python and import bindings
    virtual void initPython( QgisInterface *iface ) = 0;

#ifdef HAVE_SERVER_PYTHON_PLUGINS
    //! initialize Python and import server bindings
    virtual void initServerPython( QgsServerInterface *iface ) = 0;

    //! start server plugin: call plugin's classServerFactory(serverInterface) add to active plugins
    virtual bool startServerPlugin( QString packageName ) = 0;
#endif

    //! close Python interpreter
    virtual void exitPython() = 0;

    /* console */

    //! run a statement, show an error message on error
    //! \returns true if no error occurred
    virtual bool runString( const QString &command, QString msgOnError = QString(), bool single = true ) = 0;

    //! run a statement, error reporting is not done
    //! \returns true if no error occurred
    virtual bool runStringUnsafe( const QString &command, bool single = true ) = 0;

    virtual bool evalString( const QString &command, QString &result ) = 0;

    //! get information about error to the supplied arguments
    //! \returns false if there was no Python error
    virtual bool getError( QString &errorClassName, QString &errorText ) = 0;

    /* plugins */

    //! return list of all available Python plugins
    virtual QStringList pluginList() = 0;

    //! return whether the plugin is loaded (active)
    virtual bool isPluginLoaded( const QString &packageName ) = 0;

    //! return a list of active plugins
    virtual QStringList listActivePlugins() = 0;

    //! load Python plugin (import)
    virtual bool loadPlugin( const QString &packageName ) = 0;

    //! start plugin: add to active plugins and call initGui()
    virtual bool startPlugin( const QString &packageName ) = 0;

    //! helper function to get some information about plugin
    //! \param function one of these strings: name, tpye, version, description
    virtual QString getPluginMetadata( const QString &pluginName, const QString &function ) = 0;

    //! confirm that the plugin can be uninstalled
    virtual bool canUninstallPlugin( const QString &packageName ) = 0;

    //! unload plugin
    virtual bool unloadPlugin( const QString &packageName ) = 0;
};

#endif

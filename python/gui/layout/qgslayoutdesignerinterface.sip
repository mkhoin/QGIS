/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/gui/layout/qgslayoutdesignerinterface.h                          *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/




class QgsLayoutDesignerInterface: QObject
{
%Docstring
 A common interface for layout designer dialogs and widgets.

 Provides a common interface and stable API for layout designer dialogs and widgets.
 This interface can be used by plugins and scripts to interact with
 open layout designer dialogs.

.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgslayoutdesignerinterface.h"
%End
  public:

    QgsLayoutDesignerInterface( QObject *parent /TransferThis/ = 0 );
%Docstring
 Constructor for QgsLayoutDesignerInterface.
%End

    virtual ~QgsLayoutDesignerInterface();

    virtual QgsLayout *layout() = 0;
%Docstring
 Returns the layout displayed in the designer.
 :rtype: QgsLayout
%End

  public slots:

    virtual void close() = 0;
%Docstring
 Closes the layout designer.
%End

};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/gui/layout/qgslayoutdesignerinterface.h                          *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/

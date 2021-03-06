/***************************************************************************
                         qgsprocessingalgorithm.h
                         ------------------------
    begin                : December 2016
    copyright            : (C) 2016 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSPROCESSINGALGORITHM_H
#define QGSPROCESSINGALGORITHM_H

#include "qgis_core.h"
#include "qgis.h"
#include "qgsprocessingparameters.h"
#include "qgsprocessingoutputs.h"
#include "qgsprocessingcontext.h"
#include <QString>
#include <QVariant>
#include <QIcon>

class QgsProcessingProvider;
class QgsProcessingFeedback;
class QgsFeatureSink;
class QgsProcessingFeedback;


/**
 * \class QgsProcessingAlgorithm
 * \ingroup core
 * Abstract base class for processing algorithms.
  * \since QGIS 3.0
 */
class CORE_EXPORT QgsProcessingAlgorithm
{
  public:

    //! Flags indicating how and when an algorithm operates and should be exposed to users
    enum Flag
    {
      FlagHideFromToolbox = 1 << 1, //!< Algorithm should be hidden from the toolbox
      FlagHideFromModeler = 1 << 2, //!< Algorithm should be hidden from the modeler
      FlagSupportsBatch = 1 << 3,  //!< Algorithm supports batch mode
      FlagCanCancel = 1 << 4, //!< Algorithm can be canceled
      FlagRequiresMatchingCrs = 1 << 5, //!< Algorithm requires that all input layers have matching coordinate reference systems
      FlagDeprecated = FlagHideFromToolbox | FlagHideFromModeler, //!< Algorithm is deprecated
    };
    Q_DECLARE_FLAGS( Flags, Flag )

    /**
     * Constructor for QgsProcessingAlgorithm.
     */
    QgsProcessingAlgorithm() = default;

    virtual ~QgsProcessingAlgorithm();


    //! Algorithms cannot be copied - clone() should be used instead
    QgsProcessingAlgorithm( const QgsProcessingAlgorithm &other ) = delete;
    //! Algorithms cannot be copied- clone() should be used instead
    QgsProcessingAlgorithm &operator=( const QgsProcessingAlgorithm &other ) = delete;

    /**
     * Creates a copy of the algorithm, ready for execution.
     */
    virtual QgsProcessingAlgorithm *create() const = 0 SIP_FACTORY;

    /**
     * Returns the algorithm name, used for identifying the algorithm. This string
     * should be fixed for the algorithm, and must not be localised. The name should
     * be unique within each provider. Names should contain lowercase alphanumeric characters
     * only and no spaces or other formatting characters.
     * \see displayName()
     * \see group()
     * \see tags()
    */
    virtual QString name() const = 0;

    /**
     * Returns the unique ID for the algorithm, which is a combination of the algorithm
     * provider's ID and the algorithms unique name (e.g. "qgis:mergelayers" ).
     * \see name()
     * \see provider()
     */
    QString id() const;

    /**
     * Returns the translated algorithm name, which should be used for any user-visible display
     * of the algorithm name.
     * \see name()
     */
    virtual QString displayName() const = 0;

    /**
     * Returns a list of tags which relate to the algorithm, and are used to assist users in searching
     * for suitable algorithms. These tags should be localised.
    */
    virtual QStringList tags() const { return QStringList(); }

    /**
     * Returns a localised short helper string for the algorithm. This string should provide a basic description
     * about what the algorithm does and the parameters and outputs associated with it.
     * \see helpString()
     * \see helpUrl()
     */
    virtual QString shortHelpString() const;

    /**
     * Returns a localised help string for the algorithm. Algorithm subclasses should implement either
     * helpString() or helpUrl().
     * \see helpUrl()
     * \see shortHelpString()
     */
    virtual QString helpString() const;

    /**
     * Returns a url pointing to the algorithm's help page.
     * \see helpString()
     * \see shortHelpString()
     */
    virtual QString helpUrl() const;

    /**
     * Returns an icon for the algorithm.
     * \see svgIconPath()
    */
    virtual QIcon icon() const;

    /**
     * Returns a path to an SVG version of the algorithm's icon.
     * \see icon()
     */
    virtual QString svgIconPath() const;

    /**
     * Returns the name of the group this algorithm belongs to. This string
     * should be localised.
     * \see tags()
    */
    virtual QString group() const { return QString(); }

    /**
     * Returns the flags indicating how and when the algorithm operates and should be exposed to users.
     * Default flags are FlagSupportsBatch and FlagCanCancel.
     */
    virtual Flags flags() const;

    /**
     * Returns true if the algorithm can execute. Algorithm subclasses can return false
     * here to indicate that they are not able to execute, e.g. as a result of unmet
     * external dependencies. If specified, the \a errorMessage argument will be filled
     * with a localised error message describing why the algorithm cannot execute.
     */
    virtual bool canExecute( QString *errorMessage SIP_OUT = nullptr ) const;

    /**
     * Checks the supplied \a parameter values to verify that they satisfy the requirements
     * of this algorithm in the supplied \a context. The \a message parameter will be
     * filled with explanatory text if validation fails.
     * Overridden implementations should also check this base class implementation.
     * \returns true if parameters are acceptable for the algorithm.
     */
    virtual bool checkParameterValues( const QVariantMap &parameters,
                                       QgsProcessingContext &context, QString *message SIP_OUT = nullptr ) const;

    /**
     * Returns the provider to which this algorithm belongs.
     */
    QgsProcessingProvider *provider() const;

    /**
     * Returns an ordered list of parameter definitions utilized by the algorithm.
     * \see addParameter()
     * \see parameterDefinition()
     * \see destinationParameterDefinitions()
     */
    QgsProcessingParameterDefinitions parameterDefinitions() const { return mParameters; }

    /**
     * Returns a matching parameter by \a name. Matching is done in a case-insensitive
     * manner.
     * \see parameterDefinitions()
     */
    const QgsProcessingParameterDefinition *parameterDefinition( const QString &name ) const;

    /**
     * Returns the number of visible (non-hidden) parameters defined by this
     * algorithm.
     */
    int countVisibleParameters() const;

    /**
     * Returns a list of destination parameters definitions utilized by the algorithm.
     * \see QgsProcessingParameterDefinition::isDestination()
     * \see parameterDefinitions()
     */
    QgsProcessingParameterDefinitions destinationParameterDefinitions() const;

    /**
     * Returns an ordered list of output definitions utilized by the algorithm.
     * \see addOutput()
     * \see outputDefinition()
     */
    QgsProcessingOutputDefinitions outputDefinitions() const { return mOutputs; }

    /**
     * Returns a matching output by \a name. Matching is done in a case-insensitive
     * manner.
     * \see outputDefinitions()
     */
    const QgsProcessingOutputDefinition *outputDefinition( const QString &name ) const;

    /**
     * Returns true if this algorithm generates HTML outputs.
     */
    bool hasHtmlOutputs() const;

    /**
     * Executes the algorithm using the specified \a parameters. This method internally
     * creates a copy of the algorithm before running it, so it is safe to call
     * on algorithms directly retrieved from QgsProcessingRegistry and QgsProcessingProvider.
     *
     * The \a context argument specifies the context in which the algorithm is being run.
     *
     * Algorithm progress should be reported using the supplied \a feedback object.
     *
     * If specified, \a ok will be set to true if algorithm was successfully run.
     *
     * \returns A map of algorithm outputs. These may be output layer references, or calculated
     * values such as statistical calculations.
     *
     * \note this method can only be called from the main thread. Use prepare(), runPrepared() and postProcess()
     * if you need to run algorithms from a background thread, or use the QgsProcessingAlgRunnerTask class.
     */
    QVariantMap run( const QVariantMap &parameters,
                     QgsProcessingContext &context, QgsProcessingFeedback *feedback, bool *ok SIP_OUT = nullptr ) const;

    /**
     * Prepares the algorithm for execution. This must be run in the main thread, and allows the algorithm
     * to pre-evaluate input parameters in a thread-safe manner. This must be called before
     * calling runPrepared() (which is safe to do in any thread).
     * \see runPrepared()
     * \see postProcess()
     * \note This method modifies the algorithm instance, so it is not safe to call
     * on algorithms directly retrieved from QgsProcessingRegistry and QgsProcessingProvider. Instead, a copy
     * of the algorithm should be created with clone() and prepare()/runPrepared() called on the copy.
     */
    bool prepare( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback );

    /**
     * Runs the algorithm, which has been prepared by an earlier call to prepare().
     * This method is safe to call from any thread. Returns true if the algorithm was successfully executed.
     * After runPrepared() has finished, the postProcess() method should be called from the main thread
     * to allow the algorithm to perform any required cleanup tasks and return its final result.
     * \see prepare()
     * \see postProcess()
     * \note This method modifies the algorithm instance, so it is not safe to call
     * on algorithms directly retrieved from QgsProcessingRegistry and QgsProcessingProvider. Instead, a copy
     * of the algorithm should be created with clone() and prepare()/runPrepared() called on the copy.
     */
    QVariantMap runPrepared( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback );

    /**
     * Should be called in the main thread following the completion of runPrepared(). This method
     * allows the algorithm to perform any required cleanup tasks. The returned variant map
     * includes the results evaluated by the algorithm.
     * \note This method modifies the algorithm instance, so it is not safe to call
     * on algorithms directly retrieved from QgsProcessingRegistry and QgsProcessingProvider. Instead, a copy
     * of the algorithm should be created with clone() and prepare()/runPrepared() called on the copy.
     */
    QVariantMap postProcess( QgsProcessingContext &context, QgsProcessingFeedback *feedback );

    /**
     * If an algorithm subclass implements a custom parameters widget, a copy of this widget
     * should be constructed and returned by this method.
     * The base class implementation returns nullptr, which indicates that an autogenerated
     * parameters widget should be used.
     */
    virtual QWidget *createCustomParametersWidget( QWidget *parent = nullptr ) const SIP_FACTORY;

    /**
     * Creates an expression context relating to the algorithm. This can be called by algorithms
     * to create a new expression context ready for evaluating expressions within the algorithm.
     */
    QgsExpressionContext createExpressionContext( const QVariantMap &parameters,
        QgsProcessingContext &context ) const;

    /**
     * Checks whether the coordinate reference systems for the specified set of \a parameters
     * are valid for the algorithm. For instance, the base implementation performs
     * checks to ensure that all input CRS are equal
     * Returns true if \a parameters have passed the CRS check.
     */
    virtual bool validateInputCrs( const QVariantMap &parameters,
                                   QgsProcessingContext &context ) const;

    /**
     * Returns a Python command string which can be executed to run the algorithm
     * using the specified \a parameters.
     *
     * Algorithms which cannot be run from a Python command should return an empty
     * string.
     */
    virtual QString asPythonCommand( const QVariantMap &parameters, QgsProcessingContext &context ) const;

    /**
     * Associates this algorithm with its provider. No transfer of ownership is involved.
     */
    void setProvider( QgsProcessingProvider *provider );

  protected:

    /**
     * Adds a parameter \a definition to the algorithm. Ownership of the definition is transferred to the algorithm.
     * Returns true if parameter could be successfully added, or false if the parameter could not be added (e.g.
     * as a result of a duplicate name).
     * \see addOutput()
     */
    bool addParameter( QgsProcessingParameterDefinition *parameterDefinition SIP_TRANSFER );

    /**
     * Removes the parameter with matching \a name from the algorithm, and deletes any existing
     * definition.
     */
    void removeParameter( const QString &name );

    /**
     * Adds an output \a definition to the algorithm. Ownership of the definition is transferred to the algorithm.
     * Returns true if the output could be successfully added, or false if the output could not be added (e.g.
     * as a result of a duplicate name).
     * \see addParameter()
     */
    bool addOutput( QgsProcessingOutputDefinition *outputDefinition SIP_TRANSFER );

    /**
     * Prepares the algorithm to run using the specified \a parameters. Algorithms should implement
     * their logic for evaluating parameter values here. The evaluated parameter results should
     * be stored in member variables ready for a call to processAlgorithm().
     *
     * The \a context argument specifies the context in which the algorithm is being run.
     *
     * prepareAlgorithm should be used to handle any thread-sensitive preparation which is required
     * by the algorithm. It will always be called from the same thread that \a context has thread
     * affinity with. While this will generally be the main thread, it is not guaranteed. For instance,
     * algorithms which are run as a step in a larger model or as a subcomponent of a script-based algorithm
     * will call prepareAlgorithm from the same thread as that model/script it being executed in.
     *
     * Note that the processAlgorithm step uses a temporary context with affinity for the thread in
     * which the algorithm is executed, making it safe for processAlgorithm implementations to load
     * sources and sinks without issue. Implementing prepareAlgorithm is only required if special
     * thread safe handling is required by the algorithm.
     *
     * Algorithm preparation progress should be reported using the supplied \a feedback object. Additionally,
     * well-behaved algorithms should periodically check \a feedback to determine whether the
     * algorithm should be canceled and exited early.
     *
     * If the preparation was successful algorithms must return true. If a false value is returned
     * this indicates that the preparation could not be completed, and the algorithm execution
     * will be canceled.
     *
     * \returns true if preparation was successful.
     * \see processAlgorithm()
     * \see postProcessAlgorithm()
     */
    virtual bool prepareAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback ) SIP_VIRTUALERRORHANDLER( processing_exception_handler );

    /**
     * Runs the algorithm using the specified \a parameters. Algorithms should implement
     * their custom processing logic here.
     *
     * The \a context argument gives a temporary context with thread affinity matching the thread
     * in which the algorithm is being run. This is a cut-back copy of the context passed to
     * the prepareAlgorithm() and postProcessAlgorithm() steps, but it is generally safe
     * for most algorithms to utilize this context for loading layers and creating sinks.
     * Any loaded layers or sinks created within this temporary context will be transferred
     * back to the main execution context upon successful completion of the processAlgorithm()
     * step.
     *
     * Algorithm progress should be reported using the supplied \a feedback object. Additionally,
     * well-behaved algorithms should periodically check \a feedback to determine whether the
     * algorithm should be canceled and exited early.
     *
     * This method will not be called if the prepareAlgorithm() step failed (returned false).
     *
     * c++ implementations of processAlgorithm can throw the QgsProcessingException exception
     * to indicate that a fatal error occurred within the execution. Python based subclasses
     * should raise GeoAlgorithmExecutionException for the same purpose.
     *
     * \returns A map of algorithm outputs. These may be output layer references, or calculated
     * values such as statistical calculations. Unless the algorithm subclass overrides
     * the postProcessAlgorithm() step this returned map will be used as the output for the
     * algorithm.
     *
     * \see prepareAlgorithm()
     * \see postProcessAlgorithm()
     */
    virtual QVariantMap processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback ) = 0 SIP_VIRTUALERRORHANDLER( processing_exception_handler );

    /**
     * Allows the algorithm to perform any required cleanup tasks. The returned variant map
     * includes the results evaluated by the algorithm. These may be output layer references, or calculated
     * values such as statistical calculations.
     *
     * The \a context argument specifies the context in which the algorithm was run.
     *
     * Postprocess progress should be reported using the supplied \a feedback object. Additionally,
     * well-behaved algorithms should periodically check \a feedback to determine whether the
     * post processing should be canceled and exited early.
     *
     * postProcessAlgorithm should be used to handle any thread-sensitive cleanup which is required
     * by the algorithm. It will always be called from the same thread that \a context has thread
     * affinity with. While this will generally be the main thread, it is not guaranteed. For instance,
     * algorithms which are run as a step in a larger model or as a subcomponent of a script-based algorithm
     * will call postProcessAlgorithm from the same thread as that model/script it being executed in.
     *
     * postProcessAlgorithm will not be called if the prepareAlgorithm() step failed (returned false),
     * or if an exception was raised by the processAlgorithm() step.
     *
     * \returns A map of algorithm outputs. These may be output layer references, or calculated
     * values such as statistical calculations. Implementations which return a non-empty
     * map will override any results returned by processAlgorithm().
     *
     * \see prepareAlgorithm()
     * \see processAlgorithm()
     */
    virtual QVariantMap postProcessAlgorithm( QgsProcessingContext &context, QgsProcessingFeedback *feedback ) SIP_VIRTUALERRORHANDLER( processing_exception_handler );

    /**
     * Evaluates the parameter with matching \a name to a static string value.
     */
    QString parameterAsString( const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to an expression.
     */
    QString parameterAsExpression( const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a static double value.
     */
    double parameterAsDouble( const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a static integer value.
     */
    int parameterAsInt( const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a enum value.
     */
    int parameterAsEnum( const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to list of enum values.
     */
    QList<int> parameterAsEnums( const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a static boolean value.
     */
    bool parameterAsBool( const QVariantMap &parameters, const QString &name, const QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a feature sink.
     *
     * Sinks will either be taken from \a context's active project, or created from external
     * providers and stored temporarily in the \a context.
     *
     * The \a fields, \a geometryType and \a crs parameters dictate the properties
     * of the resulting feature sink.
     *
     * The \a destinationIdentifier argument will be set to a string which can be used to retrieve the layer corresponding
     * to the sink, e.g. via calling QgsProcessingUtils::mapLayerFromString().
     *
     * This function creates a new object and the caller takes responsibility for deleting the returned object.
     */
    QgsFeatureSink *parameterAsSink( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context, QString &destinationIdentifier SIP_OUT,
                                     const QgsFields &fields, QgsWkbTypes::Type geometryType = QgsWkbTypes::NoGeometry, const QgsCoordinateReferenceSystem &crs = QgsCoordinateReferenceSystem() ) const SIP_FACTORY;

    /**
     * Evaluates the parameter with matching \a name to a feature source.
     *
     * Sources will either be taken from \a context's active project, or loaded from external
     * sources and stored temporarily in the \a context.
     *
     * This function creates a new object and the caller takes responsibility for deleting the returned object.
     */
    QgsProcessingFeatureSource *parameterAsSource( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const SIP_FACTORY;

    /**
     * Evaluates the parameter with matching \a name to a source vector layer file path of compatible format.
     *
     * If the parameter is evaluated to an existing layer, and that layer is not of the format listed in the
     * \a compatibleFormats argument, then the layer will first be exported to a compatible format
     * in a temporary location. The function will then return the path to that temporary file.
     *
     * \a compatibleFormats should consist entirely of lowercase file extensions, e.g. 'shp'.
     *
     * The \a preferredFormat argument is used to specify to desired file extension to use when a temporary
     * layer export is required.
     */
    QString parameterAsCompatibleSourceLayerPath( const QVariantMap &parameters, const QString &name,
        QgsProcessingContext &context, const QStringList &compatibleFormats, const QString &preferredFormat = QString( "shp" ), QgsProcessingFeedback *feedback = nullptr );

    /**
     * Evaluates the parameter with matching \a name to a map layer.
     *
     * Layers will either be taken from \a context's active project, or loaded from external
     * sources and stored temporarily in the \a context. In either case, callers do not
     * need to handle deletion of the returned layer.
     */
    QgsMapLayer *parameterAsLayer( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a raster layer.
     *
     * Layers will either be taken from \a context's active project, or loaded from external
     * sources and stored temporarily in the \a context. In either case, callers do not
     * need to handle deletion of the returned layer.
     */
    QgsRasterLayer *parameterAsRasterLayer( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a output layer destination.
     */
    QString parameterAsOutputLayer( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a file based output destination.
     */
    QString parameterAsFileOutput( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a vector layer.
     *
     * Layers will either be taken from \a context's active project, or loaded from external
     * sources and stored temporarily in the \a context. In either case, callers do not
     * need to handle deletion of the returned layer.
     */
    QgsVectorLayer *parameterAsVectorLayer( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a coordinate reference system.
     */
    QgsCoordinateReferenceSystem parameterAsCrs( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a rectangular extent.
     */
    QgsRectangle parameterAsExtent( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a point.
     */
    QgsPointXY parameterAsPoint( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a file/folder name.
     */
    QString parameterAsFile( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a matrix/table of values.
     * Tables are collapsed to a 1 dimensional list.
     */
    QVariantList parameterAsMatrix( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a list of map layers.
     */
    QList< QgsMapLayer *> parameterAsLayerList( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a range of values.
     */
    QList<double> parameterAsRange( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

    /**
     * Evaluates the parameter with matching \a name to a list of fields.
     */
    QStringList parameterAsFields( const QVariantMap &parameters, const QString &name, QgsProcessingContext &context ) const;

  private:

    QgsProcessingProvider *mProvider = nullptr;
    QgsProcessingParameterDefinitions mParameters;
    QgsProcessingOutputDefinitions mOutputs;
    bool mHasPrepared = false;
    bool mHasExecuted = false;
    bool mHasPostProcessed = false;
    std::unique_ptr< QgsProcessingContext > mLocalContext;

    // friend class to access setProvider() - we do not want this public!
    friend class QgsProcessingProvider;
    friend class TestQgsProcessing;
    friend class QgsProcessingModelAlgorithm;

#ifdef SIP_RUN
    QgsProcessingAlgorithm( const QgsProcessingAlgorithm &other );
#endif

};
Q_DECLARE_OPERATORS_FOR_FLAGS( QgsProcessingAlgorithm::Flags )



#endif // QGSPROCESSINGALGORITHM_H


